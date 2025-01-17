#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

#define ROWS 15
#define COLS 20
#define MINES 30

typedef struct {
    int is_mine;
    int is_revealed;
    int is_flagged;
    int adjacent_mines;
} Cell;

Cell **grid;
int cursor_x = 0, cursor_y = 0;

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

void handle_signal(int sig) {
    printf("\nReceived signal %d. Exiting gracefully...\n", sig);
    free_grid();
    reset_terminal();
    exit(0);
}

void allocate_grid() {
    grid = (Cell **)malloc(ROWS * sizeof(Cell *));
    for (int i = 0; i < ROWS; i++) {
        grid[i] = (Cell *)malloc(COLS * sizeof(Cell));
    }
}

void free_grid() {
    for (int i = 0; i < ROWS; i++) {
        free(grid[i]);
    }
    free(grid);
}

void init_grid() {
    srand(time(NULL));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j].is_mine = 0;
            grid[i][j].is_revealed = 0;
            grid[i][j].is_flagged = 0;
            grid[i][j].adjacent_mines = 0;
        }
    }

    
    for (int m = 0; m < MINES; m++) {
        int x, y;
        do {
            x = rand() % ROWS;
            y = rand() % COLS;
        } while (grid[x][y].is_mine);
        grid[x][y].is_mine = 1;

       
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int nx = x + i;
                int ny = y + j;
                if (nx >= 0 && ny >= 0 && nx < ROWS && ny < COLS) {
                    grid[nx][ny].adjacent_mines++;
                }
            }
        }
    }
}

void display_grid() {
    system("clear");
    printf("Minesweeper (w: up, a: left, s: down, d: right, f: flag, o: open, q: quit)\n");
    printf("Grid size: %dx%d | Mines: %d\n\n", ROWS, COLS, MINES);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == cursor_x && j == cursor_y) {
                printf("[");
            } else {
                printf(" ");
            }

            if (grid[i][j].is_revealed) {
                if (grid[i][j].is_mine) {
                    printf("*");
                } else if (grid[i][j].adjacent_mines > 0) {
                    printf("%d", grid[i][j].adjacent_mines);
                } else {
                    printf(" ");
                }
            } else if (grid[i][j].is_flagged) {
                printf("F");
            } else {
                printf(".");
            }

            if (i == cursor_x && j == cursor_y) {
                printf("]");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void reveal_cell(int x, int y) {
    if (x < 0 || y < 0 || x >= ROWS || y >= COLS || grid[x][y].is_revealed || grid[x][y].is_flagged) {
        return;
    }

    grid[x][y].is_revealed = 1;

    
    if (grid[x][y].adjacent_mines == 0 && !grid[x][y].is_mine) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                reveal_cell(x + i, y + j);
            }
        }
    }
}

int check_victory() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j].is_mine && !grid[i][j].is_revealed) {
                return 0; 
            }
        }
    }
    return 1; 
}

void game_over() {
    display_grid();
    printf("\nGame Over! You hit a mine!\n");
    sleep(2); 
}

void game_loop() {
    char input;
    while (1) {
        display_grid();
        input = getchar();

        if (input == 'q') {
            printf("Exiting...\n");
            break;
        } else if (input == 'w') {
            cursor_x = (cursor_x - 1 + ROWS) % ROWS;
        } else if (input == 'a') {
            cursor_y = (cursor_y - 1 + COLS) % COLS;
        } else if (input == 's') {
            cursor_x = (cursor_x + 1) % ROWS;
        } else if (input == 'd') {
            cursor_y = (cursor_y + 1) % COLS;
        } else if (input == 'f') {
            grid[cursor_x][cursor_y].is_flagged = !grid[cursor_x][cursor_y].is_flagged;
        } else if (input == 'o') {
            if (grid[cursor_x][cursor_y].is_mine) {
                game_over();
                break;
            }
            reveal_cell(cursor_x, cursor_y);

            if (check_victory()) {
                printf("\nCongratulations! You won!\n");
                sleep(2); 
                break;
            }
        }
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    allocate_grid();
    init_terminal();
    init_grid();
    game_loop();
    free_grid();
    return 0;
}



