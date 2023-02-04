//In the name of Allah
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAX_N 1010
#define BACKSPACE 127
#define ESCAPE 27
#define MAX_LTD 20

enum Mode{
    NORMAL,
    INSERT,
    VISUAL
} mode;
int curLine, curIndx; //cursor
int lineCnt; //total lines
int saved;
char txt[MAX_N][MAX_N]; //line, indx
short color[MAX_N][MAX_N]; //pair number
int stLine, enLine;
char fileName[MAX_N];
char cmdBar[MAX_N]; int cmdPtr; //command bar
int stX, stY; //starting cordinate of visual mode
char clipboard[MAX_N * MAX_N];
char err[MAX_N];
int fnd[MAX_N * MAX_N][3], fndPtr;

int toInt(char *str){
    int num = 0, len = strlen(str);
    for (int i = 0; i < len; i ++)
        num = num * 10 + (str[i] - '0');
    return num;
}

int min(int a, int b){
    return a < b ? a : b;
}

int max(int a, int b){
    return a < b ? b : a;
}

int isFileExist(char *pth){
    FILE *f;
    if (f = fopen(pth, "r")){
        fclose(f);
        return 1;
    }
    return 0;
}

void init(){
    //vars
    mode = NORMAL;
    curLine = 1; curIndx = 0;
    lineCnt = 1;
    saved = 0;
    txt[0][0] = '\n'; txt[0][1] = '\0';
    for (int i = 1; i < MAX_N; i ++)
        txt[i][0] = '\0';
    for (int i = 0; i < MAX_N; i ++)
        for (int j = 0; j < MAX_N; j ++)
            color[i][j] = 1;
    stLine = enLine = 1;
    fileName[0] = '\0';
    cmdBar[0] = '\0';
    stX = stY = -1;
    //ncurses
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK); //normal text
    init_pair(2, COLOR_WHITE, COLOR_YELLOW); //highlighted text
    init_pair(3, COLOR_RED, COLOR_BLACK); //normal ()
    init_pair(4, COLOR_RED, COLOR_YELLOW); //highlighted ()
    init_pair(5, COLOR_BLUE, COLOR_BLACK); //normal {}
    init_pair(6, COLOR_BLUE, COLOR_YELLOW); //highlighted {}
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK); //normal []
    init_pair(8, COLOR_MAGENTA, COLOR_YELLOW); //highlighted []
}

void navigate(char c){
    if (c == 'h'){ //left
        curIndx = max(curIndx - 1, 0);
    }
    if (c == 'l'){ //right
        int len = strlen(txt[curLine]);
        if (len > 0 && txt[curLine][len - 1] == '\n')
            len --;
        curIndx = min(curIndx + 1, len);
    }
    if (c == 'j'){ //down
        if (curLine == lineCnt)
            return;
        curLine ++;
        int len = strlen(txt[curLine]);
        if (len > 0 && txt[curLine][len - 1] == '\n')
            len --;
        curIndx = min(curIndx, len);
        if (enLine - curLine < 3 && enLine < lineCnt)
            stLine ++, enLine ++;
    }
    if (c == 'k'){ //up
        if (curLine == 1)
            return;
        curLine --;
        int len = strlen(txt[curLine]);
        if (len > 0 && txt[curLine][len - 1] == '\n')
            len --;
        curIndx = min(curIndx, len);
        if (curLine - stLine < 3 && stLine > 1)
            stLine --, enLine --;
    }
}

void traverse(int line, int indx){
    line = min(max(1, line), lineCnt);
    if (curLine < line){
        int t = line - curLine;
        for (int i = 0; i < t; i ++)
            navigate('j');
    }
    else{
        int t = curLine - line;
        for (int i = 0; i < t; i ++)
            navigate('k');
    }
    if (curIndx < indx){
        int t = indx - curIndx;
        for (int i = 0; i < t; i ++)
            navigate('l');
    }
    else{
        int t = curIndx - indx;
        for (int i = 0; i < t; i ++)
            navigate('h');
    }
}

void display(){
    clear();
    for (int line = stLine; line <= enLine; line ++){
        printw("%3d ", line);
        int len = strlen(txt[line]);
        for (int indx = 0; indx < len; indx ++){
            int col = color[line][indx];
            if (stY != -1 && line == curLine && min(stY, curIndx) <= indx && indx < max(stY, curIndx))
                col = 2;
            char c = txt[line][indx];
            if (c == '(' || c == ')')
                col = col == 1 ? 3 : 4;
            if (c == '{' || c == '}')
                col = col == 1 ? 5 : 6;
            if (c == '[' || c == ']')
                col = col == 1 ? 7 : 8;
            attron(COLOR_PAIR(col));
            printw("%c", c);
            attroff(COLOR_PAIR(col));
        }
    }
    for (int i = 0; i < MAX_LTD - (enLine - stLine) + 2; i ++)
        printw("\n");
    if (mode == NORMAL)
        printw("   NORMAL       ");
    if (mode == INSERT)
        printw("   INSERT       ");
    if (mode == VISUAL)
        printw("   VISUAL       ");
    if (strlen(fileName))
        printw("%s  ", fileName);
    else
        printw("no file  ");
    if (!saved)
        printw("+");
    cmdBar[cmdPtr] = '\0';
    printw("\n   %s", cmdBar);
    printw("\n   %s", err); err[0] = '\0';
    if (!cmdPtr)
        move(curLine - stLine, curIndx + 4);
    else
        move(MAX_LTD + 2 + 1, 3 + cmdPtr);
}

void _insert(char *str){
    char tmp[MAX_N];
    for (int i = 0; i < curIndx; i ++)
        tmp[i] = txt[curLine][i];
    int strLen = strlen(str), lineLen = strlen(txt[curLine]);
    for (int i = 0; i < strLen; i ++)
        tmp[curIndx + i] = str[i];
    for (int i = curIndx; i < lineLen; i ++)
        tmp[i + strLen] = txt[curLine][i];
    tmp[lineLen + strLen] = '\0';
    FILE *f = fopen("/home/mahdishokoufi/ostream.txt", "w");
    for (int i = 1; i <= lineCnt; i ++){
        if (i == curLine)
            fputs(tmp, f);
        else
            fputs(txt[i], f);
    }
    fclose(f);
    f = fopen("/home/mahdishokoufi/ostream.txt", "r");
    lineCnt = 0;
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, lineCnt ++);
    if (txt[lineCnt][strlen(txt[lineCnt]) - 1] == '\n'){
        lineCnt ++;
        txt[lineCnt][0] = '\0';
    }
    if (strLen == 1 && str[0] == '\n'){
        curLine ++;
        curIndx = 0;
        if (enLine - stLine + 1 < MAX_LTD)
            enLine ++;
        else if (enLine - curLine < 3 && enLine < lineCnt)
            stLine ++, enLine ++;
    }
    else
        curIndx += strLen;
    saved = 0;
    fclose(f);
    remove("/home/mahdishokoufi/ostream.txt");
}

void _remove(int R, int size){
    char tmp[MAX_N * MAX_N];
    int ptr = 0, pos = -1;
    for (int i = 1; i <= lineCnt; i ++){
        int len = strlen(txt[i]);
        for (int j = 0; j < len; j ++){
            tmp[ptr ++] = txt[i][j];
            if (i == curLine && j == R)
                pos = ptr - 1;
        }
        if (i == curLine && R == len)
            pos = ptr;
    }
    if (pos - size < 0 || size <= 0)
        return;
    int st = pos - size;
    int en = pos;
    st = st < 0 ? 0 : st;
    en = en > ptr ? ptr : en;
    int sz = strlen(txt[curLine - 1]) - 1;
    FILE *f = fopen("/home/mahdishokoufi/ostream.txt", "w");
    for (int i = 0; i < st; i ++)
        fputc(tmp[i], f);
    for (int i = en; i < ptr; i ++)
        fputc(tmp[i], f);
    fclose(f);
    f = fopen("/home/mahdishokoufi/ostream.txt", "r");
    lineCnt = 0;
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, lineCnt ++);
    if (txt[lineCnt][strlen(txt[lineCnt]) - 1] == '\n'){
        lineCnt ++;
        txt[lineCnt][0] = '\0';
    }
    if (R == 0){
        curLine --;
        curIndx = sz;
        enLine = min(enLine, lineCnt);
        if (curLine - stLine < 3 && stLine > 1)
            stLine --, enLine --;
    }
    saved = 0;
    fclose(f);
    remove("/home/mahdishokoufi/ostream.txt");
}

void _autoIndent(){
    int cntOpn = 0;
    for (int i = 1; i <= lineCnt; i ++){
        int len = strlen(txt[i]);
        for (int j = 0; j < len; j ++){
            if (txt[i][j] == '{')
                cntOpn ++;
            if (txt[i][j] == '}')
                cntOpn --;
            if (cntOpn < 0)
                return; //wrong braces
        }
    }
    if (cntOpn)
        return; //wrong braces
    char **res = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        res[i] = malloc(MAX_N * sizeof(char));
    int crLine = 1, ptr = 0;
    for (int i = 1; i <= lineCnt; i ++){
        int len = strlen(txt[i]), j = 0;
        while (j < len){
            if (ptr == 0 && (txt[i][j] == ' ' || txt[i][j] == '\t' || txt[i][j] == '\n')){
                while (j < len && (txt[i][j] == ' ' || txt[i][j] == '\t' || txt[i][j] == '\n'))
                    j ++;
                continue;
            }
            if (txt[i][j] == '{'){
                while (ptr >= 1 && (res[crLine][ptr - 1] == ' ' || res[crLine][ptr - 1] == '\t' || res[crLine][ptr - 1] == '\n'))
                    ptr --;
                if (ptr > 0)
                    res[crLine][ptr ++] = ' ';
                if (ptr == 0)
                    for (int k = 0; k < cntOpn; k ++)
                        for (int t = 0; t < 4; t ++)
                            res[crLine][ptr ++] = ' ';
                res[crLine][ptr ++] = '{';
                res[crLine][ptr ++] = '\n';
                res[crLine][ptr ++] = '\0';
                crLine ++; ptr = 0;
                j ++;
                cntOpn ++;
            }
            else if (txt[i][j] == '}'){
                while (ptr >= 1 && (res[crLine][ptr - 1] == ' ' || res[crLine][ptr - 1] == '\t' || res[crLine][ptr - 1] == '\n'))
                    ptr --;
                if (ptr > 0){
                    res[crLine][ptr ++] = '\n';
                    res[crLine][ptr ++] = '\0';
                    crLine ++; ptr = 0;
                }
                cntOpn --;
                for (int k = 0; k < cntOpn; k ++)
                    for (int t = 0; t < 4; t ++)
                        res[crLine][ptr ++] = ' ';
                res[crLine][ptr ++] = '}';
                res[crLine][ptr ++] = '\n';
                res[crLine][ptr ++] = '\0';
                crLine ++; ptr = 0;
                j ++;
            }
            else{
                if (ptr == 0)
                    for (int k = 0; k < cntOpn; k ++)
                        for (int t = 0; t < 4; t ++)
                            res[crLine][ptr ++] = ' ';
                res[crLine][ptr ++] = txt[i][j];
                if (txt[i][j] == '\n'){
                    res[crLine][ptr ++] = '\0';
                    crLine ++; ptr = 0;
                }
                j ++;
            }
        }
    }
    FILE *f = fopen("/home/mahdishokoufi/ostream.txt", "w");
    for (int i = 1; i < crLine; i ++)
        fputs(res[i], f);
    fclose(f);
    f = fopen("/home/mahdishokoufi/ostream.txt", "r");
    lineCnt = 0;
    for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, lineCnt ++);
    if (txt[lineCnt][strlen(txt[lineCnt]) - 1] == '\n'){
        lineCnt ++;
        txt[lineCnt][0] = '\0';
    }
    curLine = 1; curIndx = 0;
    saved = 0;
    stLine = 1; enLine = min(lineCnt, MAX_LTD);
    fclose(f);
    remove("/home/mahdishokoufi/ostream.txt");
    free(res);
}

void _find(char *str){
    fndPtr = 0;
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
    for (int crLine = 1; crLine <= lineCnt; crLine ++){
        int curLen = strlen(txt[crLine]), crIndx = 0;
        while (crIndx < curLen){
            int fndFlg = 1, st = -1, en = -1;
            if (begFlg && !spcFlg){ //A B *a
                while (crIndx < curLen && txt[crLine][crIndx] == ' ')
                    crIndx ++;
                int endWrd = crIndx;
                while (endWrd < curLen && txt[crLine][endWrd] != ' ')
                    endWrd ++;
                if (crIndx - pos < 0)
                    fndFlg = 0;
                for (int i = 0; i < pos && fndFlg; i ++)
                    if (txt[crLine][crIndx - 1 - i] != str[pos - 1 - i])
                        fndFlg = 0;
                if (fndFlg){
                    fndFlg = 0;
                    for (int i = endWrd; i - len + pos + 1 >= crIndx && !fndFlg; i --){
                        fndFlg = 1;
                        for (int j = 0; j < len - (pos + 1); j ++)
                            if (str[len - j - 1] != txt[crLine][i - j - 1])
                                fndFlg = 0;
                        if (fndFlg)
                            en = i;
                    }
                    if (fndFlg)
                        st = crIndx - pos;
                }
            }
            else{
                int L = pos + 1, R = len; //A or A *a B
                if (endFlg) //A a* B
                    L = 0, R = pos;
                if (crIndx + (R - L) - 1 >= curLen)
                    fndFlg = 0;
                for (int i = 0; i < R - L && fndFlg; i ++)
                    if (txt[crLine][crIndx + i] != str[L + i])
                        fndFlg = 0;
                if (pos == -1 && fndFlg) //A
                    st = crIndx, en = crIndx + len;
                if (begFlg && fndFlg){ //A *a B
                    int i = crIndx - 1;
                    while (0 <= i && txt[crLine][i] != ' ')
                        i --;
                    L = 0; R = pos;
                    if (i - (R - L - 1) < 0)
                        fndFlg = 0;
                    for (int j = 0; j < R - L && fndFlg; j ++)
                        if (txt[crLine][i - j] != str[R - 1 - j])
                            fndFlg = 0;
                    if (fndFlg)
                        st = i - (R - L - 1), en = crIndx + len - pos - 1;
                }
                if (endFlg && fndFlg){ //A a* B
                    int i = crIndx + R - L;
                    while (i < curLen && (txt[crLine][i] != ' ' && txt[crLine][i] != '\n'))
                        i ++;
                    L = pos + 1; R = len;
                    if (i + (R - L) - 1 >= curLen)
                        fndFlg = 0;
                    for (int j = 0; j < R - L && fndFlg; j ++)
                        if (txt[crLine][i + j] != str[L + j])
                            fndFlg = 0;
                    if (fndFlg)
                        st = crIndx, en = i + (R - L);
                }
            }
            if (fndFlg && (fndPtr <= 0 || !(fnd[fndPtr - 1][0] == crLine && fnd[fndPtr - 1][1] < en && st < fnd[fndPtr - 1][2]))){
                fnd[fndPtr][0] = crLine;
                fnd[fndPtr][1] = st;
                fnd[fndPtr][2] = en;
                fndPtr ++;
                crIndx = en - 1;
            }
            crIndx ++;
        }
    }
}

void handleFind(){
    _find(cmdBar + 1);
    cmdBar[0] = '\0'; cmdPtr = 0;
    if (fndPtr == 0){
        char c = getch();
        while (c == 'n')
            c = getch();
        return;
    }
    for (int i = 0; i < fndPtr; i ++){
        for (int j = fnd[i][1]; j < fnd[i][2]; j ++)
            color[fnd[i][0]][j] = 2;
    }
    int cur = 0;
    traverse(fnd[0][0], fnd[0][1]);
    display();
    char c = getch();
    while (c == 'n'){
        cur = (cur + 1) % fndPtr;
        traverse(fnd[cur][0], fnd[cur][1]);
        display();
        c = getch();
    }
    for (int i = 0; i < fndPtr; i ++){
        for (int j = fnd[i][1]; j < fnd[i][2]; j ++)
            color[fnd[i][0]][j] = 1;
    }
    display();
}

void handleReplace(char *str1, char *str2, int _at, int _all){
    _find(str1);
    if (!_at && !_all)
        _at = 1;
    if (_at){
        if (_at > fndPtr)
            return;
        traverse(fnd[_at - 1][0], fnd[_at - 1][1]);
        _remove(fnd[_at - 1][2], fnd[_at - 1][2] - fnd[_at - 1][1]);
        _insert(str2);
        return;
    }
    for (int i = fndPtr - 1; i >= 0; i --){
        traverse(fnd[i][0], fnd[i][1]);
        _remove(fnd[i][2], fnd[i][2] - fnd[i][1]);
        _insert(str2);
    }
}

void handleCommand(){
    char **inp = malloc(MAX_N * sizeof(char *));
    for (int i = 0; i < MAX_N; i ++)
        inp[i] = malloc(MAX_N * sizeof(char));
    int cur = 0, ptr = 0, inQ = 0;
    for (int i = 1; i < cmdPtr; i ++){
        if (cmdBar[i] == '"'){
            if (ptr == 0)
                inQ = 1;
            else if (inQ && inp[cur][ptr - 1] == '\\')
                inp[cur][ptr - 1] = '"';
            else if (!inQ)
                inp[cur][ptr ++] = '"';
            else
                inQ = 0;
        }
        else if (cmdBar[i] == ' '){
            if (inQ)
                inp[cur][ptr ++] = ' ';
            else if (ptr){
                inp[cur ++][ptr] = '\0';
                ptr = inQ = 0;
            }
        }
        else if (cmdBar[i] == 'n'){
            if (ptr >= 2 && inp[cur][ptr - 1] == '\\' && inp[cur][ptr - 2] == '\\')
                inp[cur][ptr - 1] = 'n';
            else if (ptr >= 1 && inp[cur][ptr - 1] == '\\')
                inp[cur][ptr - 1] = '\n';
            else
                inp[cur][ptr ++] = 'n';
        }
        else
            inp[cur][ptr ++] = cmdBar[i];
    }
    if (ptr){
        inp[cur ++][ptr] = '\0';
        ptr = inQ = 0;
    }
    if (!strcmp(inp[0], "save")){
        //save
        if (strlen(fileName) == 0){
            //error
            char msg[] = "Error: choose a name with :saveas <file_name>\n";
            for (int i = 0; i < strlen(msg); i ++)
                err[i] = msg[i];
            err[strlen(msg)] = '\0';
        }
        else{
            FILE *f = fopen(fileName, "w");
            for (int i = 1; i <= lineCnt; i ++)
                fputs(txt[i], f);
            fclose(f);
            saved = 1;
        }
    }
    if (!strcmp(inp[0], "saveas")){
        //saveas /root/...
        int len = strlen(inp[1]);
        for (int i = 1; i < len; i ++)
            fileName[i - 1] = inp[1][i];
        fileName[len - 1] = '\0';
        FILE *f = fopen(fileName, "w");
        for (int i = 1; i <= lineCnt; i ++)
            fputs(txt[i], f);
        fclose(f);
        saved = 1;
    }
    if (!strcmp(inp[0], "open")){
        //open /root/...
        if (strlen(fileName) != 0){ //save current file
            FILE *f = fopen(fileName, "w");
            for (int i = 1; i <= lineCnt; i ++)
                fputs(txt[i], f);
            fclose(f);
            saved = 1;
        }
        //open new file
        mode = NORMAL;
        curLine = 1; curIndx = 0;
        lineCnt = 0;
        for (int i = 1; i < strlen(inp[1]); i ++)
            fileName[i - 1] = inp[1][i];
        fileName[strlen(inp[1]) - 1] = '\0';
        FILE *f;
        if (isFileExist(fileName))
            f = fopen(fileName, "r");
        else
            f = fopen(fileName, "w+");
        for (int i = 1; fgets(txt[i], MAX_N, f) != NULL; i ++, lineCnt ++);
        if (txt[lineCnt][strlen(txt[lineCnt]) - 1] == '\n'){
            lineCnt ++;
            txt[lineCnt][0] = '\0';
        }
        fclose(f);
        saved = 1;
        stLine = 1; enLine = min(lineCnt, MAX_LTD);
    }
    if (!strcmp(inp[0], "replace")){
        //replace --str1 str1 --str2 str2 -at || -all
        int _at = 0, _all = 0;
        if (5 < cur && !strcmp(inp[5], "-at"))
            _at = toInt(inp[6]);
        if (5 < cur && !strcmp(inp[5], "-all"))
            _all = 1;
        handleReplace(inp[2], inp[4], _at, _all);
    }
}

void handleNORMAL(char c){
    if (cmdPtr > 0 || c == ':' || c == '/'){ //typing in cmd bar
        if (c == '\n'){
            cmdBar[cmdPtr] = '\0';
            if (cmdBar[0] == ':'){
                handleCommand();
                cmdBar[0] = '\0'; cmdPtr = 0;
                return;
            }
            //handle find /<exp>
            handleFind();
            return;
        }
        if (c == BACKSPACE)
            cmdPtr --;
        else
            cmdBar[cmdPtr ++] = c;
        return;
    }
    if (c == 'p'){ //paste
        _insert(clipboard);
    }
    if (c == 'h' || c == 'j' || c == 'k' || c == 'l'){ //navigation
        navigate(c);
    }
    if (c == 'u'){ //undo
        //TODO
    }
    if (c == '='){ //auto-indent
        _autoIndent();
    }
    if (c == 'i'){ //change mode to insert
        mode = INSERT;
        cmdBar[0] = '\0';
    }
    if (c == 'v'){ //change mode to visual
        mode = VISUAL;
        cmdBar[0] = '\0';
        stX = curLine;
        stY = curIndx;
    }
    return;
}

void handleINSERT(char c){
    if (c == ESCAPE){ //change mode to normal
        mode = NORMAL;
        return;
    }
    if (c == BACKSPACE){ //remove
        int f = curIndx != 0;
        _remove(curIndx, 1);
        if (f)
            curIndx --;
        return;
    }
    //insert
    char str[2]; str[0] = c; str[1] = '\0';
    _insert(str);
}

void handleVISUAL(char c){
    if (c == ESCAPE){ //change mode to normal
        mode = NORMAL;
        stX = stY = -1;
        //maybe remove highlights later ...
    }
    if (c == 'd'){ //cut
        int L = min(stY, curIndx);
        int R = max(stY, curIndx);
        for (int i = 0; i < R - L; i ++)
            clipboard[i] = txt[curLine][L + i];
        clipboard[R - L] = '\0';
        _remove(R, R - L);
        mode = NORMAL;
        curIndx = L;
        stX = stY = -1;
        //maybe remove highlights later ...
    }
    if (c == 'y'){ //copy
        int L = min(stY, curIndx);
        int R = max(stY, curIndx);
        for (int i = 0; i < R - L; i ++)
            clipboard[i] = txt[curLine][L + i];
        clipboard[R - L] = '\0';
        mode = NORMAL;
        stX = stY = -1;
        //maybe remove highlights later ...
    }
    if (c == 'h' || c == 'j' || c == 'k' || c == 'l'){ //navigation
        navigate(c);
    }
}

int handle(){
    char c = getch();
    if (mode == NORMAL)
        handleNORMAL(c);
    else if (mode == INSERT)
        handleINSERT(c);
    else
        handleVISUAL(c);
    display();
    return 0;
}

int main(){
    init();
    int end = 0;
    display();
    while (!end)
        end = handle();
    endwin();
    return 0;
}