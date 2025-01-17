#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>

#define MAX_GAMES 100

char *game_list[MAX_GAMES];
int game_count = 0;
int selected_button = 0; // 0: Games, 1: Exit
int selected_game = 0;   // Oyun seçimi

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
    printf("\nExiting main menu gracefully...\n");
    exit(0);
}

void load_games() {
    DIR *dir = opendir("."); // `.` ile kendi bulunduğu dizini açar
    if (dir == NULL) {
        perror("Failed to open current directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Kendisi dışında `game_` ile başlayan çalıştırılabilir dosyaları al
        if (strncmp(entry->d_name, "game_", 5) == 0) {
            game_list[game_count] = strdup(entry->d_name);
            game_count++;
        }
    }
    closedir(dir);

    if (game_count == 0) {
        printf("No games found in the current directory. Exiting...\n");
        exit(EXIT_FAILURE);
    }
}

void display_main_screen(int show_games) {
    system("clear");
    printf("==== Game Console Main Menu ====\n");
    printf("Press 'q' to exit.\n\n");

    // Butonlar
    if (selected_button == 0) {
        printf("[Games]   Exit\n");
    } else {
        printf(" Games   [Exit]\n");
    }

    // Eğer oyunlar gösteriliyorsa listele
    if (show_games) {
        printf("\nAvailable Games:\n");
        for (int i = 0; i < game_count; i++) {
            if (i == selected_game) {
                printf("-> %s\n", game_list[i]);
            } else {
                printf("   %s\n", game_list[i]);
            }
        }
        printf("\nPress 'Enter' to play the selected game or 'q' to exit the program.\n");
    }
}

void free_game_list() {
    for (int i = 0; i < game_count; i++) {
        free(game_list[i]);
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    init_terminal();
    load_games();

    char input;
    int show_games = 0; // Oyunlar listeleniyor mu?

    while (1) {
        display_main_screen(show_games);
        input = getchar();

        if (input == 'q') {
            if (show_games) {
                // Oyun listesi açıkken `q` ile programdan çık
                printf("Exiting...\n");
                break;
            } else {
                // Ana menüde `q` ile çıkış
                printf("Exiting main menu...\n");
                break;
            }
        } else if (input == 'a') { // Sol butona geçiş
            if (!show_games) {
                selected_button = (selected_button - 1 + 2) % 2;
            }
        } else if (input == 'd') { // Sağ butona geçiş
            if (!show_games) {
                selected_button = (selected_button + 1) % 2;
            }
        } else if (input == 'w' && show_games) { // Oyun seçimi yukarı
            selected_game = (selected_game - 1 + game_count) % game_count;
        } else if (input == 's' && show_games) { // Oyun seçimi aşağı
            selected_game = (selected_game + 1) % game_count;
        } else if (input == '\n') { // Enter tuşu
            if (show_games) {
                // Seçilen oyunu çalıştır
                char command[100];
                snprintf(command, sizeof(command), "./%s", game_list[selected_game]);
                system(command);
            } else if (selected_button == 0) {
                // Games butonuna tıklandı
                show_games = 1;
            } else if (selected_button == 1) {
                // Exit butonuna tıklandı
                printf("Exiting...\n");
                break;
            }
        }
    }

    free_game_list();
    return 0;
}






