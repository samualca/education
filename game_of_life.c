#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#define ROW 25
#define COL 80

void input(int a[ROW][COL], int row, int col, FILE *initpos);
FILE* init_pos();
int menu();
void output(int a[ROW][COL], int row, int col);
int geometry(int a1[ROW][COL], int a2[ROW][COL], int row, int col);
void acopy(int a1[ROW][COL], int a2[ROW][COL], int row, int col);
void nuller(int a[ROW][COL], int row, int col);
int game_control(int *delay);
void speedcount(int delay);
int contin(int a[ROW][COL], int irow, int icol);
int born(int a[ROW][COL], int irow, int icol);
int die(int a[ROW][COL], int irow, int icol);
int indent(int a1[ROW][COL], int a2[ROW][COL], int row, int col);

int main() {
    int endgame = 0;
    int delay = 100000;
    int row = ROW;
    int col = COL;
    int a1[ROW][COL], a2[ROW][COL];
    FILE *initpos;
    initscr();
    timeout(1);
    noecho();
    nuller(a1, row, col);
    nuller(a2, row, col);
    curs_set(0);
    initpos = init_pos();
    input(a1, row, col, initpos);
    fclose(initpos);
    while (1) {
        if (!game_control(&delay))
           endgame = 1;
        output(a1, row, col);
        refresh();
        if (!geometry(a1, a2, row, col)) {
            sleep(3);
            endgame = 1;
        }
        usleep(delay);
        clear();
        if (endgame)
            break;
    }
    endwin();
}

void input(int a[ROW][COL], int row, int col, FILE *initpos) {
    for (int i = 0; i < row; i++) {
        int n;
        n = 0;
        do {
            fscanf(initpos, "%d", &(a[i][n]));
            n++;
        } while (n < col);
    }
}

FILE* init_pos() {
    FILE *initpos = NULL;
    int type = menu();
    if (type == '1')
        initpos = fopen("1", "r");
    else if (type == '2')
        initpos = fopen("2", "r");
    else if (type == '3')
        initpos = fopen("3", "r");
    else if (type == '4')
        initpos = fopen("4", "r");
    else if (type == '5')
        initpos = fopen("5", "r");
    return initpos;
}

int menu() {
    int choose = 0;
    WINDOW *cong;
    cong = subwin(stdscr, 11, 30, 6, 25);
    wprintw(cong, "Choose init position\n\n");
    wprintw(cong, "1 - First world\n\n");
    wprintw(cong, "2 - Second world\n\n");
    wprintw(cong, "3 - Third world\n\n");
    wprintw(cong, "4 - Fourth world\n\n");
    wprintw(cong, "5 - Fifth world");
    wrefresh(cong);
    do {
        choose = getch();
    } while (choose != '1' && choose != '2' && choose != '3' && choose != '4' && choose != '5');
    wclear(cong);
    delwin(cong);
    return choose;
}

void output(int a[ROW][COL], int row, int col) {
    for (int i = 0; i < row; i++)
            for (int n = 0; n < col; n++)
                if (a[i][n])
                    mvwaddch(stdscr, i, n, '*');
}

int born(int a[ROW][COL], int irow, int icol) {
    int count = 0;
    int res = 0;
    for (int i = irow - 1; i <= irow + 1; i++) {
        for (int n = icol - 1; n <= icol + 1; n++) {
            if (i == irow && n == icol)
                continue;
            if (a[(i + ROW) % ROW][(n + COL) % COL])
                count++;
        }
    }
    if (count == 3)
        res = 1;
    return res;
}

int contin(int a[ROW][COL], int irow, int icol) {
    int count = 0;
    int res = 0;
    for (int i = irow - 1; i <= irow + 1; i++) {
        for (int n = icol - 1; n <= icol + 1; n++) {
            if (i == irow && n == icol)
                continue;
            if (a[(i + ROW) % ROW][(n + COL) % COL])
                count++;
        }
    }
    if (count == 3 || count == 2)
        res = 1;
    return res;
}

int die(int a[ROW][COL], int irow, int icol) {
    int count = 0;
    int res = 0;
    for (int i = irow - 1; i <= irow + 1; i++) {
        for (int n = icol - 1; n <= icol + 1; n++) {
            if (i == irow && n == icol)
                continue;
            if (a[(i + ROW) % ROW][(n + COL) % COL])
                count++;
        }
    }
    if (count > 3 || count < 2)
        res = 1;
    return res;
}

int geometry(int a1[ROW][COL], int a2[ROW][COL], int row, int col) {
    int res = 1;
    for (int i = 0; i < row; i++) {
        for (int n = 0; n < col; n++) {
            if (a1[i][n] && die(a1, i, n))
                a2[i][n] = 0;
            if (a1[i][n] && contin(a1, i, n))
                a2[i][n] = 1;
            if ((!a1[i][n]) && born(a1, i, n))
                a2[i][n] = 1;
        }
    }
    if (indent(a1, a2, row, col))
        res = 0;
    acopy(a1, a2, row, col);
    nuller(a2, row, col);
    return res;
}

void acopy(int a1[ROW][COL], int a2[ROW][COL], int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int n = 0; n < col; n++) {
            a1[i][n] = a2[i][n];
        }
    }
}

void nuller(int a[ROW][COL], int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int n = 0; n < col; n++) {
            a[i][n] = 0;
        }
    }
}

int game_control(int *delay) {
    int c = 0;
    int res = 1;
    c = getch();
    if (c == 'f')
        (*delay) += 10000;
    if (c == 'r')
        (*delay) -= 10000;
    if (*delay == 0)
        (*delay) += 10000;
    if (*delay == 310000)
        (*delay) -= 10000;
    if (c == 'q')
        res = 0;
    return res;
}

int indent(int a1[ROW][COL], int a2[ROW][COL], int row, int col) {
    int count = 0;
    int res = 0;
    for (int i = 0; i < row; i++) {
        for (int n = 0; n < col; n++) {
            if (a1[i][n] == a2[i][n])
                count++;
        }
    }
    if (count == row * col)
        res = 1;
    return res;
}
