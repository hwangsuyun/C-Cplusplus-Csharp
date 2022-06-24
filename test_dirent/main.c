#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

void concat_path(char *dest, char* src1, char* src2){
        strcpy(dest,src1);
        strcat(dest,"/");
        strcat(dest,src2);
}
void read_dirent(char *dir_name){

        DIR* dir = opendir(dir_name);
        if(dir == NULL){
                printf("디렉토리를 열수 없습니다.\n");
                return;
        }

        struct dirent *entry;
        while((entry=readdir(dir))!=NULL){
                if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0) continue;
                struct stat buf;
                char full_path[256];

                concat_path(full_path, dir_name, entry->d_name);

                printf("%s\n",full_path);

                if(lstat(full_path, &buf) <0)
                        printf("lstat error\n");
                else{
                        if(S_ISDIR(buf.st_mode))        //디렉토리일 경우 재귀 호출
                                read_dirent(full_path);

                }

        }

        closedir(dir);
}
int main(int argc, char *argv[]){
        if(argc != 2){
                printf("1개의 directory path가 필요합니다. \n");
                return 1;
        }
        //오른쪽에 /가 들어올 수 있음. ex) /etc/ -> /etc
        if(argv[1][strlen(argv[1])-1] == '/') argv[1][strlen(argv[1])-1]='\0';

        read_dirent(argv[1]);
}