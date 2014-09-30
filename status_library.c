// Written by amoghbl1

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUF_SIZE 1024
char value[BUF_SIZE];
int concatI;
char * getValueFromKey(char *key, char *path) {
    FILE *fd = fopen(path, "r");
    int i = 0;
    char tmp[1000], buf;
    while(1) {
        i = 0;
        buf = fgetc(fd);
        while(buf != ':') { 
            tmp[i++] = buf;
            buf = fgetc(fd);
        }
        tmp[i] = '\0';
        
        if(!strcmp(tmp, key)) {
            i = 0;
            while(buf != '\n') {
                if(buf != '\t')
                    value[i++] = buf;
                buf = fgetc(fd);
            }
            value[i] = '\0';
            return value;
        }
        else {
            while(buf != '\n')
                buf = fgetc(fd);
        }   
    }
}

int main(int argc, char* argv[]) {
    char *pid, path[100];
    FILE *fd;
    path[0] = '\0';
    if(argc < 2) {
        printf("Error: Invalid number of arguments.\n");
        exit(EXIT_FAILURE);
    }
    pid = argv[1];
    strcat(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/status");
    fd = fopen(path, "r");
    if(fd == NULL) {
        printf("Error: Invalid pid.\n");
        exit(EXIT_FAILURE);
    }
    printf("Pid: %s", pid);
    printf(" PPid");
    printf("%s", getValueFromKey("PPid", path));
    printf(" Threads");
    printf("%s", getValueFromKey("Threads", path));
    printf("\n");
    return 0;
}

