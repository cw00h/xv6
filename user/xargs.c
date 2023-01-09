#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void xargs(int argc, char *argv[]);
int readline(int fd, char *buf, int maxlen);

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("ERROR: Need more arguments\n");
        exit(1);
    }

    xargs(argc, argv);

    exit(0);
}

void xargs(int argc, char *argv[]){
    char *new_argv[MAXARG];
    char buf[100];
    int len, arg_num;

    for(arg_num = 0; arg_num < argc - 1; arg_num++) new_argv[arg_num] = argv[arg_num + 1];
    
    while((len = readline(0, buf, 100)) != 0) {
        new_argv[arg_num] = buf;
        if(fork() == 0) {
            exec(new_argv[0], new_argv);
        }
        else {
            wait(0);
        }
    }
}

int readline(int fd, char *buf, int maxlen) {
    int n, rc;
    char c, *bufp = buf;

    for(n = 1; n < maxlen; n++) {
        if((rc = read(fd, &c, 1)) == 1) {
            if(c == '\n') {
                break;
            }
            *bufp++ = c;
        }
        else if(rc == 0) { /* EOF */
            if(n == 1) return 0; /* No data was read */
            else break; /* Some data was read */
        }
        else { /* Error */
            return -1;
        }
    }
    *bufp = 0;
    return n - 1;
}