#include <ncurses.h>    /*display functions*/
#include <memory.h>     /*memcpy()*/
#include <stdlib.h>     /*srand(), rand()*/
#include <time.h>       /*clock(), nanosleep()*/

//Game dimensions
#define W 100
#define H 50
#define T_PER_FRAME 0.05

void disp(char grid[H][W]);
void iterate(char grid[H][W], char buff[H][W]);
char count(char grid[H][W], int x, int y);
void init_rand(char grid[H][W]);
void msleep(double s_time);

int main(void){
    initscr(); //start curses mode
    
    int h, w;
    clear();
    getmaxyx(stdscr, h, w);
    if(w<W+1 || h<H){
        endwin();
        printf("Terminal too small: w=%d, W=%d   h=%d, H=%d\n", w, W, h, H);
        exit(-1);
    }

    char grid[H][W]; //Initialise grid
    char buff[H][W];
    init_rand(grid);

    clock_t t1; //Initialise clock for rate limiting
    double elapsed;
    
    disp(grid);
    while(1){
        t1 = clock();
        elapsed = 0;
        iterate(grid, buff);
        memcpy(grid, buff, sizeof(char)*H*W); 
	    disp(grid);
	    
        //if there's time left, sleep it off
        elapsed = ((double) (clock() - t1)) / CLOCKS_PER_SEC;
        if(elapsed < T_PER_FRAME) 
            msleep(T_PER_FRAME-elapsed);
    }

    endwin();			/* End curses mode		  */
    return 0;
}
void disp(char grid[H][W]){
    //Displays the grid with ncurses
    unsigned int x, y;
    clear();
    for(y=0;y<H;y++) {
        for(x=0;x<W;x++){
            printw("%c", (grid[y][x] ? '#' : '.'));
        }
        printw("\n");
    }
    refresh();
    return;
}

void iterate(char grid[H][W], char buff[H][W]){
    //Performs one iteration of the Conways Game of Life
    //Puts new grid in buff[][]
    int x, y;
    int neighbours;
    for(y=0;y<H;y++) {
        for(x=0;x<W;x++){
            neighbours = count(grid, x, y);
            if(neighbours < 2)
                buff[y][x] = 0;
            else if(neighbours == 2)
                buff[y][x] = grid[y][x];
            else if(neighbours == 3)
                buff[y][x] = 1;
            else if(neighbours > 3)
                buff[y][x] = 0;
        }
    }
    return;
}

char count(char grid[H][W], const int x, const int y){
    //counts the number of neighbours of a cell
    int neighbours, i, j;
    neighbours = -grid[y][x];
    for(i=y-1;i<=y+1;i++){
        if(i<0 || i>=H)
            continue;    
        for(j=x-1;j<=x+1;j++){
            if(j<0 || j>=W)
                continue;
            if(grid[i][j]==1)
                neighbours++;
        }
    }
    return neighbours;
}

void init_rand(char grid[H][W]){
    srand( time(NULL)); //seed the random number generator
    int x, y;
    for(y=0;y<H;y++) {
        for(x=0;x<W;x++){
            grid[y][x] = (rand()%2-1)?0:1;
        }
    }
    return;
}

void msleep(double s_time){
    //sleeps for s_time (seconds)
    struct timespec req = {0};
    req.tv_nsec = s_time * 1000000000L;
    nanosleep(&req, (struct timespec *)NULL);
    return;
}
