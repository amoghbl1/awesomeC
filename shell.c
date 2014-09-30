#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
void parseCommand(char c[], char **p) {
    int i = 0, j = 0, pos = 0;
    char t[100], ch;
    while(1) {
        j=0;
        ch=c[i++];
        while(ch!=' ' && ch!='\0') {
            t[j++] = ch;
            ch = c[i++];
        }
        while(ch==' '){
            ch = c[i++];
        }
        i--;
        t[j] = '\0';
        p[pos++] = strdup(t);
        if(ch=='\0')
            break;
    }
    p[pos] = '\0';
}
void sig_handler(int signo)
{
    pid_t thisprocess;
    int statet;
    thisprocess=getpid();
    if (signo == SIGINT)
        write(1, "Please use exit to exit the shell.\nCSHELL$ ", 43);
    else if (signo == SIGKILL)
        write(1, "Please use exit to exit the shell.\nCSHELL$ ", 43);
    return;
}

stripFirst(char *arg[]) {
    int i = 1;
    char ch = arg[i];
    while(ch!='\0') {
        arg[i-1] = arg[i];
        ch = arg[++i];
    }
    arg[i] = '\0';
    puts(arg);
}

int main(int argc, char *argv[]) {
    char command[1000];
    int pid, status, t, pflag, inFlag, outFlag, outFile, inFile, saveOut, getIn;
    char *in, *out;
    char *args[1000];
    char *args2[1000];
    while(1) {
        signal (SIGTTOU, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            write(2,"\ncan't catch SIGINT\n",20);
            continue;
        }
        if (signal(SIGQUIT, sig_handler) == SIG_ERR)
        {
            write(2,"\ncan't catch SIGKILL\n",21);
            continue;
        }
        
        printf("CSHELL$ ");
        gets(command);
        parseCommand(command, args);
        if(!strcmp(args[0], "cd")) {
            chdir(args[1]);
            continue;
        }
        else if(args[0][0] == 12) {
            stripFirst(args[0]);
            system("clear");
            continue;
        }
        else if(!strcmp(args[0], "exit")) {
            break;
        }
        pid = fork();
        if (pid!=0)
            waitpid(pid, &status, WUNTRACED);
        else {
            //parseCommand(command, args);
            inFlag = outFlag = 0;
            t=0;
            pflag = 0;
            while(args[t]!='\0') {
                if(pflag) {
                    if(pflag == 2) {
                        in = args[t];
                        inFlag = 1;
                    }
                    else if(pflag == 1) {
                        out = args[t];
                        if(!strcmp(args[t-1], ">>"))
                            outFlag = 1;
                        else
                            outFlag = 2;
                    }
                    t++;
                    pflag = 0;
                    continue;
                }
                if(!strcmp(args[t], ">") || !strcmp(args[t], ">>")) {
                    pflag = 1;
                    t++;
                    continue;
                }
                else if(!strcmp(args[t], "<")) {
                    pflag = 2;
                    t++;
                    continue;
                }
                args2[t] = args[t];
                t++;
            }
            args2[t] = '\0';
            if(outFlag) {
                if(access(out, F_OK)!=0) {
                    printf("Error, don't have permissions to write to file");
                    break;
                }
                if(outFlag == 1)
                    outFile = open(out, O_RDWR|O_CREAT|O_APPEND, 0777);
                else if(outFlag == 2)
                    outFile = open(out, O_CREAT | O_WRONLY, 0777);
                if (-1 == outFile) { 
                    perror("problem redirecting output"); 
                    return 255; 
                }
                saveOut = dup(fileno(stdout));
                dup2(outFile, fileno(stdout));
            }
            if(inFlag) {
                if(access(in, F_OK)!=0) {
                    printf("Error, don't have permissions to read to file, or something like that...\n");
                    break;
                }
                inFile = open(in,O_RDONLY);
                if(-1 == inFile) {
                    perror("problem getting input"); 
                    return 255; 
                }
                getIn = dup(fileno(stdin));
                dup2(inFile, fileno(stdin));
            }
            t =  execvp(args2[0], args2);
            if(outFlag) {
                fflush(stdout); 
                close(outFile);
            }
            if(inFlag) {
                dup2(getIn, fileno(stdin));
                close(inFile);
            }
            if(t==-1)
                printf("No such command!\n"); 
            break;
        }
    }
}
