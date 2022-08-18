main()
{
	int	r;
	r = ini_readinteger("a.txt", "DBG_INFO", "DBG_LEVEL", 9);
	printf("%d\n", r);
}
