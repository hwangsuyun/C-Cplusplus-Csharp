int serverMain(int sockFd);
int checkSemShmKey(PROTOCOL *data, int *temp_semid, int *temp_shmid, int *index);
int writeInitFile(int isShared);
