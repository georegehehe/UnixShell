#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("count is: %d\n", count);
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


int main(){
    char str[30] = "aasdfja >asdf";
    
    char **new = parse(str, ">");
    int i = 0;
    while(new[i] != NULL){
        printf("new: %s, %d\n", new[i], i);
        
        char **arg = parse(new[i], " ");
        int j = 0;
        /*
        while(arg[j] != NULL){

            printf("%s", arg[j]);
            j++;
        }
        */
        
        i++;
    }
    char *strin[3] = {"abcdef","bcdefg",NULL};

    /*
    int count = 0;
    int boo = 0;
    int i = 0;
    while(str[i] != '\0'){
        if (!boo && str[i] != ';'){
            boo = 1;
            count++;
        } 
        if (str[i] == ';' && boo)
            boo = 0;
        i++;
    }
    char *ret = strtok(str, ";");
    printf("%d\n", count);
    while (ret != NULL){
        printf("%d, %s", count, ret);
        ret = strtok(NULL, ";");
        count++;
    }
    */
    
}