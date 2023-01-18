//In the name of Allah
#include <stdio.h>
#include <io.h>
#include <string.h>

int parse(){
    return 1;
}

int isFileExist(char *pth){
    FILE *f;
    if (f = fopen(pth, "r")){
        fclose(f);
        return 1;
    }
    return 0;
}

int createFile(char *pth){
    if (isFileExist(pth))
        return -1;
    int len = strlen(pth);
    char cur[1000];
    //pth: "root/sth/A.txt"
    //create directories
    for (int i = 0; i < len; i ++){
        cur[i] = pth[i];
        cur[i + 1] = '\0';
        if (cur[i] == '/'){
            cur[i + 1] = '\0';
            mkdir(cur);
        }
    }
    FILE *f = fopen(pth, "w");
    fclose(f);
    return 1; //file created
}

int main(){
    int end = 0;
    while (!end)
        end = parse();
    return 0;
}