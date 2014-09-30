// Written by amoghbl1

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <stdlib.h>
#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define CONCAT_BUF_SIZE 10240000
#define BUF_SIZE 1024
char concatStr[CONCAT_BUF_SIZE], value[BUF_SIZE];
int concatI;
char* concat(char *str1,char *str2) {
    int i = concatI;
    while(*str1)
        concatStr[concatI++] = *str1++;
    while(*str2)
        concatStr[concatI++] = *str2++;
    concatStr[concatI] = '\0';
    concatI++;
    return concatStr+i;
}
int getStrLen(char *str) {
    int ret = 0;
    while(str[ret++] != '\0');
    return ret-1;
}
void writeStr(char *str) {
    write(1, str, getStrLen(str));
}
void writeChar(char str) {
        write(1, &str, 1);
}
int strEql(char *a, char *b) {
    int l1, l2;
    l1 = getStrLen(a);
    l2 = getStrLen(b);
    if(l1 != l2)
        return 0;
    for(l1 = 0; l1 < l2; l1++)
        if(a[l1] != b[l1])
            return 0;
    return 1;
}
char * getValueFromKey(char *key, char *path) {
    int fd, i = 0;
    char tmp[1000], buf[1];
    fd = open(path, O_RDONLY);
    if(fd == -1) {
        writeStr("Error: Invalid pid.\n");
        exit(EXIT_FAILURE);
    }
    while(1) {
        i = 0;
        read(fd, buf, 1);
        while(buf[0] != ':') { 
            tmp[i++] = buf[0];
            read(fd, buf, 1);
        }
        tmp[i] = '\0';
        
        if(strEql(tmp, key)) {
            i = 0;
            while(buf[0] != '\n') {
                if(buf[0] != '\t')
                    value[i++] = buf[0];
                read(fd, buf, 1);
            }
            value[i] = '\0';
            return value;
        }
        else {
            while(buf[0] != '\n')
                read(fd, buf, 1);
        }   
    }
}

int main(int argc, char* argv[]) {
    char *pid, *path;
    if(argc < 2) {
        writeStr("Error: Invalid number of arguments.\n");
        exit(EXIT_FAILURE);
    }
    pid = argv[1];
    path = concat(concat("/proc/", pid), "/status");
    writeStr(concat("Pid:", pid));
    writeStr(" PPid");
    writeStr(getValueFromKey("PPid", path));
    writeStr(" Threads");
    writeStr(getValueFromKey("Threads", path));
    writeStr("\n");
    exit(EXIT_SUCCESS);
}

