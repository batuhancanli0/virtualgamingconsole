#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#define ROWS 15
#define COLS 15

typedef struct {
    char letter;
} Cell;

Cell **board;
int cursor_x = 0, cursor_y = 0;
int player_turn = 1;
int score[2] = {0, 0};

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
    printf("\nExiting gracefully...\n");
    free_board();
    reset_terminal();
    exit(0);
}

void allocate_board() {
    board = (Cell **)malloc(ROWS * sizeof(Cell *));
    for (int i = 0; i < ROWS; i++) {
        board[i] = (Cell *)calloc(COLS, sizeof(Cell));
    }
}

void free_board() {
    for (int i = 0; i < ROWS; i++) {
        free(board[i]);
    }
    free(board);
}

void init_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j].letter = '.';
        }
    }
}

void display_board() {
    system("clear");
    printf("SOS Game (w: up, a: left, s: down, d: right, enter: place letter, q: quit)\n");
    printf("Player 1: %d points | Player 2: %d points\n", score[0], score[1]);
    printf("Player %d's turn\n\n", player_turn);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == cursor_x && j == cursor_y) {
                printf("[");
            } else {
                printf(" ");
            }

            printf("%c", board[i][j].letter);

            if (i == cursor_x && j == cursor_y) {
                printf("]");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int check_sos(int x, int y) {
    int points = 0;

    if (board[x][y].letter == 'O') {
        if (x >= 1 && x < ROWS - 1) {
            if (board[x - 1][y].letter == 'S' && board[x + 1][y].letter == 'S') {
                points++;
            }
        }

        if (y >= 1 && y < COLS - 1) {
            if (board[x][y - 1].letter == 'S' && board[x][y + 1].letter == 'S') {
                points++;
            }
        }

        if (x >= 1 && y >= 1 && x < ROWS - 1 && y < COLS - 1) {
            if (board[x - 1][y - 1].letter == 'S' && board[x + 1][y + 1].letter == 'S') {
                points++;
            }
        }

        if (x >= 1 && y < COLS - 1 && x < ROWS - 1 && y >= 1) {
            if (board[x - 1][y + 1].letter == 'S' && board[x + 1][y - 1].letter == 'S') {
                points++;
            }
        }
    }

    if (board[x][y].letter == 'S') {
        if (x >= 2) {
            if (board[x - 1][y].letter == 'O' && board[x - 2][y].letter == 'S') {
                points++;
            }
        }
        if (x < ROWS - 2) {
            if (board[x + 1][y].letter == 'O' && board[x + 2][y].letter == 'S') {
                points++;
            }
        }

        if (y >= 2) {
            if (board[x][y - 1].letter == 'O' && board[x][y - 2].letter == 'S') {
                points++;
            }
        }
        if (y < COLS - 2) {
            if (board[x][y + 1].letter == 'O' && board[x][y + 2].letter == 'S') {
                points++;
            }
        }

        if (x >= 2 && y >= 2) {
            if (board[x - 1][y - 1].letter == 'O' && board[x - 2][y - 2].letter == 'S') {
                points++;
            }
        }
        if (x < ROWS - 2 && y < COLS - 2) {
            if (board[x + 1][y + 1].letter == 'O' && board[x + 2][y + 2].letter == 'S') {
                points++;
            }
        }

        if (x >= 2 && y < COLS - 2) {
            if (board[x - 1][y + 1].letter == 'O' && board[x - 2][y + 2].letter == 'S') {
                points++;
            }
        }
        if (x < ROWS - 2 && y >= 2) {
            if (board[x + 1][y - 1].letter == 'O' && board[x + 2][y - 2].letter == 'S') {
                points++;
            }
        }
    }

    return points;
}

void place_letter() {
    if (board[cursor_x][cursor_y].letter != '.') {
        printf("This cell is already occupied!\n");
        sleep(1);
        return;
    }

    printf("Press 's' to place S or 'o' to place O: ");
    char input;
    while ((input = getchar()) != 's' && input != 'o') {
        printf("Invalid input! Press 's' or 'o': ");
    }

    board[cursor_x][cursor_y].letter = (input == 's') ? 'S' : 'O';

    int points = check_sos(cursor_x, cursor_y);
    if (points > 0) {
        score[player_turn - 1] += points;
        printf("Player %d scored %d points!\n", player_turn, points);
        sleep(1);
    } else {
        player_turn = 3 - player_turn;
    }
}

void game_loop() {
    char input;
    while (1) {
        display_board();
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
        } else if (input == '\n') {
            place_letter();
        }
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    allocate_board();
    init_terminal();
    init_board();
    game_loop();
    free_board();
    return 0;
}







