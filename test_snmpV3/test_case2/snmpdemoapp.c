#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string.h>

/* change the word "define" to "undef" to try the (insecure) SNMPv1 version */
//#define DEMO_USE_SNMP_VERSION_3

#ifdef DEMO_USE_SNMP_VERSION_3
const char *our_v3_passphrase = "password";
#endif

int main(int argc, char ** argv)
{
    //netsnmp_session session, *ss;
    //netsnmp_pdu *pdu;
    //netsnmp_pdu *response;
	struct snmp_session aSession;
    //struct snmp_session session, *ss;
    struct snmp_session *session = &aSession, *ss;
    struct snmp_pdu *pdu;
    struct snmp_pdu *response;

    oid anOID[MAX_OID_LEN];
    size_t anOID_len;

    netsnmp_variable_list *vars;
    int status;
    int count=1;

    /*
     * Initialize the SNMP library
     */
    //init_snmp("snmpdemoapp");
    //init_snmp(NULL); // segmantation fault
    init_snmp(""); 
    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    //snmp_sess_init( &session );                   /* set up defaults */
    snmp_sess_init( session );                   /* set up defaults */
    //session.peername = strdup("test.net-snmp.org");
    //session.peername = strdup("localhost:161"); // success 
    //session->peername = strdup("localhost:161"); // success 
    //session.peername = strdup("localhost"); // success
    session->peername = SNMP_DEFAULT_PEERNAME; // success 
    //session.remote_port = (u_short)161; // deprecated

    /* set up the authentication parameters for talking to the server */

#ifdef DEMO_USE_SNMP_VERSION_3
    /* Use SNMPv3 to talk to the experimental server */
printf("V3\n");
    /* set the SNMP version number */
    session.version=SNMP_VERSION_3;
        
    /* set the SNMPv3 user name */
    session.securityName = strdup("user");
    session.securityNameLen = strlen(session.securityName);

    /* set the security level to authenticated, but not encrypted */
    //session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV; // authenticated, not encrypted
    session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV; // authenticated, encrypted

    /* set the authentication method to MD5 */
    //session.securityAuthProto = usmHMACMD5AuthProtocol;
    //session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
    /* set the authentication method to SHA */
    session.securityAuthProto = usmHMACSHA1AuthProtocol;
    session.securityAuthProtoLen = sizeof(usmHMACSHA1AuthProtocol)/sizeof(oid);
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

    /* set the encrypted method to key */
	session.securityPrivProto = usmAESPrivProtocol; // AES
	//session.securityPrivProto = usmDESPrivProtocol; // DES
	session.securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN; // AES
	//session.securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN; // DES
	session.securityPrivKeyLen = USM_PRIV_KU_LEN;

    /* set the authentication key to a MD5 hashed version of our
       passphrase "The Net-SNMP Demo Password" (which must be at least 8
       characters long) */
    if (generate_Ku(session.securityAuthProto,
                    session.securityAuthProtoLen,
                    (u_char *) our_v3_passphrase, strlen(our_v3_passphrase),
                    session.securityAuthKey,
                    &session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
        snmp_perror(argv[0]);
		snmp_log(LOG_ERR,
                 "Error generating Ku from authentication pass phrase. \n");
        exit(1);
    }
   
    // set priKey
    if (generate_Ku(session.securityAuthProto,
                    session.securityAuthProtoLen,
                    (u_char *) our_v3_passphrase, strlen(our_v3_passphrase),
                    session.securityPrivKey,
                    &session.securityPrivKeyLen) != SNMPERR_SUCCESS) {
        snmp_perror(argv[0]);
		snmp_log(LOG_ERR,
                 "Error generating Ku from priKey pass phrase. \n");
        exit(1);
    }
    
#else /* we'll use the insecure (but simplier) SNMPv1 */

printf("V1\n");
    /* set the SNMP version number */
    //session.version = SNMP_VERSION_1;
    session->version = SNMP_VERSION_1;

    /* set the SNMPv1 community name used for authentication */
    //session.community = "demopublic";
    /*
	session.community = "public";
    session.community_len = strlen(session.community);
	session.retries = SNMP_DEFAULT_RETRIES;
	session.timeout = SNMP_DEFAULT_TIMEOUT;
	//session.local_port = 161;
	session.callback = NULL;
	session.callback_magic = NULL;
	session.authenticator = NULL;
	session.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;
	*/
	session->community = "public";
	session->community_len = strlen(session->community);
	session->retries = SNMP_DEFAULT_RETRIES;
	session->timeout = SNMP_DEFAULT_TIMEOUT;
	//session->local_port = 161;
	session->callback = NULL; 
	session->callback_magic = NULL;
	session->authenticator = NULL;
	session->isAuthoritative = SNMP_SESS_UNKNOWNAUTH;
#endif /* SNMPv1 */

    /*
     * Open the session
     */
    SOCK_STARTUP;
    //ss = snmp_open(&session);                     /* establish the session */
    ss = snmp_open(session);                     /* establish the session */
    if (!ss) {
      //snmp_sess_perror("ack", &session);
      snmp_sess_perror("ack", session);
      SOCK_CLEANUP;
      exit(1);
    }
    
    /*
     * Create the PDU for the data for our request.
     *   1) We're going to GET the system.sysDescr.0 node.
     */
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    anOID_len = MAX_OID_LEN;
    //if (!snmp_parse_oid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len)) {
    if (!snmp_parse_oid("1.3.6.1.4.1.2021.4.2.0", anOID, &anOID_len)) {
      snmp_perror(".1.3.6.1.2.1.1.1.0");
      SOCK_CLEANUP;
      exit(1);
    }
#if OTHER_METHODS
    /*
     *  These are alternatives to the 'snmp_parse_oid' call above,
     *    e.g. specifying the OID by name rather than numerically.
     */
    read_objid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len);
    get_node("sysDescr.0", anOID, &anOID_len);
    read_objid("system.sysDescr.0", anOID, &anOID_len);
#endif

    snmp_add_null_var(pdu, anOID, anOID_len);
  
    /*
     * Send the Request out.
     */
printf("before send\n");
    status = snmp_synch_response(ss, pdu, &response);
printf("after send : (%d)\n", status);

    /*
     * Process the response.
     */
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
      /*
       * SUCCESS: Print the result variables
       */

      for(vars = response->variables; vars; vars = vars->next_variable)
        print_variable(vars->name, vars->name_length, vars);

      /* manipuate the information ourselves */
      for(vars = response->variables; vars; vars = vars->next_variable) {
        if (vars->type == ASN_OCTET_STR) {
	  char *sp = (char *)malloc(1 + vars->val_len);
	  memcpy(sp, vars->val.string, vars->val_len);
	  sp[vars->val_len] = '\0';
          printf("value #%d is a string: %s\n", count++, sp);
	  free(sp);
	}
        else
          printf("value #%d is NOT a string! Ack!\n", count++);
      }
    } else {
      /*
       * FAILURE: print what went wrong!
       */

      if (status == STAT_SUCCESS)
        fprintf(stderr, "Error in packet\nReason: %s\n",
                snmp_errstring(response->errstat));
      else if (status == STAT_TIMEOUT)
        //fprintf(stderr, "Timeout: No response from %s.\n",
        //        session.peername);
        fprintf(stderr, "Timeout: No response from %s.\n",
                session->peername);
      else
        snmp_sess_perror("snmpdemoapp", ss);
    }

    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
    if (response)
      snmp_free_pdu(response);
    snmp_close(ss);

    SOCK_CLEANUP;
    return (0);
} /* main() */
