#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *cur_path, char *target) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512], *p;

    if((fd = open(cur_path, 0)) < 0) {
        fprintf(2, "ERROR: cannot open the path: %s\n", cur_path);
        exit(1);
    }

    strcpy(buf, cur_path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) continue;
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        
        if(stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", de.name);
            continue;
        }

        switch (st.type) {
        case T_DIR:
            if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
            find(buf, target);
            break;
        case T_DEVICE:
        case T_FILE:
            if(!strcmp(de.name, target)) {
                printf("%s/%s\n", cur_path, de.name);
            }
            break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc < 3) {
        printf("ERROR: Need more arguments\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}