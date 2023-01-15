#include <stdio.h>
#include <sys/utsname.h>
 
int main(void) {
    struct utsname system_info;
 
    puts("------ struct utsname ------");
    puts("struct utsname {");
    puts("    char sysname[];");
    puts("    char nodename[];");
    puts("    char release[];");
    puts("    char version[];");
    puts("    char machine[];");
    puts("#ifdef _GNU_SOURCE");  
    puts("    char domainname[];");   
    puts("#endif");
    puts("};");
    puts("----------------------------");
    puts("");
 
    printf("return value is %d \n\n", uname(&system_info));
                                   // 시스템 정보를 가져옴과 동시에 반환값 확인
 
    printf("System name - %s \n", system_info.sysname);
    printf("Nodename    - %s \n", system_info.nodename);
    printf("Release     - %s \n", system_info.release);
    printf("Version     - %s \n", system_info.version);
    printf("Machine     - %s \n", system_info.machine);
 // printf("Domain name - %s \n", system_info.domainname);
 
    return 0;
}
