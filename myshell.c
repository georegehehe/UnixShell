#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

char *trim_space(char *str)
{
    int i = 0;
    while(str[i] != '\0' && str[i] != '\n'){
        i++;
    }
    char *ret = (char *)malloc(sizeof(char) * (i+1));
    for (i = 0; str[i] != '\0' && str[i] != '\n'; i++){
        ret[i] = str[i];
    }
    ret[i] = '\0';
    return ret;
}

char **parse(char *str, const char *c)
{
    int i = 0;
    char *temp = str;
    int boo = 0;
    int count = 0;
       while(temp[i] != '\0'){
        if (!boo && str[i] != c[0]){
            boo = 1;
            count++;
        } 
        if (temp[i] == c[0] && boo)
            boo = 0;
        i++;
    }
    char **out = (char **)malloc(sizeof(char *) * (count + 1));
    char *buffer = strtok(str, c);
    i = 0;
    while(buffer != NULL){
        if (i == count - 1){
            out[i] = trim_space(buffer);
        } else{
            out[i] = strdup(buffer);
        }
        buffer = strtok(NULL, c);
        i++;
    }
    out[i] =  NULL;
    return out;
}

void free_arg(char **arg){
    int i = 0;
    while(arg[i] != NULL){
        free(arg[i]);
        i++;
    }
    free(arg);
}

int is_empty(char *str){
    int i = 0;
    while (str[i] != '\0'){
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
            return 0;
        }
        i++;
    }
    return 1;
}

char *save_file(FILE *fl){
    char *out = (char *)malloc(sizeof(char) * 501);
    char c;
    int currlen = 500;
    int i = 0;
    while((c = getc(fl)) != EOF){
        if (i >= currlen){
            out = realloc(out, currlen + 500);
            currlen += 500;
        }
        out[i] = c;
        i++;
    }
    out[i] = '\0';
    return out;
    
}

int in_str(char c, char *str){
    int i = 0;
    while (str[i] != '\0'){
        if(str[i] == c)
            return 1;
        i++;
    }
    return 0;
}

int char_count(char c, char *str){
    int i = 0;
    int count = 0;
    while (str[i] != '\0'){
        if(str[i] == c)
            count++;
        i++;
    }
    return count;
} 

void tab_switch(char *str){
    int i = 0;
    while(str[i] != '\0'){
        if (str[i] == '\t'){
            str[i] = ' ';
        }
        i++;
    }
}

int main(int argc, char *argv[]) 
{
    char cmd_buff[550];
    char error_msg[30] = "An error has occurred\n";
    char *pinput;
    int child_status;
    int ret;
    int batch = 0;
    int long_str = 0;
    int save_out;
    FILE *fout = stdin;
    if (argc > 1){
        if (argc > 2){
            myPrint(error_msg);
            exit(1);
        }
        batch = 1;
        if (!(fout = fopen(argv[1], "r"))){
            myPrint(error_msg);
            exit(1);
        }
    }
    while (1) {
        if (!batch && !long_str)
            myPrint("myshell> ");
        pinput = fgets(cmd_buff, 550, fout);
        if (strlen(cmd_buff) > 513){
            long_str = 1;
            if (cmd_buff[strlen(cmd_buff)-1] != '\n'){
                myPrint(cmd_buff);
                continue;
            }
        } 
        if (long_str){
            myPrint(cmd_buff);
            myPrint(error_msg);
            long_str = 0;
            continue;
        }
        if (!pinput) {
            exit(0);
        }
        if (batch && !is_empty(cmd_buff))
            myPrint(cmd_buff);
        char **cmdlst = parse(cmd_buff, ";");
        int i = 0;
        while(cmdlst[i] != NULL){
            char *saved = NULL;
            tab_switch(cmdlst[i]);
            char *cmd_saved = strdup(cmdlst[i]);
            int fd;
            char **redir_lst = parse(cmdlst[i], ">");
            if (char_count('>', cmd_saved) > 1){
                myPrint(error_msg);
                free(cmd_saved);
                free_arg(redir_lst);
                i++;
                continue;
            } else if (redir_lst[0] == NULL || is_empty(redir_lst[0])){
                if(in_str('>',cmd_saved)){
                    myPrint(error_msg);
                }
                free(cmd_saved);
                free_arg(redir_lst);
                i++;
                continue;
            } else if(redir_lst[1] == NULL && in_str('>',cmd_saved)){
                myPrint(error_msg);
                free(cmd_saved);
                free_arg(redir_lst);
                i++;
                continue;
            } else if(redir_lst[1] != NULL && is_empty(redir_lst[1])){
                myPrint(error_msg);
                free(cmd_saved);
                free_arg(redir_lst);
                i++;
                continue;
            } else if(redir_lst[1] != NULL && redir_lst[1][0] == '+' && is_empty(redir_lst[1]+1)){
                myPrint(error_msg);
                free(cmd_saved);
                free_arg(redir_lst);
                i++;
                continue;
            }
            char **arglst = parse(redir_lst[0], " ");
            if (arglst[0] == NULL){
                free_arg(redir_lst);
                free_arg(arglst);
                i++;
                continue;
            }
            if (strcmp(arglst[0], "pwd") == 0){
                if(arglst[1] != NULL || in_str('>', cmd_saved)){
                    free_arg(redir_lst);
                    free_arg(arglst);
                    myPrint(error_msg);
                    free(cmd_saved);
                    i++;
                    continue;
                }
                char pwd_buff[100];
                if(getcwd(pwd_buff, sizeof(pwd_buff)) != NULL){
                    myPrint(pwd_buff);
                    myPrint("\n");
                }

            }else if(strcmp(arglst[0], "exit") == 0){
                if(arglst[1] != NULL || in_str('>', cmd_saved)){
                    free_arg(redir_lst);
                    free_arg(arglst);
                    free(cmd_saved);
                    myPrint(error_msg);
                    i++;
                    continue;
                }
                free_arg(redir_lst);
                free_arg(arglst);
                exit(0);
            }else if(strcmp(arglst[0], "cd") == 0){
                if(arglst[1] == NULL && !in_str('>', cmd_saved)){
                    chdir(getenv("HOME"));
                } else{
                    if(arglst[2] != NULL || in_str('>', cmd_saved)){
                        free_arg(redir_lst);
                        free_arg(arglst);
                        free(cmd_saved);
                        myPrint(error_msg);
                        i++;
                        continue;
                    }
                    if (chdir(arglst[1]) < 0){
                        free_arg(redir_lst);
                        free_arg(arglst);
                        free(cmd_saved);
                        myPrint(error_msg);
                        i++;
                        continue;
                    }
                    
                }
            }else {
                int advanced = 0;
                int redir = 0;
                if (redir_lst[1] != NULL)
                {   
                    redir = 1;
                    if(redir_lst[1][0] == '+'){
                        advanced = 1;
                    }
                    char **no_space = parse(redir_lst[1], " ");
                    FILE *f_rd;
                    if (!advanced){
                        if(no_space[1] != NULL){
                            myPrint(error_msg);
                            free(no_space);
                            free_arg(redir_lst);
                            free_arg(arglst);
                            i++;
                            continue;
                        }
                        fd = open(no_space[0], O_WRONLY|O_CREAT|O_EXCL, 0664);
                        if (fd < 0){
                            myPrint(error_msg);
                            free(no_space);
                            free_arg(redir_lst);
                            free_arg(arglst);
                            close(fd);
                            i++;
                            continue;
                        }
                    } else{
                        if (strcmp("+", no_space[0]) == 0){
                            if(no_space[2] != NULL){
                                myPrint(error_msg);
                                free(no_space);
                                free_arg(redir_lst);
                                free_arg(arglst);
                                i++;
                                continue;
                            }
                            if ((f_rd = fopen(no_space[1], "r"))){
                                saved = save_file(f_rd);
                            }
                            fd = open(no_space[1], O_WRONLY|O_CREAT|O_TRUNC, 0664);
                        } else{
                            if(no_space[1] != NULL){
                                free(no_space);
                                free_arg(redir_lst);
                                free_arg(arglst);
                                myPrint(error_msg);
                                i++;
                                continue;
                            }
                            if ((f_rd = fopen(no_space[1], "r"))){
                                saved = save_file(f_rd);
                            }

                            if ((fd = open(no_space[0]+1, O_WRONLY|O_CREAT|O_TRUNC, 0664)) < 0){
                                free(no_space);
                                free_arg(redir_lst);
                                free_arg(arglst);
                                myPrint(error_msg);
                                i++;
                                continue;
                            }
                        }
                    }
                    free(no_space);
                    save_out = dup(1);
                    if (advanced){
                        if (f_rd)
                            fclose(f_rd);
                    }
                    dup2(fd, STDOUT_FILENO);
                } 
                if ((ret = fork()) == 0){
                    if (execvp(arglst[0], arglst) < 0){
                        myPrint(error_msg);
                    }
                    exit(0);
                } else {
                    waitpid(ret, &child_status, 0);
                    if(advanced){
                        if (saved != NULL && saved[0] != '\0'){
                            myPrint(saved);
                            free(saved);
                        }
                        advanced = 0;
                    }
                    if (redir){
                        dup2(save_out,1);
                        close(fd);
                    }
                }
            }
            if (cmd_saved != NULL){
                free(cmd_saved);
            }
            free_arg(redir_lst);
            free_arg(arglst);
            i++;
        }
        free_arg(cmdlst);
    }
}
