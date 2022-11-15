#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NETSNMP_TRAPHANDLER_FLAG_MATCH_TREE     0x1
#define NETSNMP_TRAPHANDLER_FLAG_STRICT_SUBTREE 0x2
#define NETSNMPTRAPD_HANDLER_BREAK   3
#define NETSNMPTRAPD_HANDLER_FINISH  4

static int lastlookup;
char            ddefault_port[] = "udp:162";    /* Default default port */
char            *default_port = ddefault_port;
int 		    running = 1;
const char *our_v3_passphrase = "password";
netsnmp_pdu *gPdu = NULL;
struct sockaddr_in *agent; 
typedef struct netsnmp_trapd_handler_s netsnmp_trapd_handler;
typedef int (Netsnmp_Trap_Handler)(netsnmp_pdu           *pdu,
		netsnmp_transport     *transport,
		netsnmp_trapd_handler *handler);

struct netsnmp_trapd_handler_s {
	oid  *trapoid;
	int   trapoid_len;
	char *token;               /* Or an array of tokens? */
	char *format;              /* Formatting string */
	int   version;             /* ??? */
	int   authtypes;
	int   flags;
	Netsnmp_Trap_Handler *handler;
	void *handler_data;

	netsnmp_trapd_handler *nexth;      /* Next handler for this trap */
	/* Doubly-linked list of traps with registered handlers */
	netsnmp_trapd_handler *prevt;
	netsnmp_trapd_handler *nextt;
};

netsnmp_trapd_handler *netsnmp_auth_global_traphandlers   = NULL;
netsnmp_trapd_handler *netsnmp_pre_global_traphandlers    = NULL;
netsnmp_trapd_handler *netsnmp_post_global_traphandlers   = NULL;
netsnmp_trapd_handler *netsnmp_default_traphandlers  = NULL;
netsnmp_trapd_handler *netsnmp_specific_traphandlers = NULL;

typedef struct netsnmp_handler_map_t {
	netsnmp_trapd_handler **handler;
	const char             *descr;
} netsnmp_handler_map;

static netsnmp_handler_map handlers[] = {
	{ &netsnmp_auth_global_traphandlers, "auth trap" },
	{ &netsnmp_pre_global_traphandlers, "pre-global trap" },
	{ NULL, "trap specific" },
	{ &netsnmp_post_global_traphandlers, "global" },
	{ NULL, NULL }
};

int
netsnmp_trapd_check_auth(int authtypes)
{
    if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_APP_NO_AUTHORIZATION)) {
        DEBUGMSGTL(("snmptrapd:auth", "authorization turned off\n"));
        return 1;
    }

    DEBUGMSGTL(("snmptrapd:auth",
                "Comparing auth types: result=%d, request=%d, result=%d\n",
                lastlookup, authtypes,
                ((authtypes & lastlookup) == authtypes)));
    return ((authtypes & lastlookup) == authtypes);
}

netsnmp_trapd_handler *
netsnmp_get_traphandler( oid *trapOid, int trapOidLen ) {
	netsnmp_trapd_handler *traph;

	if (!trapOid || !trapOidLen) {
		DEBUGMSGTL(( "snmptrapd:lookup", "get_traphandler no OID!\n"));
		return NULL;
	}
	DEBUGMSGTL(( "snmptrapd:lookup", "Looking up Trap OID: "));
	DEBUGMSGOID(("snmptrapd:lookup", trapOid, trapOidLen));
	DEBUGMSG(( "snmptrapd:lookup", "\n"));

	/*
	 * Look for a matching OID, and return that list...
	 */
	for (traph = netsnmp_specific_traphandlers;
			traph; traph=traph->nextt ) {

		/*
		 * If the trap handler wasn't wildcarded, then the trapOID
		 *   should match the registered OID exactly.
		 */
		if (!(traph->flags & NETSNMP_TRAPHANDLER_FLAG_MATCH_TREE)) {
			if (snmp_oid_compare(traph->trapoid, traph->trapoid_len,
						trapOid, trapOidLen) == 0) {
				DEBUGMSGTL(( "snmptrapd:lookup",
							"get_traphandler exact match (%p)\n", traph));
				return traph;
			}
		} else {
			/*
			 * If the trap handler *was* wildcarded, then the trapOID
			 *   should have the registered OID as a prefix...
			 */
			if (snmp_oidsubtree_compare(traph->trapoid,
						traph->trapoid_len,
						trapOid, trapOidLen) == 0) {
				if (traph->flags & NETSNMP_TRAPHANDLER_FLAG_STRICT_SUBTREE) {
					/*
					 * ... and (optionally) *strictly* as a prefix
					 *   i.e. not including an exact match.
					 */
					if (snmp_oid_compare(traph->trapoid, traph->trapoid_len,
								trapOid, trapOidLen) != 0) {
						DEBUGMSGTL(( "snmptrapd:lookup", "get_traphandler strict subtree match (%p)\n", traph));
						return traph;
					}
				} else {
					DEBUGMSGTL(( "snmptrapd:lookup", "get_traphandler subtree match (%p)\n", traph));
					return traph;
				}
			}
		}
	}

	/*
	 * .... or failing that, return the "default" list (which may be NULL)
	 */
	DEBUGMSGTL(( "snmptrapd:lookup", "get_traphandler default (%p)\n",
				netsnmp_default_traphandlers));
	return netsnmp_default_traphandlers;
}

int snmp_input(int op, netsnmp_session *session,
		int reqid, netsnmp_pdu *pdu, void *magic)
{
	oid stdTrapOidRoot[] = { 1, 3, 6, 1, 6, 3, 1, 1, 5 };
	oid snmpTrapOid[]    = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
	oid trapOid[MAX_OID_LEN+2] = {0};
	int trapOidLen;
	netsnmp_variable_list *vars;
	netsnmp_trapd_handler *traph;
	netsnmp_transport *transport = (netsnmp_transport *) magic;
	int ret, idx;

	printf("op(%d)\n", op);
	switch (op) {
		case NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE:
			/*
			 * Drops packets with reception problems
			 */
			if (session->s_snmp_errno) {
				/* drop problem packets */
				return 1;
			}
			/*
			 * Determine the OID that identifies the trap being handled
			 */
			printf("snmptrapd: %x\n", pdu->command);
			printf("snmptrapd: %d\n", pdu->command);
			switch (pdu->command) {
				case SNMP_MSG_TRAP:
					printf("trap msg\n");
					if (pdu->trap_type == SNMP_TRAP_ENTERPRISESPECIFIC) {
						trapOidLen = pdu->enterprise_length;
						memcpy(trapOid, pdu->enterprise, sizeof(oid) * trapOidLen);
						if (trapOid[trapOidLen - 1] != 0) {
							trapOid[trapOidLen++] = 0;
						}
						trapOid[trapOidLen++] = pdu->specific_type;
					} else {
						memcpy(trapOid, stdTrapOidRoot, sizeof(stdTrapOidRoot));
						trapOidLen = OID_LENGTH(stdTrapOidRoot);  /* 9 */
						trapOid[trapOidLen++] = pdu->trap_type+1;
					}
					break;
				case SNMP_MSG_TRAP2:
				case SNMP_MSG_INFORM:
					printf("trap2 msg\n");
					printf("inform msg\n");
					vars = pdu->variables;
					if (vars)
						vars = vars->next_variable;
					if (!vars || snmp_oid_compare(vars->name, vars->name_length,
								snmpTrapOid, OID_LENGTH(snmpTrapOid))) {
						/*
						 * Didn't find it!
						 * Let's look through the full list....
						 */
						for ( vars = pdu->variables; vars; vars=vars->next_variable) {
							if (!snmp_oid_compare(vars->name, vars->name_length,
										snmpTrapOid, OID_LENGTH(snmpTrapOid)))
								break;
						}
						if (!vars) {
							/*
							 * Still can't find it!  Give up.
							 */
							snmp_log(LOG_ERR, "Cannot find TrapOID in TRAP2 PDU\n");
							return 1;       /* ??? */
						}
					}
					memcpy(trapOid, vars->val.objid, vars->val_len);
					trapOidLen = vars->val_len /sizeof(oid);
					break;
				default :
					printf("default\n");
					break;
			}
/*
			for( idx = 0; handlers[idx].descr; ++idx ) {
				DEBUGMSGTL(("snmptrapd", "Running %s handlers\n",
							handlers[idx].descr));
				printf("snmptrapd Running %s handlers\n", handlers[idx].descr);
				if (NULL == handlers[idx].handler) // specific //
					traph = netsnmp_get_traphandler(trapOid, trapOidLen);
				else
					traph = *handlers[idx].handler;

				for( ; traph; traph = traph->nexth) {
					if (!netsnmp_trapd_check_auth(traph->authtypes))
						continue; // we continue on and skip this one //

					ret = (*(traph->handler))(pdu, transport, traph);
					if(NETSNMPTRAPD_HANDLER_FINISH == ret)
						return 1;
					if (ret == NETSNMPTRAPD_HANDLER_BREAK)
						break; // move on to next type //
				} // traph //
			} // handlers //
*/

	}

	//gPdu = pdu;
	//gPdu = NULL;
	gPdu = snmp_clone_pdu(pdu);

	char temp[100];
	memset(temp, 0x00, sizeof(temp));
	snprint_objid(temp, sizeof(temp), gPdu->enterprise, gPdu->enterprise_length);
	printf("temp(%s)\n", temp);
	
	memset(temp, 0x00, sizeof(temp));
	snprint_objid(temp, sizeof(temp), trapOid, trapOidLen);
	printf("temp(%s)\n", temp);

	/*
	   agent = (struct sockaddr_in *)&(gPdu->agent_addr);

	   printf("*******************************************\n");
	   printf("version(%d)\n", gPdu->version);
	   printf("command(%d)\n", gPdu->command);
	   printf("agent_addr(%u)\n", gPdu->agent_addr);
	//printf("OIP(%s)\n", inet_ntoa(agent->sin_addr));
	printf("securityModel(%d)\n", gPdu->securityModel);
	printf("community(%u)\n", gPdu->community);
	printf("community_len(%d)\n", gPdu->community_len);
	printf("contextEngineID(%s)\n", gPdu->contextEngineID);
	printf("contextName(%s)\n", gPdu->contextName);
	printf("securityEngineID(%s)\n", gPdu->securityEngineID);
	printf("securityName(%s)\n", gPdu->securityName);
	printf("*******************************************\n");
	*/
	printf("snmp_input test\n");
}

	static void
snmptrapd_close_sessions(netsnmp_session * sess_list)
{
	netsnmp_session *s = NULL, *next = NULL;

	for (s = sess_list; s != NULL; s = next) {
		next = s->next;
		snmp_close(s);
	}
}

	static int
pre_parse(netsnmp_session * session, netsnmp_transport *transport,
		void *transport_data, int transport_data_length)
{
#if NETSNMP_USE_LIBWRAP
	char *addr_string = NULL;

	if (transport != NULL && transport->f_fmtaddr != NULL) {
		/*
		 * Okay I do know how to format this address for logging.  
		 */
		addr_string = transport->f_fmtaddr(transport, transport_data,
				transport_data_length);
		/*
		 * Don't forget to free() it.  
		 */
	}

	if (addr_string != NULL) {
		/* Catch udp,udp6,tcp,tcp6 transports using "[" */
		char *tcpudpaddr = strstr(addr_string, "[");
		if ( tcpudpaddr != 0 ) {
			char sbuf[64];
			char *xp;

			strlcpy(sbuf, tcpudpaddr + 1, sizeof(sbuf));
			xp = strstr(sbuf, "]");
			if (xp)
				*xp = '\0';

			if (hosts_ctl("snmptrapd", STRING_UNKNOWN,
						sbuf, STRING_UNKNOWN) == 0) {
				DEBUGMSGTL(("snmptrapd:libwrap", "%s rejected", addr_string));
				SNMP_FREE(addr_string);
				return 0;
			}
		}
		SNMP_FREE(addr_string);
	} else {
		if (hosts_ctl("snmptrapd", STRING_UNKNOWN,
					STRING_UNKNOWN, STRING_UNKNOWN) == 0) {
			DEBUGMSGTL(("snmptrapd:libwrap", "[unknown] rejected"));
			return 0;
		}
	}
#endif/*  NETSNMP_USE_LIBWRAP  */
	return 1;
}

	static netsnmp_session *
snmptrapd_add_session(netsnmp_transport *t)
{
	netsnmp_session sess, *session = &sess, *rc = NULL;

	snmp_sess_init(session);
	session->peername = SNMP_DEFAULT_PEERNAME;  /* Original code had NULL here
	*/
	session->version = SNMP_DEFAULT_VERSION;
	session->community_len = SNMP_DEFAULT_COMMUNITY_LEN;
	session->retries = SNMP_DEFAULT_RETRIES;
	session->timeout = SNMP_DEFAULT_TIMEOUT;
	session->callback = snmp_input; 
	session->callback_magic = NULL; 
	session->authenticator = NULL;
	sess.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;

	//rc = snmp_add(session, t, pre_parse, NULL);
	rc = snmp_add(session, t, NULL, NULL);
	if (rc == NULL) {
		snmp_sess_perror("snmptrapd", session);
	}
	return rc;
}

	static void
snmptrapd_main_loop()
{
	int             count, numfds, block;
	fd_set          readfds,writefds,exceptfds;
	struct timeval  timeout;
	NETSNMP_SELECT_TIMEVAL timeout2;

	while (running) {
		numfds = 0;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		block = 0;
		timerclear(&timeout);
		timeout.tv_sec = 5;
		snmp_select_info(&numfds, &readfds, &timeout, &block);
		timeout2.tv_sec = timeout.tv_sec;
		timeout2.tv_usec = timeout.tv_usec;
		count = select(numfds, &readfds, &writefds, &exceptfds,
				!block ? &timeout2 : NULL);
		if (count > 0) {
			/* If there are any more events after external events, then
			 * try SNMP events. */
			snmp_read(&readfds);

			if (gPdu == NULL)
			{
				printf("NULL\n");
			}
			else
			{
				printf("NOT NULL\n");
				printf("*******************************************\n");
				printf("version(%d)\n", gPdu->version);
				printf("command(%d)\n", gPdu->command);
				printf("agent_addr(%u)\n", gPdu->agent_addr);
				//printf("OIP(%s)\n", inet_ntoa(agent->sin_addr));
				printf("securityModel(%d)\n", gPdu->securityModel);
				printf("community(%u)\n", gPdu->community);
				printf("community_len(%d)\n", gPdu->community_len);
				printf("contextEngineID(%s)\n", gPdu->contextEngineID);
				printf("contextName(%s)\n", gPdu->contextName);
				printf("securityEngineID(%s)\n", gPdu->securityEngineID);
				printf("securityName(%s)\n", gPdu->securityName);
				printf("*******************************************\n");
			}
		} else {
			switch (count) {
				case 0:
					snmp_timeout();
					break;
				case -1:
					if (errno == EINTR)
						continue;
					snmp_log_perror("select");
					running = 0;
					break;
				default:
					fprintf(stderr, "select returned %d\n", count);
					running = 0;
			}
		}
		//run_alarms();
	}
}


int main()
{
	netsnmp_session *sess_list = NULL, *ss = NULL;
	netsnmp_transport *transport = NULL;
	int             arg, i = 0;
	int             uid = 0, gid = 0;
	int             exit_code = 1;
	char           *cp, *listen_ports = NULL;
	char listen_port[10];

	//init_usm_conf("snmptrapd"); // v3
	/*
	   netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
	   NETSNMP_DS_LIB_DONT_READ_CONFIGS, 1);

	   netsnmp_ds_set_string(NETSNMP_DS_LIBRARY_ID,
	   NETSNMP_DS_LIB_OPTIONALCONFIG, "/usr/local/etc/snmp/snmptrapd.conf");
	   */
	//init_agent("snmptrapd"); // v3

	init_snmp("");
	//init_snmp("snmptrapd");

	//SOCK_STARTUP;

	memset(listen_port, 0x00, sizeof(listen_port));
	sprintf(listen_port, "%d", 10004);
	//memcpy(listen_port, "10004", sizeof(listen_port));

	listen_ports = "10004";
	if (listen_ports)
		cp = listen_ports;
	else
		cp = default_port;
	printf("1\n");
	//transport = netsnmp_transport_open_server("snmptrap", cp);
	transport = netsnmp_transport_open_server("snmptrap", listen_port);
	printf("2\n");
	if (transport == NULL) {
		snmp_log(LOG_ERR, "couldn't open %s -- errno %d (\"%s\")\n",
				cp, errno, strerror(errno));
		snmptrapd_close_sessions(sess_list);
		goto sock_cleanup;
	} else {
		printf("3\n");	
		ss = snmptrapd_add_session(transport);
		printf("4\n");
		if (ss == NULL) {
			/*
			 * Shouldn't happen?  We have already opened the transport
			 * successfully so what could have gone wrong?  
			 */
			snmptrapd_close_sessions(sess_list);
			snmp_log(LOG_ERR, "couldn't open snmp - %s", strerror(errno));
			goto sock_cleanup;
		} else {
			ss->next = sess_list;
			sess_list = ss;
		}
	}
	printf("5\n");
	//	SNMP_FREE(listen_ports);

	snmptrapd_main_loop();

	snmptrapd_close_sessions(sess_list);
	snmp_shutdown("snmptrapd");

sock_cleanup:
	SOCK_CLEANUP;

	return 0;
}
