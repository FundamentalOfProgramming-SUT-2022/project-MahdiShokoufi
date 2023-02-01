//In the name of Allah
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#define MAX_N 1010

char clipboard[MAX_N * MAX_N];
int fnd[MAX_N * MAX_N][4], fndPtr;

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
    char cur[MAX_N];
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
    char cur[MAX_N];
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
    char txt[MAX_N][MAX_N], tmp[MAX_N];
    int cntLine = 0;
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, cntLine ++);
    fclose(f);
    if (cntLine == 0){ //handle empty files
        if (line != 1 || indx != 0)
            return -10; //wrong pos
        f = fopen(pth, "w");
        fputs(str, f);
        fclose(f);
        return 1; //successfull
    }
    if (cntLine < line || strlen(txt[line]) < indx)
        return -10; //wrong pos
    for (int i = 0; i < indx; i ++)
        tmp[i] = txt[line][i];
    int strLen = strlen(str), lineLen = strlen(txt[line]);
    for (int i = 0; i < strLen; i ++)
        tmp[i + indx] = str[i];
    for (int i = indx; i < lineLen; i ++)
        tmp[i + strLen] = txt[line][i];
    tmp[lineLen + strLen] = '\0';
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
    char s[MAX_N];
    while (fgets(s, MAX_N, f) != NULL)
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
    char txt[MAX_N * MAX_N], tmp[MAX_N];
    int ptr = 0, curLine = 0, pos = -1;
    while (fgets(tmp, MAX_N, f) != NULL){
        int len = strlen(tmp);
        curLine ++;
        for (int i = 0; i < len; i ++){
            txt[ptr ++] = tmp[i];
            if (curLine == line && i == indx)
                pos = ptr - 1;
        }
    }
    fclose(f);
    if (pos == -1)
        return -10; //wrong pos
    int st, en;
    if (dir == -1)
        st = pos - size, en = pos; //backward
    else
        st = pos, en = pos + size; //forward
    st = st < 0 ? 0 : st;
    en = en > ptr ? ptr : en;
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
    char txt[MAX_N * MAX_N], tmp[MAX_N];
    int ptr = 0, curLine = 0, pos = -1;
    while (fgets(tmp, MAX_N, f) != NULL){
        int len = strlen(tmp);
        curLine ++;
        for (int i = 0; i < len; i ++){
            txt[ptr ++] = tmp[i];
            if (curLine == line && i == indx)
                pos = ptr - 1;
        }
    }
    fclose(f);
    if (pos == -1)
        return -10; //wrong pos
    int st, en;
    if (dir == -1)
        st = pos - size, en = pos; //backward
    else
        st = pos, en = pos + size; //forward
    st = st < 0 ? 0 : st;
    en = en > ptr ? ptr : en;
    for (int i = st; i < en; i ++)
        clipboard[i - st] = txt[i];
    clipboard[en - st] = '\0';
    return 1; //successfull
}

int cut(char *pth, int line, int indx, int size, int dir){
    int stat = copy(pth, line, indx, size, dir);
    if (stat != 1) //error
        return stat;
    return removeStr(pth, line, indx, size, dir);
}

int paste(char *pth, int line, int indx){
    return insertStr(pth, clipboard, line, indx);
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
    char s1[MAX_N], s2[MAX_N];
    int flg1 = fgets(s1, MAX_N, f1) != NULL;
    int flg2 = fgets(s2, MAX_N, f2) != NULL;
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
        flg1 = fgets(s1, MAX_N, f1) != NULL;
        flg2 = fgets(s2, MAX_N, f2) != NULL;
    }
    line ++;
    if (flg1){
        char tmp[MAX_N][MAX_N];
        int len = strlen(s1);
        for (int i = 0; i < len; i ++)
            tmp[0][i] = s1[i];
        tmp[0][len] = '\0';
        int ptr = 0;
        while (flg1)
            flg1 = fgets(tmp[++ ptr], MAX_N, f1) != NULL;
        printf("<<<<<<<<<<<< #%d - #%d <<<<<<<<<<<<\n", line, line + ptr - 1);
        for (int i = 0; i < ptr; i ++)
            printf("%s", tmp[i]);
    }
    if (flg2){
        char tmp[MAX_N][MAX_N];
        int len = strlen(s2);
        for (int i = 0; i < len; i ++)
            tmp[0][i] = s2[i];
        tmp[0][len] = '\0';
        int ptr = 0;
        while (flg2)
            flg2 = fgets(tmp[++ ptr], MAX_N, f2) != NULL;
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", line, line + ptr - 1);
        for (int i = 0; i < ptr; i ++)
            printf("%s", tmp[i]);
    }
    fclose(f1);
    fclose(f2);
    return 1; //successfull
}

void tree(char *pth, int dep, int cdp){
    //should call with ("root/", dep, 0)
    if (dep == 0)
        return;
    DIR *d = opendir(pth);
    struct dirent *dir;
    int len = strlen(pth);
    while ((dir = readdir(d)) != NULL){
        if (dir->d_name[0] == '.')
            continue;
        int dlen = strlen(dir->d_name);
        for (int i = 0; i < dlen; i ++)
            pth[len + i] = dir->d_name[i];
        pth[len + dlen] = '/';
        pth[len + dlen + 1] = '\0';
        DIR *tmp = opendir(pth);
        if (!tmp){
            for (int i = 0; i < cdp - 1; i ++)
                printf("     ");
            if (cdp)
                printf("|____");
            printf("%s\n", dir->d_name);
        }
    }
    pth[len] = '\0';
    d = opendir(pth);
    while ((dir = readdir(d)) != NULL){
        if (dir->d_name[0] == '.')
            continue;
        int dlen = strlen(dir->d_name);
        for (int i = 0; i < dlen; i ++)
            pth[len + i] = dir->d_name[i];
        pth[len + dlen] = '/';
        pth[len + dlen + 1] = '\0';
        DIR *tmp = opendir(pth);
        if (tmp){
            for (int i = 0; i < cdp - 1; i ++)
                printf("     ");
            if (cdp)
                printf("|____");
            printf("%s:\n", dir->d_name);
            tree(pth, dep - 1, cdp + 1);
        }
    }
}

int grep(char *str, int n, char pth[MAX_N][MAX_N], int _c, int _l){
    for (int i = 0; i < n; i ++)
        if (!isPathExist(pth[i]))
            return -1; //wrong path
    for (int i = 0; i < n; i ++)
        if (!isFileExist(pth[i]))
            return -2; //wrong file
    if (_c && _l)
        return -3; //wrong options
    int res = 0, len = strlen(str);
    char s[MAX_N];
    for (int i = 0; i < n; i ++){
        FILE *f = fopen(pth[i], "r");
        int flg_l = 0;
        while (fgets(s, MAX_N, f) != NULL){
            int ln = strlen(s), find = 0;
            for (int j = 0; j + len - 1 < ln; j ++){
                int flg = 1;
                for (int k = 0; k < len; k ++){
                    if (str[k] != s[j + k]){
                        flg = 0;
                        break;
                    }
                }
                if (flg){
                    find = 1;
                    break;
                }
            }
            if (!find)
                continue;
            if (_l){
                printf("%s\n", pth[i]);
                break;
            }
            if (_c)
                res ++;
            else{
                printf("%s: %s", pth[i], s);
                if (s[ln - 1] != '\n')
                    printf("\n");
            }
        }
        fclose(f);
    }
    if (_c)
        printf("%d\n", res);
    return 1; //successfull
}

int autoIndent(char *pth){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char txt[MAX_N][MAX_N];
    int cntLine = 0;
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, cntLine ++);
    fclose(f);
    int cntOpn = 0;
    for (int i = 1; i <= cntLine; i ++){
        int len = strlen(txt[i]);
        for (int j = 0; j < len; j ++){
            if (txt[i][j] == '{')
                cntOpn ++;
            if (txt[i][j] == '}')
                cntOpn --;
            if (cntOpn < 0)
                return -3; //wrong braces
        }
    }
    if (cntOpn)
        return -3; //wrong braces
    char res[MAX_N][MAX_N];
    int curLine = 1, ptr = 0;
    for (int i = 1; i <= cntLine; i ++){
        int len = strlen(txt[i]), j = 0;
        while (j < len){
            if (ptr == 0 && (txt[i][j] == ' ' || txt[i][j] == '\t' || txt[i][j] == '\n')){
                while (j < len && (txt[i][j] == ' ' || txt[i][j] == '\t' || txt[i][j] == '\n'))
                    j ++;
                continue;
            }
            if (txt[i][j] == '{'){
                while (ptr >= 1 && (res[curLine][ptr - 1] == ' ' || res[curLine][ptr - 1] == '\t' || res[curLine][ptr - 1] == '\n'))
                    ptr --;
                if (ptr > 0)
                    res[curLine][ptr ++] = ' ';
                if (ptr == 0)
                    for (int k = 0; k < cntOpn; k ++)
                        res[curLine][ptr ++] = '\t';
                res[curLine][ptr ++] = '{';
                res[curLine][ptr ++] = '\n';
                res[curLine][ptr ++] = '\0';
                curLine ++; ptr = 0;
                j ++;
                cntOpn ++;
            }
            else if (txt[i][j] == '}'){
                while (ptr >= 1 && (res[curLine][ptr - 1] == ' ' || res[curLine][ptr - 1] == '\t' || res[curLine][ptr - 1] == '\n'))
                    ptr --;
                if (ptr > 0){
                    res[curLine][ptr ++] = '\n';
                    res[curLine][ptr ++] = '\0';
                    curLine ++; ptr = 0;
                }
                cntOpn --;
                for (int k = 0; k < cntOpn; k ++)
                    res[curLine][ptr ++] = '\t';
                res[curLine][ptr ++] = '}';
                res[curLine][ptr ++] = '\n';
                res[curLine][ptr ++] = '\0';
                curLine ++; ptr = 0;
                j ++;
            }
            else{
                if (ptr == 0)
                    for (int k = 0; k < cntOpn; k ++)
                        res[curLine][ptr ++] = '\t';
                res[curLine][ptr ++] = txt[i][j];
                if (txt[i][j] == '\n'){
                    res[curLine][ptr ++] = '\0';
                    curLine ++; ptr = 0;
                }
                j ++;
            }
        }
    }
    f = fopen(pth, "w");
    for (int i = 1; i < curLine; i ++)
        fputs(res[i], f);
    fclose(f);
    return 1; //successfull
}

int find(char *pth, char *str, int _cnt, int _at, int _byw, int _all){
    fndPtr = 0;
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    if ((_cnt && (_at || _byw || _all)) || (_at && _all))
        return -3; //wrong options
    int len = strlen(str);
    int pos = -1;
    for (int i = 0; i < len ; i++)
        if (str[i] == '*' && !(i > 0 && str[i - 1] == '\\'))
            pos = i;
    int spcFlg = 0;
    for (int i = pos + 1; i < len; i ++)
        if (str[i] == ' ')
            spcFlg = 1;
    int begFlg = (pos > 0 && str[pos - 1] == ' ') || (pos == 0);
    int endFlg = (pos != -1 && pos + 1 < len && str[pos + 1] == ' ') || (pos == len - 1);
    int sPtr = 0;
    for (int i = 0; i < len; i ++)
        if (!(str[i] == '\\' && i + 1 < len && str[i + 1] == '*'))
            str[sPtr ++] = str[i];
    str[sPtr] = '\0';
    len = sPtr;
    FILE *f = fopen(pth, "r");
    char txt[MAX_N][MAX_N];
    int cntLine = 0, wrdCnt = 0;
    int **byWrd = malloc(MAX_N * sizeof(int *));
    for (int i = 0; i < MAX_N; i ++)
        byWrd[i] = malloc(MAX_N * sizeof(int));
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, cntLine ++);
    fclose(f);
    for (int i = 1; i <= cntLine; i ++){
        int lenT = strlen(txt[i]);
        byWrd[i][0] = (lenT == 1 && txt[i][0] == '\n') ? wrdCnt : (++ wrdCnt);
        for (int j = 1; j < lenT; j ++){
            if (txt[i][j] == ' ' && txt[i][j - 1] != ' ')
                wrdCnt ++;
            byWrd[i][j] = wrdCnt;
        }
    }
    for (int curLine = 1; curLine <= cntLine; curLine ++){
        int curLen = strlen(txt[curLine]), curIndx = 0;
        while (curIndx < curLen){
            int fndFlg = 1, st = -1, en = -1;
            if (begFlg && !spcFlg){ //A B *a
                while (curIndx < curLen && txt[curLine][curIndx] == ' ')
                    curIndx ++;
                int endWrd = curIndx;
                while (endWrd < curLen && txt[curLine][endWrd] != ' ')
                    endWrd ++;
                if (curIndx - pos < 0)
                    fndFlg = 0;
                for (int i = 0; i < pos && fndFlg; i ++)
                    if (txt[curLine][curIndx - 1 - i] != str[pos - 1 - i])
                        fndFlg = 0;
                if (fndFlg){
                    fndFlg = 0;
                    for (int i = endWrd; i - len + pos + 1 >= curIndx && !fndFlg; i --){
                        fndFlg = 1;
                        for (int j = 0; j < len - (pos + 1); j ++)
                            if (str[len - j - 1] != txt[curLine][i - j - 1])
                                fndFlg = 0;
                        if (fndFlg)
                            en = i;
                    }
                    if (fndFlg)
                        st = curIndx - pos;
                }
            }
            else{
                int L = pos + 1, R = len; //A or A *a B
                if (endFlg) //A a* B
                    L = 0, R = pos;
                if (curIndx + (R - L) - 1 >= curLen)
                    fndFlg = 0;
                for (int i = 0; i < R - L && fndFlg; i ++)
                    if (txt[curLine][curIndx + i] != str[L + i])
                        fndFlg = 0;
                if (pos == -1 && fndFlg) //A
                    st = curIndx, en = curIndx + len;
                if (begFlg && fndFlg){ //A *a B
                    int i = curIndx - 1;
                    while (0 <= i && txt[curLine][i] != ' ')
                        i --;
                    L = 0; R = pos;
                    if (i - (R - L - 1) < 0)
                        fndFlg = 0;
                    for (int j = 0; j < R - L && fndFlg; j ++)
                        if (txt[curLine][i - j] != str[R - 1 - j])
                            fndFlg = 0;
                    if (fndFlg)
                        st = i - (R - L - 1), en = curIndx + len - pos - 1;
                }
                if (endFlg && fndFlg){ //A a* B
                    int i = curIndx + R - L;
                    while (i < curLen && (txt[curLine][i] != ' ' && txt[curLine][i] != '\n'))
                        i ++;
                    L = pos + 1; R = len;
                    if (i + (R - L) - 1 >= curLen)
                        fndFlg = 0;
                    for (int j = 0; j < R - L && fndFlg; j ++)
                        if (txt[curLine][i + j] != str[L + j])
                            fndFlg = 0;
                    if (fndFlg)
                        st = curIndx, en = i + (R - L);
                }
            }
            if (fndFlg && (fndPtr <= 0 || !(fnd[fndPtr - 1][0] == curLine && fnd[fndPtr - 1][1] < en && st < fnd[fndPtr - 1][2]))){
                fnd[fndPtr][0] = curLine;
                fnd[fndPtr][1] = st;
                fnd[fndPtr][2] = en;
                fnd[fndPtr][3] = byWrd[curLine][st];
                fndPtr ++;
                curIndx = en - 1;
            }
            curIndx ++;
        }
    }
    free(byWrd);
    return 1; //successfull
}

int main(){
    int end = 0;
    while (!end)
        end = parse();
    return 0;
}