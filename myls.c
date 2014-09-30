#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include<sys/types.h>
#include<sys/stat.h>
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define writeNewLine()write(1, "\n", 1);
#define writeTabSpace()write(1, "    ", 4);
#define writeArrow()write(1, " -> ", 4);
#define BUF_SIZE 1024
#define CONCAT_BUF_SIZE 10240000
int normalYearMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int leapYearMonths[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char *monthsOfYear[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};
struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};
char concatStr[CONCAT_BUF_SIZE];
int l, a, h, concatI;
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
void revStr(char *str) {
   int i = 0;
   char temp;
   int j = getStrLen(str) - 1;
   while (i < j) {
       temp = str[i];
       str[i] = str[j];
       str[j] = temp;
       i++;
       j--;
   } 
}
int getIntLen(int num) {
    int ret = 0;
    while(num != 0) {
        num /= 10;
        ret++;
    }
    return ret;
}
void writeInt(int num) {
    char string[100];
    int i = 0;
    string[0] = '0';
    string[1] = '\0';
    while(num!=0) {
        string[i++]=num%10+'0';
        num /= 10;
    }
    if(i)
        string[i] = '\0';
    revStr(string);
    writeStr(string);
}
void writeMyFloat(float num){
    char str[3];
    str[1] = '.';
    str[0] = (int)num%10+'0';
    num = num*10;
    str[2] = (int)num%10+'0';
    writeStr(str);
}
void writeBytesHuman(long long size) {
    char tag;
    float out;
    int count = 0;
    if(size > 1000){ //KB
        if(size > 1000000){ //MB
            if(size > 1000000000){ //GB
                tag = 'G';
                out = (float)size/1000000000;
            }
            else {
                out = (float)size/1000000;
                tag = 'M';
            }
        }
        else {
            out = (float)size/1000;
            tag = 'K';
        }
    } 
    else {
        count = getIntLen(size);
        count = (count == 0) ? 1 : count;
        while(count != 4) {
            writeChar(' ');
            count++;
        }
        writeInt(size);
        return;
    }
    if(out<10) 
        writeMyFloat(out);
    else {
        count = getIntLen(out);
        while(count != 3) {
            writeChar(' ');
            count++;
        }
        writeInt(out);
    }
    writeChar(tag);
}
void writePermissions(struct stat fileStat) {
    (S_ISDIR(fileStat.st_mode)) ? writeChar('d') :writeChar('-');
    (fileStat.st_mode & S_IRUSR) ? writeChar('r') :writeChar('-');
    (fileStat.st_mode & S_IWUSR) ? writeChar('w') : writeChar('-');
    (fileStat.st_mode & S_IXUSR) ? writeChar('x') : writeChar('-');
    (fileStat.st_mode & S_IRGRP) ? writeChar('r') : writeChar('-');
    (fileStat.st_mode & S_IWGRP) ? writeChar('w') : writeChar('-');
    (fileStat.st_mode & S_IXGRP) ? writeChar('x') : writeChar('-');
    (fileStat.st_mode & S_IROTH) ? writeChar('r') : writeChar('-');
    (fileStat.st_mode & S_IWOTH) ? writeChar('w') : writeChar('-');
    (fileStat.st_mode & S_IXOTH) ? writeChar('x') : writeChar('-');
}
long int getIntFromStr(char *str) {
    int len = getStrLen(str);
    int i = 0;
    long int ret = 0;
    while(i < len){
        ret = ret*10;
        ret += str[i++] - '0';
    }
    return ret;
}
void getUserByID(char *name, long int ugid, char* filename) {
    int file, i = 0, flag = 0;
    char buf[1];
    char id[100];
    file = open(filename, O_RDONLY);
    while(!flag) {
        i = 0;
        read(file, buf, 1);
        while(buf[0]!=':') {
            name[i++] = buf[0];
            read(file, buf, 1);
        }
        name[i] = '\0'; 
        read(file, buf, 1);
        while(buf[0]!=':') {
            read(file, buf, 1);
        }
        i = 0;
        read(file, buf, 1);
        while(buf[0]!=':') {
            id[i++] = buf[0];
            read(file, buf, 1);
        }
        id[i] = '\0';
        while(buf[0]!='\n')
            read(file, buf, 1);
        if(ugid == getIntFromStr(id))
            flag = 1;
    }
}
int isLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100)) || (year % 400 == 0);
}
long long int getSecsInYear(int year) {
    long long int ret = isLeapYear(year) ? 31622400 : 31536000;
    return ret;
}
void writeTimeFromSeconds(long long int secs) {
    struct timezone tz;
    int hrs, mins;
    gettimeofday(NULL, &tz);
    hrs = (((long long)((secs)/3600))%24);
    (hrs<10) ? writeChar('0') : NULL;
    writeInt(hrs);
    writeChar(':');
    mins = (((long long)((secs)/60))%60);
    (mins<10) ? writeChar('0') : NULL;
    writeInt(mins);
}
long long int getMonthDiff(long long int s1, long long int s2) {
    // Assuming 30 day months.
    s1 -= s2;
    if(s1 < 1)
        return 0;
    return s1 / 2592000;
}
long long int getYearFromSeconds(long long int secs) {
    int year;
    long long int s = getSecsInYear(year);
    for (year = 1970;secs > s; ) {
        secs -= s;
        year++;
        s = getSecsInYear(year);
    }
    return year-1;
}
long long int getSecsFromYearStart(long long int secs) {
    int year;
    long long int s = getSecsInYear(year);
    for (year = 1970;secs > s; ) {
        secs -= s;
        year++;
        s = getSecsInYear(year);
    }
    return secs;
}
long long int secsInMonth(long long int secs, int month) {
    long long int ret = (isLeapYear(getYearFromSeconds(secs))) ? leapYearMonths[month] * 86400: normalYearMonths[month] * 86400;
    return ret;
}
void writeMonthFromSeconds(long long int secs) {
    int month = 0;
    long long int secsFromStart;
    secsFromStart = getSecsFromYearStart(secs);
    for (month = 0;secsFromStart > secsInMonth(secs, month);month++)
        secsFromStart -= secsInMonth(secs, month);
    writeStr(monthsOfYear[month]);
}
void writeDateFromSeconds(long long int secs) {
    long long int secsFromStart;
    int daysFromStart, month = 0;
    float t;
    secsFromStart = getSecsFromYearStart(secs);
    daysFromStart = t = (float)secsFromStart / (float)86400;
    if(t > daysFromStart)
        daysFromStart+=2;
    if(isLeapYear(getYearFromSeconds(secs))) {
        while(daysFromStart > leapYearMonths[month])
            daysFromStart -= leapYearMonths[month++];
    }
    else
        while(daysFromStart > normalYearMonths[month])
            daysFromStart -= normalYearMonths[month++];
    if(daysFromStart < 10)
        writeChar(' ');
    writeInt(daysFromStart);
}
void writeModificationDetails(long long int secs) {
    struct timeval tv;
    struct timezone tz;
    long long int currentTime;
    int diff;
    gettimeofday(&tv, &tz);
    diff = tz.tz_minuteswest * 60;
    writeMonthFromSeconds(secs - diff);
    writeChar(' ');
    writeDateFromSeconds(secs - diff);
    writeChar(' ');
    if (getMonthDiff((long long)tv.tv_sec - diff, secs - diff) < 6) {
         writeTimeFromSeconds(secs - diff);
         return;
    }
    writeInt(getYearFromSeconds(secs));
}
void writeUserGroup(struct stat fileStat) {
    char name[100];
    char *uidFile = "/etc/passwd";
    char *gidFile = "/etc/group";
    long long int year, days, diff;
    getUserByID(name, (long) fileStat.st_uid, uidFile);
    writeStr(name);
    writeTabSpace();
    getUserByID(name, (long) fileStat.st_gid, gidFile);
    writeStr(name);
}
int main(int argc, char *argv[]) {
    int fd, nread;
    ssize_t r;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;
    char *temp, *dirname, *fname;
    char tag[1], linkname[100];
    tag[0] = '/';
    int i = 0, dir_flag = 0, tlen, j;
    struct stat sb;
    l = 0;
    a = 0;
    h = 0;
    for(i = 1; i < argc; i++) {
        temp = argv[i];
        if(temp[0] == '-') {
            tlen = getStrLen(argv[i]);
            for(j = 1; j < tlen; j++){
                switch(temp[j]){
                    case 'l':
                        l++;
                        break;
                    case 'a':
                        a++;
                        break;
                    case 'h':
                        h++;
                }
            }
        }
        else {
            dirname = argv[i];
            dir_flag = 1;
        }
        // printf("%d %d %d", l, a, h);
    }
    // concat kinda acts like malloc here
    fname = concat(dirname, "");
    dirname = dir_flag ? dirname : ".";
    dirname = concat(dirname, tag);
    fd = open(dirname, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        if (open(fname, O_RDONLY) == -1) {
            writeStr("open: Not a file or directory\n");
        }
        if(!l)
            writeStr(fname);
        else {
            lstat(fname, &sb);
            writePermissions(sb);
            writeTabSpace();
            writeInt((long)sb.st_nlink);
            writeTabSpace();
            writeUserGroup(sb);
            writeTabSpace();
            writeModificationDetails(sb.st_mtim.tv_sec);
            writeTabSpace();
            if(h)
                writeBytesHuman((long long)sb.st_size);
            else
                writeInt((long long)sb.st_size);
            writeTabSpace();
            switch (sb.st_mode & S_IFMT) {
                case S_IFLNK:
                    r = readlink(fname, linkname, sb.st_size + 1);
                    if (r == -1) handle_error("lstat");
                    if (r > sb.st_size) handle_error("lstat1");
                    linkname[r] = '\0';
                    writeStr(fname);
                    writeArrow();
                    writeStr(linkname);
                    break;
                default:
                    writeStr(fname);
            }
        }
        writeNewLine();            
        exit(EXIT_SUCCESS);
    }

    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            d_type = *(buf + bpos + d->d_reclen - 1);

            if(!l){
                if(d->d_name[0] == '.' && a >= 1) {
                    writeStr(d->d_name);
                    writeTabSpace();
                }
                else if(d->d_name[0] != '.') {
                    writeStr(d->d_name);
                    writeTabSpace();
                }
            }
            else {
                if(d->d_name[0] == '.' && a>=1) {
                lstat(concat(dirname, d->d_name), &sb);
                writePermissions(sb);
                writeTabSpace();
                writeInt((long)sb.st_nlink);
                writeTabSpace();
                writeUserGroup(sb);
                writeTabSpace();
                writeModificationDetails(sb.st_mtim.tv_sec);
                writeTabSpace();
                if(h)
                    writeBytesHuman((long long)sb.st_size);
                else
                    writeInt((long long)sb.st_size);
                writeTabSpace();
                switch (sb.st_mode & S_IFMT) {
                    case S_IFLNK:
                    r = readlink(concat(dirname, d->d_name), linkname, sb.st_size + 1);
                    if (r == -1) handle_error("lstat");
                    if (r > sb.st_size) handle_error("lstat1");
                    linkname[r] = '\0';
                    writeStr(d->d_name);
                    writeArrow();
                    writeStr(linkname);
                    break;
                    default:
                    writeStr(d->d_name);
                    } 
                writeNewLine();
                }
                else if(d->d_name[0] != '.') {
                lstat(concat(dirname, d->d_name), &sb);
                writePermissions(sb);
                writeTabSpace();
                writeInt((long)sb.st_nlink);
                writeTabSpace();
                writeUserGroup(sb);
                writeTabSpace();
                writeModificationDetails(sb.st_mtim.tv_sec);
                writeTabSpace();
                if(h)
                    writeBytesHuman((long long)sb.st_size);
                else
                    writeInt((long long)sb.st_size);
                writeTabSpace();
                switch (sb.st_mode & S_IFMT) {
                    case S_IFLNK:
                    r = readlink(concat(dirname, d->d_name), linkname, sb.st_size + 1);
                    if (r == -1) handle_error("lstat");
                    if (r > sb.st_size) handle_error("lstat1");
                    linkname[r] = '\0';
                    writeStr(d->d_name);
                    writeArrow();
                    writeStr(linkname);
                    break;
                    default:
                    writeStr(d->d_name);
                    } 
                writeNewLine();
                }
            }
            bpos += d->d_reclen;
        }
        if(!l)
            writeNewLine();
    }   
    exit(EXIT_SUCCESS);
}
