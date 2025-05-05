// main.c

#include <stdio.h>
#include <ncurses.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263
#define ESCAPE 27

typedef enum
{
    NORMAL,
    INSERT
} Mode;

Mode mode = NORMAL;

char *stringify_mode()
{
    switch (mode)
    {
    case NORMAL:

        return "NORMAL";
        break;
    case INSERT:
        return "INSERT";
        break;
    default:
        return "NORMAL";
        break;
    }
}
int main()
{
    int row, col;

    int ch;

    initscr();
    set_escdelay(25);
    raw();
    keypad(stdscr, TRUE);
    noecho();

    getmaxyx(stdscr, row, col);
    mvprintw(row - 1, 0, "%s", stringify_mode());

    move(0, 0);

    ch = ' ';

    int x, y = 0;
    while (ch != ctrl('q'))
    {
        getyx(stdscr, y, x);

        mvprintw(row - 1, 0, "%s", stringify_mode());

        move(y, x);
        ch = getch();

        switch (mode)
        {
        case NORMAL:
            if (ch == 'i')
            {
                mode = INSERT;
            }
            break;
        case INSERT:
            if (ch == BACKSPACE)
            {
                getyx(stdscr, y, x);
                move(y, x - 1);
                delch();
            }
            else if (ch == ESCAPE)
            {
                mode = NORMAL;
            }
            else
            {
                addch(ch);
            }
            break;
        }
    }

    refresh();
    endwin();
    return 0;
}