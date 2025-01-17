#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <time.h>

#define ROWS 15
#define COLS 15

typedef struct {
    int x, y;
} Point;

Point snake[ROWS * COLS];
int snake_length = 1;
Point bait;

char grid[ROWS][COLS];

struct termios oldt, newt;

void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void init_terminal() {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    atexit(reset_terminal);
}

void init_grid() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = '.';
        }
    }

    
    snake[0].x = ROWS / 2;
    snake[0].y = COLS / 2;
    grid[snake[0].x][snake[0].y] = 'O';

    
    do {
        bait.x = rand() % ROWS;
        bait.y = rand() % COLS;
    } while (grid[bait.x][bait.y] != '.'); 

    grid[bait.x][bait.y] = 'X';
}

void display_grid() {
    system("clear");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

void move_snake(char direction) {
    
    Point new_head = snake[0];
    switch (direction) {
        case 'w': new_head.x = (new_head.x - 1 + ROWS) % ROWS; break;
        case 'a': new_head.y = (new_head.y - 1 + COLS) % COLS; break;
        case 's': new_head.x = (new_head.x + 1) % ROWS; break;
        case 'd': new_head.y = (new_head.y + 1) % COLS; break;
    }

    
    if (new_head.x == bait.x && new_head.y == bait.y) {
        snake_length++;
        do {
            bait.x = rand() % ROWS;
            bait.y = rand() % COLS;
        } while (grid[bait.x][bait.y] != '.'); 
        grid[bait.x][bait.y] = 'X';
    } else {
        
        Point tail = snake[snake_length - 1];
        grid[tail.x][tail.y] = '.';
    }

    
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    
    snake[0] = new_head;

    
    grid[new_head.x][new_head.y] = 'O';
    for (int i = 1; i < snake_length; i++) {
        grid[snake[i].x][snake[i].y] = '#';
    }
}

void handle_signal(int sig) {
    printf("\nExiting gracefully...\n");
    exit(0);
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    init_terminal();
    init_grid();

    char input = 'd'; 
    while (1) {
        display_grid();
        usleep(3000); 

        if (input == 'q') break;
        if (read(STDIN_FILENO, &input, 1) == -1) continue; 
        move_snake(input);
    }

    printf("Game Over!\n");
    return 0;
}

