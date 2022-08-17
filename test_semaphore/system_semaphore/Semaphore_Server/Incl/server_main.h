int server_main(int sockFd);
int checkSemShmKey(PROTOCOL *data, int *temp_semid, int *temp_shmid, int *index);
int writeInitFile(int isShared);
