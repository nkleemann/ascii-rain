/*

                                 000      00
                           0000000   0000
              0      00  00000000000000000
            0000 0  000000000000000000000000       0
         000000000000000000000000000000000000000 000
        0000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000
            C
                O M        |
                    F          |
                 Y                         |         |
            |                R  A
                                  I N
                       I N   |
              |                                    |
        |                            Y O
                |                   U        R
                   |            T E

                                     R   |   |
                         |            M
                               I N
                                        AL


    Although this was a purely fun-motivated project I
    challenged myself to write this code clean & leak-free.

    If you find bugs or leaks feel free to contact me or fork
    this. That would be awesome.

    @ Nik, 07.2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>


//
//  GLOBALS
//

int userResized = 0;
int slowerDrops = 0; 

typedef struct
{
    int  w;
    int  h;
    int  speed;
    int  color;
    char shape;
} Drop;

/**
 * Since we want the total number of drops to
 * be terminal-size dependent we need a dynamic/
 * resizeable data structure containing an array of
 * drops.
 */

typedef struct
{
    Drop *drops;
    int size;
    int capacity;

} d_Vector;


//
//  PROTOTYPES
//

Drop d_create();
void d_fall(Drop *d);
void d_show(Drop *d);

void v_init(d_Vector *v, int cap);
void v_free(d_Vector *v);
void v_delete(d_Vector *v);
void v_resize(d_Vector *v, int newCap);
void v_add(d_Vector *v, Drop d);
Drop *v_getAt(d_Vector *v, int pos);

void initCurses();
void exitCurses();

int pRand(int min, int max);
int getNumOfDrops();
void handleResize();
void exitErr(const char *err);
void usage();


//
//  FUNCTIONS - DROP
//

Drop d_create()
{
    Drop d;

    d.w = pRand(0, COLS);
    d.h = pRand(0, LINES);

    if (slowerDrops)
    {
        d.speed = pRand(1, 3);
        (d.speed < 2) ? (d.shape = '|') : (d.shape = ':');
    }

    else
    {
        d.speed = pRand(1, 6);
        (d.speed < 3) ? (d.shape = '|') : (d.shape = ':');
    }

    int x = d.speed;

    // patented
    d.color = (int) ((0.0416 * (x - 4)
                             * (x - 3)
                             * (x - 2) - 4)
                             * (x - 1) + 255);
    return d;
}

void d_fall(Drop *d)
{
    d->h += d->speed;

    if (d->h >= LINES-1)
        d->h = pRand(0, 10);
}

void d_show(Drop *d)
{
    attron(COLOR_PAIR(d->color));
    mvaddch(d->h, d->w, d->shape);
}


//
//  FUNCTIONS - VECTOR
//

void v_init(d_Vector *v, int cap)
{
    if (cap > 0 && v != 0)
    {
        v->drops = (Drop *) malloc(sizeof(Drop) * cap);

        if (v->drops != 0)
        {
            v->size = 0;
            v->capacity = cap;
        }
        else
            exitErr("\n*DROP ARRAY IS >NULL<*\n");
    }
    else
        exitErr("\n*VECTOR INIT FAILED*\n");
}

void v_free(d_Vector *v)
{
    if(v->drops != 0)
    {
        free(v->drops);
        v->drops = 0;
    }

    v->size = 0;
    v->capacity = 0;
}

void v_delete(d_Vector *v)
{
    v_free(v);
}

void v_resize(d_Vector *v, int newCap)
{
    d_Vector newVec;
    v_init(&newVec, newCap);

    for (int i = 0; i < newCap; i++)
        v_add(&newVec, d_create());

    v_free(v);
    *v = newVec;
}

void v_add(d_Vector *v, Drop d)
{
    if(v->size >= v->capacity)
        v_resize(v, 2 * v->capacity);

    v->drops[v->size] = d;
    v->size++;
}

Drop *v_getAt(d_Vector *v, int pos)
{
    if ((pos < v->size) && (pos >= 0))
        return &(v->drops[pos]);

    exitErr("\n*BAD ACCESS*\n");
}


//
//  FUNCTIONS - CURSES
//

void initCurses()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1);
    curs_set(0);

    if ((curs_set(0)) == 1)
        exitErr("\n*Terminal emulator lacks capabilities.\n(Can't hide Cursor).*\n");

    timeout(0);
    signal(SIGWINCH, handleResize);

    int hazColors = has_colors() && can_change_color();
    if (hazColors)
    {
        use_default_colors();
        start_color();

        for (short i = 0; i < COLORS; i++)
            init_pair(i + 1, i, -1);
    }
    else
        exitErr("\n*Terminal emulator lacks capabilities.\n(Can't have colors).\n*");

}

void exitCurses()
{
    curs_set(1);
    clear();
    refresh();
    resetty();
    endwin();
}


//
//  UTILS
//

int pRand(int min, int max)
{
    max -= 1;
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void handleResize()
{
    endwin();

    userResized = 1;

    refresh();
    erase();
}

void exitErr(const char *err)
{
    exitCurses();
    printf("%s", err);
    exit(0);
}

int getNumOfDrops()
{
    int nDrops = 0;

    if ((LINES < 20 && COLS > 100) || (COLS < 100 && LINES < 40))
    {
        nDrops = (int) (COLS * 0.75);

        // Watch that state..
        slowerDrops = 1;
    }
    else
    {
        nDrops = (int) (COLS * 1.5);
        slowerDrops = 0;
    }

    return nDrops;
}

void usage()
{
    printf(" Usage: rain\n");
    printf("No arguments supported yet. It's just rain, after all.\n");
    printf("Hit 'q' to exit.\n");
}


int main(int argc, char **argv)
{
    if (argc != 1)
    {
        usage();
        exit(0);
    }

    // User KeyEvent
    int key;

    srand((unsigned int) getpid());
    initCurses();

    int dropsTotal = getNumOfDrops();
    d_Vector drops;
    v_init(&drops, dropsTotal);

    for (int i = 0; i < dropsTotal; i++)
        v_add(&drops, d_create());


    //
    //  DRAW-LOOP
    //

    while (1)
    {

        if (userResized)
        {
            // Stabilizing hectic user..
            usleep(90000);

            dropsTotal = getNumOfDrops();
            v_resize(&drops, dropsTotal);

            userResized = 0;
        }

        dropsTotal = getNumOfDrops();

        for (int i = 0; i < dropsTotal; i++)
        {
            d_fall(v_getAt(&drops, i));
            d_show(v_getAt(&drops, i));
        }

        // Frame Delay
        usleep(30000);

        if ((key = wgetch(stdscr)) == 'q')
            break;

        erase();
    }

    // Free pointers & exit gracefully
    v_delete(&drops);
    exitCurses();

}
