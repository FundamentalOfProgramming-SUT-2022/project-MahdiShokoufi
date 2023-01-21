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
    printf("\n");
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

int paste(char *pth, int line, int indx){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    insertStr(pth, clipboard, line, indx);
    return 1; //successfull
}

int compare(char *pth1, char *pth2){
    if (!isPathExist(pth1))
        return -1; //wrong path1
    if (!isPathExist(pth2))
        return -2; //wrong path2
    if (!isFileExist(pth1))
        return -3; //wrong file1
    if (!isFileExist(pth2))
        return -4; //wrong file2
    FILE *f1 = fopen(pth1, "r");
    FILE *f2 = fopen(pth2, "r");
    char s1[1000], s2[1000];
    int flg1 = fgets(s1, 1000, f1) != NULL;
    int flg2 = fgets(s2, 1000, f2) != NULL;
    int line = 0;
    while (flg1 && flg2){
        line ++;
        if (strcmp(s1, s2)){
            printf("============ #%d ============\n", line);
            printf("%s", s1);
            if (s1[strlen(s1) - 1] != '\n')
                printf("\n");
            printf("%s", s2);
            if (s2[strlen(s2) - 1] != '\n')
                printf("\n");
        }
        flg1 = fgets(s1, 1000, f1) != NULL;
        flg2 = fgets(s2, 1000, f2) != NULL;
    }
    line ++;
    if (flg1){
        char tmp[1000][1000];
        int len = strlen(s1);
        for (int i = 0; i < len; i ++)
            tmp[0][i] = s1[i];
        tmp[0][len] = '\0';
        int ptr = 0;
        while (flg1)
            flg1 = fgets(tmp[++ ptr], 1000, f1) != NULL;
        printf("<<<<<<<<<<<< #%d - #%d <<<<<<<<<<<<\n", line, line + ptr - 1);
        for (int i = 0; i < ptr; i ++){
            printf("%s", tmp[i]);
            if (tmp[i][strlen(tmp[i]) - 1] != '\n')
                printf("\n");
        }
    }
    if (flg2){
        char tmp[1000][1000];
        int len = strlen(s2);
        for (int i = 0; i < len; i ++)
            tmp[0][i] = s2[i];
        tmp[0][len] = '\0';
        int ptr = 0;
        while (flg2)
            flg2 = fgets(tmp[++ ptr], 1000, f2) != NULL;
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", line, line + ptr - 1);
        for (int i = 0; i < ptr; i ++){
            printf("%s", tmp[i]);
            if (tmp[i][strlen(tmp[i]) - 1] != '\n')
                printf("\n");
        }
    }
    fclose(f1);
    fclose(f2);
    return 1; //successfull
}

int main(){
    int end = 0;
    while (!end)
        end = parse();
    return 0;
}