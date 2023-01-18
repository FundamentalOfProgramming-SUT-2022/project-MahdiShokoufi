//In the name of Allah
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <sys/stat.h>

char clipboard[1000 * 1000];

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

int isPathExist(char *pth){
    //pth: root/sth/A.txt
    struct stat st;
    int len = strlen(pth);
    char cur[1000];
    for (int i = 0; i < len; i ++){
        cur[i] = pth[i];
        cur[i + 1] = '\0';
        if (cur[i] == '/'){
            cur[i] = '\0';
            if (stat(cur, &st) == -1)
                return 0;
            cur[i] = '/';
        }
    }
    return 1; //path exists
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
        if (cur[i] == '/')
            mkdir(cur);
    }
    FILE *f = fopen(pth, "w");
    fclose(f);
    return 1; //file created
}

int insertStr(char *pth, char *str, int line, int indx){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char txt[1000][1000], tmp[1000];
    int cntLine = 0;
    for (int i = 1; fgets(txt[i], 1000, f) != NULL; i ++, cntLine ++);
    for (int i = 0; i < indx; i ++)
        tmp[i] = txt[line][i];
    int strLen = strlen(str), lineLen = strlen(txt[line]);
    for (int i = 0; i < strLen; i ++)
        tmp[i + indx] = str[i];
    for (int i = indx; i < lineLen; i ++)
        tmp[i + strLen] = txt[line][i];
    tmp[lineLen + strLen] = '\0';
    fclose(f);
    f = fopen(pth, "w");
    for (int i = 1; i <= cntLine; i ++){
        if (i == line)
            fputs(tmp, f);
        else
            fputs(txt[i], f);
    }
    fclose(f);
    return 1; //successfull
}

int cat(char *pth){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char s[1000];
    while (fgets(s, 1000, f) != NULL)
        printf("%s", s);
    fclose(f);
    return 1; //successfull
}

int removeStr(char *pth, int line, int indx, int size, int dir){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char txt[1000 * 1000], tmp[1000];
    int ptr = 0, curLine = 0, pos = -1;
    while (fgets(tmp, 1000, f) != NULL){
        int len = strlen(tmp);
        curLine ++;
        for (int i = 0; i < len; i ++){
            txt[ptr ++] = tmp[i];
            if (curLine == line && i == indx)
                pos = ptr - 1;
        }
    }
    fclose(f);
    int st, en;
    if (dir == -1)
        st = pos - size, en = pos; //backward
    else
        st = pos, en = pos + size; //forward
    f = fopen(pth, "w");
    for (int i = 0; i < st; i ++)
        fputc(txt[i], f);
    for (int i = en; i < ptr; i ++)
        fputc(txt[i], f);
    fclose(f);
    return 1; //successfull
}

int copy(char *pth, int line, int indx, int size, int dir){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char txt[1000 * 1000], tmp[1000];
    int ptr = 0, curLine = 0, pos = -1;
    while (fgets(tmp, 1000, f) != NULL){
        int len = strlen(tmp);
        curLine ++;
        for (int i = 0; i < len; i ++){
            txt[ptr ++] = tmp[i];
            if (curLine == line && i == indx)
                pos = ptr - 1;
        }
    }
    fclose(f);
    int st, en;
    if (dir == -1)
        st = pos - size, en = pos; //backward
    else
        st = pos, en = pos + size; //forward
    for (int i = st; i < en; i ++)
        clipboard[i - st] = txt[i];
    clipboard[size] = '\0';
    return 1; //successfull
}

int cut(char *pth, int line, int indx, int size, int dir){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    copy(pth, line, indx, size, dir);
    removeStr(pth, line, indx, size, dir);
    return 1; //successfull 
}

int main(){
    int end = 0;
    while (!end)
        end = parse();
    return 0;
}