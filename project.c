//In the name of Allah
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#define MAX_N 1010

char clipboard[MAX_N * MAX_N], buff[MAX_N * MAX_N];
int fnd[MAX_N * MAX_N][4], fndPtr, buffPtr;

int toInt(char *str){
    int num = 0, len = strlen(str);
    for (int i = 0; i < len; i ++)
        num = num * 10 + (str[i] - '0');
    return num;
}

void buff_putc(char c){
    buff[buffPtr ++] = c;
}

void buff_puts(char *s){
    int len = strlen(s);
    for (int i = 0; i < len; i ++)
        buff_putc(s[i]);
}

void buff_putd(int d){
    if (d == 0)
        return buff_putc('0');
    int a[10], i = 0;
    while (d){
        a[i ++] = d % 10;
        d /= 10;
    }
    i --;
    while (i >= 0)
        buff_putc('0' + a[i --]);
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
    pth[0] = 'u'; pth[1] = 'n'; pth[2] = 'd'; pth[3] = 'o';
    for (int i = 0; i < len; i ++){
        cur[i] = pth[i];
        cur[i + 1] = '\0';
        if (cur[i] == '/')
            mkdir(cur);
    }
    f = fopen(pth, "w");
    fclose(f);
    pth[0] = 'r'; pth[1] = pth[2] = 'o'; pth[3] = 't';
    return 1; //file created
}

void handleUndo(char *pth){
    FILE *f1 = fopen(pth, "r");
    pth[0] = 'u'; pth[1] = 'n'; pth[2] = 'd'; pth[3] = 'o';
    FILE *f2 = fopen(pth, "w");
    char tmp[MAX_N];
    while (fgets(tmp, MAX_N, f1) != NULL)
        fputs(tmp, f2);
    pth[0] = 'r'; pth[1] = pth[2] = 'o'; pth[3] = 't';
    fclose(f1);
    fclose(f2);
}

int undo(char *pth){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    char **txt1 = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        txt1[i] = malloc(MAX_N * sizeof(char));
    char **txt2 = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        txt2[i] = malloc(MAX_N * sizeof(char));
    FILE *f = fopen(pth, "r");
    int cntLine1 = 0;
    for (int i = 1; fgets(txt1[i], MAX_N, f) != NULL; i ++, cntLine1 ++);
    fclose(f);
    pth[0] = 'u'; pth[1] = 'n'; pth[2] = 'd'; pth[3] = 'o';
    f = fopen(pth, "r");
    int cntLine2 = 0;
    for (int i = 1; fgets(txt2[i], MAX_N, f) != NULL; i ++, cntLine2 ++);
    fclose(f);
    f = fopen(pth, "w");
    for (int i = 1; i <= cntLine1; i ++)
        fputs(txt1[i], f);
    fclose(f);
    pth[0] = 'r'; pth[1] = pth[2] = 'o'; pth[3] = 't';
    f = fopen(pth, "w");
    for (int i = 1; i <= cntLine2; i ++)
        fputs(txt2[i], f);
    fclose(f);
    return 1; //successfull
}

int insertStr(char *pth, char *str, int line, int indx, int undoFlg){
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
    if (undoFlg)
        handleUndo(pth);
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
    while (fgets(s, MAX_N, f) != NULL){
        buff_puts(s);
        if (s[strlen(s) - 1] != '\n')
            buff_putc('\n');
    }
    fclose(f);
    return 1; //successfull
}

int removeStr(char *pth, int line, int indx, int size, int dir, int undoFlg){
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
    if (undoFlg)
        handleUndo(pth);
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
    return removeStr(pth, line, indx, size, dir, 1);
}

int paste(char *pth, int line, int indx){
    return insertStr(pth, clipboard, line, indx, 1);
}

int compare(char *pth1, char *pth2){
    if (!isPathExist(pth1) || !isPathExist(pth2))
        return -1; //wrong path
    if (!isFileExist(pth1) || !isFileExist(pth2))
        return -2; //wrong file
    FILE *f1 = fopen(pth1, "r");
    FILE *f2 = fopen(pth2, "r");
    char s1[MAX_N], s2[MAX_N];
    int flg1 = fgets(s1, MAX_N, f1) != NULL;
    int flg2 = fgets(s2, MAX_N, f2) != NULL;
    int line = 0;
    while (flg1 && flg2){
        line ++;
        if (strcmp(s1, s2)){
            buff_puts("============ #");
            buff_putd(line);
            buff_puts(" ============\n");
            buff_puts(s1);
            if (s1[strlen(s1) - 1] != '\n')
                buff_putc('\n');
            buff_puts(s2);
            if (s2[strlen(s2) - 1] != '\n')
                buff_putc('\n');
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
        buff_puts("<<<<<<<<<<<< #");
        buff_putd(line);
        buff_puts(" - #");
        buff_putd(line + ptr - 1);
        buff_puts(" <<<<<<<<<<<<\n");
        for (int i = 0; i < ptr; i ++){
            buff_puts(tmp[i]);
            if (tmp[i][strlen(tmp[i]) - 1] != '\n')
                buff_putc('\n');
        }
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
        buff_puts(">>>>>>>>>>>> #");
        buff_putd(line);
        buff_puts(" - #");
        buff_putd(line + ptr - 1);
        buff_puts(" >>>>>>>>>>>>\n");
        for (int i = 0; i < ptr; i ++){
            buff_puts(tmp[i]);
            if (tmp[i][strlen(tmp[i]) - 1] != '\n')
                buff_putc('\n');
        }
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
                buff_puts("     ");
            if (cdp)
                buff_puts("|____");
            buff_puts(dir->d_name);
            buff_putc('\n');
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
                buff_puts("     ");
            if (cdp)
                buff_puts("|____");
            buff_puts(dir->d_name);
            buff_puts(":\n");
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
                buff_puts(pth[i]);
                buff_putc('\n');
                break;
            }
            if (_c)
                res ++;
            else{
                buff_puts(pth[i]);
                buff_puts(": ");
                buff_puts(s);
                if (s[ln - 1] != '\n')
                    buff_putc('\n');
            }
        }
        fclose(f);
    }
    if (_c){
        buff_putd(res);
        buff_putc('\n');
    }
    return 1; //successfull
}

int autoIndent(char *pth){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    FILE *f = fopen(pth, "r");
    char **txt = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        txt[i] = malloc(MAX_N * sizeof(char));
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
    char **res = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        res[i] = malloc(MAX_N * sizeof(char));
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
    handleUndo(pth);
    f = fopen(pth, "w");
    for (int i = 1; i < curLine; i ++)
        fputs(res[i], f);
    fclose(f);
    free(txt);
    free(res);
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
    int cntLine = 0, wrdCnt = (_byw ? 0 : -1);
    int **res = malloc(MAX_N * sizeof(int *));
    for (int i = 0; i < MAX_N; i ++)
        res[i] = malloc(MAX_N * sizeof(int));
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, cntLine ++);
    fclose(f);
    for (int i = 1; i <= cntLine; i ++){
        int lenT = strlen(txt[i]);
        if (_byw)
            res[i][0] = (lenT == 1 && txt[i][0] == '\n') ? wrdCnt : (++ wrdCnt);
        else
            res[i][0] = ++ wrdCnt;
        for (int j = 1; j < lenT; j ++){
            if (!_byw)
                wrdCnt ++;
            else if (txt[i][j] == ' ' && txt[i][j - 1] != ' ')
                wrdCnt ++;
            res[i][j] = wrdCnt;
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
                fnd[fndPtr][3] = res[curLine][st];
                fndPtr ++;
                curIndx = en - 1;
            }
            curIndx ++;
        }
    }
    free(res);
    return 1; //successfull
}

int replace(char *pth, char *str1, char *str2, int _at, int _all){
    if (!isPathExist(pth))
        return -1; //wrong path
    if (!isFileExist(pth))
        return -2; //wrong file
    if (_at && _all)
        return -3; //wrong options
    find(pth, str1, 0, 0, 0, 0);
    if (fndPtr == 0 || fndPtr < _at)
        return -4; //not found
    handleUndo(pth);
    if (_at){
        _at --;
        removeStr(pth, fnd[_at][0], fnd[_at][1], fnd[_at][2] - fnd[_at][1], 1, 0);
        insertStr(pth, str2, fnd[_at][0], fnd[_at][1], 0);
    }
    else{
        for (int i = fndPtr - 1; i >= 0; i --){
            removeStr(pth, fnd[i][0], fnd[i][1], fnd[i][2] - fnd[i][1], 1, 0);
            insertStr(pth, str2, fnd[i][0], fnd[i][1], 0);
        }
    }
    return 1; //successfull
}

int parse(){
    buffPtr = 0;
    char c;
    char **inp = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        inp[i] = malloc(MAX_N * sizeof(char));
    int cur = 0, ptr = 0, inQ = 0;
    while ((c = getchar()) != '\n'){
        if (c == '"'){
            if (ptr == 0)
                inQ = 1;
            else if (inQ && inp[cur][ptr - 1] == '\\')
                inp[cur][ptr - 1] = '"';
            else if (!inQ)
                inp[cur][ptr ++] = '"';
            else
                inQ = 0;
        }
        else if (c == ' '){
            if (inQ)
                inp[cur][ptr ++] = ' ';
            else if (ptr){
                inp[cur ++][ptr] = '\0';
                ptr = inQ = 0;
            }
        }
        else if (c == 'n'){
            if (ptr >= 2 && inp[cur][ptr - 1] == '\\' && inp[cur][ptr - 2] == '\\')
                inp[cur][ptr - 1] = 'n';
            else if (ptr >= 1 && inp[cur][ptr - 1] == '\\')
                inp[cur][ptr - 1] = '\n';
            else
                inp[cur][ptr ++] = 'n';
        }
        else
            inp[cur][ptr ++] = c;
    }
    if (ptr){
        inp[cur ++][ptr] = '\0';
        ptr = inQ = 0;
    }
    int tot = cur;
    for (int i = 0; i < tot; i ++)
        if (!strcmp(inp[i], "=D"))
            cur = i;
    if (!strcmp(inp[0], "createfile")){
        //createfile --file /root/...
        int stat = createFile(inp[2] + 1);
        if (stat == -1)
            printf("file exists\n");
    }
    else if (!strcmp(inp[0], "insertstr")){
        //insertstr --file /root/... --str str -pos line:indx
        int line = 0, indx = 0, i = 0;
        for (; inp[6][i] != ':'; i ++)
            line = line * 10 + (inp[6][i] - '0');
        for (i ++; i < strlen(inp[6]); i ++)
            indx = indx * 10 + (inp[6][i] - '0');
        int stat = insertStr(inp[2] + 1, inp[4], line, indx, 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    else if (!strcmp(inp[0], "cat")){
        //cat --file /root/...
        int stat = cat(inp[2] + 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == 1 && cur == tot){
            buff[buffPtr] = '\0';
            printf("%s", buff);
        }
    }
    else if (!strcmp(inp[0], "removestr")){
        //removestr --file /root/... -pos line:indx -size size -b | -f
        int line = 0, indx = 0, i = 0;
        for (; inp[4][i] != ':'; i ++)
            line = line * 10 + (inp[4][i] - '0');
        for (i ++; i < strlen(inp[4]); i ++)
            indx = indx * 10 + (inp[4][i] - '0');
        int size = toInt(inp[6]);
        int dir = (inp[7][1] == 'f' ? 1 : -1);
        int stat = removeStr(inp[2] + 1, line, indx, size, dir, 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    else if (!strcmp(inp[0], "copystr")){
        //copystr --file /root/... -pos line:indx -size size -f | -b
        int line = 0, indx = 0, i = 0;
        for (; inp[4][i] != ':'; i ++)
            line = line * 10 + (inp[4][i] - '0');
        for (i ++; i < strlen(inp[4]); i ++)
            indx = indx * 10 + (inp[4][i] - '0');
        int size = toInt(inp[6]);
        int dir = (inp[7][1] == 'f' ? 1 : -1);
        int stat = copy(inp[2] + 1, line, indx, size, dir);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    else if (!strcmp(inp[0], "cutstr")){
        //cutstr --file /root/... -pos line:indx -size size -f | -b
        int line = 0, indx = 0, i = 0;
        for (; inp[4][i] != ':'; i ++)
            line = line * 10 + (inp[4][i] - '0');
        for (i ++; i < strlen(inp[4]); i ++)
            indx = indx * 10 + (inp[4][i] - '0');
        int size = toInt(inp[6]);
        int dir = (inp[7][1] == 'f' ? 1 : -1);
        int stat = cut(inp[2] + 1, line, indx, size, dir);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    else if (!strcmp(inp[0], "pastestr")){
        //pastestr --file /root/... -pos line:indx -size size
        int line = 0, indx = 0, i = 0;
        for (; inp[4][i] != ':'; i ++)
            line = line * 10 + (inp[4][i] - '0');
        for (i ++; i < strlen(inp[4]); i ++)
            indx = indx * 10 + (inp[4][i] - '0');
        int size = toInt(inp[6]);
        int stat = paste(inp[2] + 1, line, indx);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    else if (!strcmp(inp[0], "find")){
        //find --str str --file /root/... -count | -at | -byword | -all
        int _count = 0, _at = 0, _byword = 0, _all = 0;
        for (int i = 5; i < cur; i ++){
            if (!strcmp(inp[i], "-count"))
                _count = 1;
            if (!strcmp(inp[i], "-at"))
                _at = toInt(inp[i + 1]);
            if (!strcmp(inp[i], "-byword"))
                _byword = 1;
            if (!strcmp(inp[i], "-all"))
                _all = 1;
        }
        if (!_count && !_at && !_all)
            _at = 1;
        int stat = find(inp[4] + 1, inp[2], _count, _at, _byword, _all);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong options\n");
        if (stat == 1){
            if (_count)
                buff_putd(fndPtr);
            if (_at){
                if (_at > fndPtr)
                    buff_puts("-1\n");
                else{
                    buff_putd(fnd[_at - 1][3]);
                    buff_putc('\n');
                }
            }
            if (_all){
                if (fndPtr == 0)
                    buff_puts("nothing\n");
                else{
                    for (int i = 0; i < fndPtr - 1; i ++){
                        buff_putd(fnd[i][3]);
                        buff_puts(", ");
                    }
                    //printf("%d\n", fnd[fndPtr - 1][3]);
                    buff_putd(fnd[fndPtr - 1][3]);
                    buff_putc('\n');
                }
            }
            if (cur == tot)
                printf("%s", buff);
        }
    }
    else if (!strcmp(inp[0], "replace")){
        //replace --str1 str1 --str2 str2 --file /root/... -at | -all
        int _at = 0, _all = 0;
        for (int i = 7; i < cur; i ++){
            if (!strcmp(inp[i], "-at"))
                _at = toInt(inp[i + 1]);
            if (!strcmp(inp[i], "-all"))
                _all = 1;
        }
        if (!_at && !_all)
            _at = 1;
        int stat = replace(inp[6] + 1, inp[2], inp[4], _at, _all);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong options\n");
        if (stat == -4)
            printf("nothing found\n");
        if (stat == 1)
            printf("replaced successfully\n");
    }
    else if (!strcmp(inp[0], "grep")){
        //grep --str str --files /root/... /root/... ... -c | -l
        int n = 0, _c = 0, _l = 0;
        char pth[MAX_N][MAX_N];
        for (int i = 4; i < cur; i ++){
            int len = strlen(inp[i]);
            if (!strcmp(inp[i], "-c"))
                _c = 1;
            else if (!strcmp(inp[i], "-l"))
                _l = 1;
            else{
                for (int j = 1; j < len; j ++)
                    pth[n][j - 1] = inp[i][j];
                pth[n ++][len - 1] = '\0';
            }
        }
        int stat = grep(inp[2], n, pth, _c, _l);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong options\n");
        if (stat == 1 && cur == tot){
            buff[buffPtr] = '\0';
            printf("%s", buff);
        }
    }
    else if (!strcmp(inp[0], "undo")){
        //undo --file /root/...
        int stat = undo(inp[2] + 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
    }
    else if (!strcmp(inp[0], "auto-indent")){
        //auto-indent /root/...
        int stat = autoIndent(inp[1] + 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong braces\n");
    }
    else if (!strcmp(inp[0], "compare")){
        //compare /root/... /root/...
        int stat = compare(inp[1] + 1, inp[2] + 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == 1 && cur == tot){
            buff[buffPtr] = '\0';
            printf("%s", buff);
        }
    }
    else if (!strcmp(inp[0], "tree")){
        //tree depth
        int dep = (inp[1][0] == '-' ? -toInt(inp[1] + 1) : toInt(inp[1]));
        if (dep < -1)
            printf("invalid depth\n");
        else{
            char *pth = malloc(MAX_N * sizeof(char));
            pth[0] = 'r'; pth[1] = pth[2] = 'o'; pth[3] = 't'; pth[4] = '/'; pth[5] = '\0';
            tree(pth, dep, 0);
            if (cur == tot){
                buff[buffPtr] = '\0';
                printf("%s", buff);
            }
            free(pth);
        } 
    }
    else if (!strcmp(inp[0], "exit")){
        free(inp);
        return 1;
    }
    else{
        printf("invalid command\n");
        free(inp);
        return 0;
    }
    if (cur == tot){
        free(inp);
        return 0;
    }
    if (!strcmp(inp[cur + 1], "insertstr")){
        //insertstr --file /root/... -pos line:indx
        int line = 0, indx = 0, i = 0;
        for (; inp[cur + 5][i] != ':'; i ++)
            line = line * 10 + (inp[cur + 5][i] - '0');
        for (i ++; i < strlen(inp[cur + 5]); i ++)
            indx = indx * 10 + (inp[cur + 5][i] - '0');
        buff[buffPtr] = '\0';
        int stat = insertStr(inp[cur + 3] + 1, buff, line, indx, 1);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -10)
            printf("wrong line number or start position\n");
    }
    if (!strcmp(inp[cur + 1], "find")){
        //find --file /root/... -count | -at | -byword | -all
        int _count = 0, _at = 0, _byword = 0, _all = 0;
        for (int i = cur + 4; i < tot; i ++){
            if (!strcmp(inp[i], "-count"))
                _count = 1;
            if (!strcmp(inp[i], "-at"))
                _at = toInt(inp[i + 1]);
            if (!strcmp(inp[i], "-byword"))
                _byword = 1;
            if (!strcmp(inp[i], "-all"))
                _all = 1;
        }
        if (!_count && !_at && !_all)
            _at = 1;
        buff[buffPtr] = '\0';
        int stat = find(inp[cur + 3] + 1, buff, _count, _at, _byword, _all);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong options\n");
        if (stat == 1){
            if (_count)
                printf("%d\n", fndPtr);
            if (_at){
                if (_at > fndPtr)
                    printf("-1\n");
                else
                    printf("%d\n", fnd[_at - 1][3]);
            }
            if (_all){
                if (fndPtr == 0)
                    printf("nothing\n");
                else{
                    for (int i = 0; i < fndPtr - 1; i ++)
                        printf("%d, ", fnd[i][3]);
                    printf("%d\n", fnd[fndPtr - 1][3]);
                }
            }
        }
    }
    if (!strcmp(inp[cur + 1], "replace")){
        //replace --str2 str2 --file /root/... -at | -all
        int _at = 0, _all = 0;
        for (int i = cur + 6; i < tot; i ++){
            if (!strcmp(inp[i], "-at"))
                _at = toInt(inp[i + 1]);
            if (!strcmp(inp[i], "-all"))
                _all = 1;
        }
        if (!_at && !_all)
            _at = 1;
        buff[buffPtr] = '\0';
        int stat = replace(inp[cur + 5] + 1, buff, inp[cur + 3], _at, _all);
        if (stat == -1)
            printf("no such directories\n");
        if (stat == -2)
            printf("no such file\n");
        if (stat == -3)
            printf("wrong options\n");
        if (stat == -4)
            printf("nothing found\n");
        if (stat == 1)
            printf("replaced successfully\n");
    }
    free(inp);
    //Handle =D command
    return 0;
}

int main(){
    int end = 0;
    while (!end)
        end = parse();
    return 0;
}