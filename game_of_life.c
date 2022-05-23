#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void clear_screen(void);
void fill_field(char field_matrix[][80], int field_width, int field_height);
void input_field(char field_matrix[][80], int field_width, int field_height);
void print_field(char field_matrix[][80], int field_width, int field_height);
void speed_control(char c, int *speed_level, int *is_game_continue);
void next_genreation(char field_matrix[][80], char buffer_matrix[][80], int field_width, int field_height);
int count_neighbor(char buffer_matrix[][80], int column, int row);
void print_titres(int is_game_continue, int generation);
int is_keyboard_pressed(void);

int main() {
    int field_width = 80;
    int field_height = 25;
    char field_matrix[25][80];
    char buffer_matrix[25][80];

    int speed = 1000000000;
    int speed_level = 1;

    int is_alive = 0;
    int generation = 0;
    int is_game_continue = 1;

    fill_field(field_matrix, field_width, field_height);
    input_field(field_matrix, field_width, field_height);

    if (!freopen("/dev/tty", "r", stdin)) {
        perror("/dev/tty");
    }

    print_field(field_matrix, field_width, field_height);

    while (is_game_continue == 1) {
        struct timespec beginTime, currentTime;
        int timeDelta = 0;

        clock_gettime(CLOCK_REALTIME, &beginTime);

        while (timeDelta < (speed * 2 * pow(2, (-1 * speed_level)))) {
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeDelta = 1000000000 * (currentTime.tv_sec - beginTime.tv_sec) + \
            (currentTime.tv_nsec - beginTime.tv_nsec);

            if (is_keyboard_pressed()) {
                char c;
                c = getchar();

                speed_control(c, &speed_level, &is_game_continue);
                print_field(field_matrix, field_width, field_height);
            }
        }

        generation++;
        next_genreation(field_matrix, buffer_matrix, field_width, field_height);

        for (int i = 0; i < field_height; i++) {
            for (int j = 0; j < field_width; j++) {
                if (field_matrix[i][j] != ' ') {
                    is_alive++;
                }
            }
        }

        if (is_alive == 0) {
            is_game_continue = 3;
        }

        print_field(field_matrix, field_width, field_height);
    }

    print_titres(is_game_continue, generation);

    return 0;
}

void clear_screen(void) {
    printf("\033[2J");
    printf("\033[0;0f");
}

void fill_field(char field_matrix[][80], int field_width, int field_height) {
    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
           field_matrix[i][j] = ' ';
        }
    }
}

void input_field(char field_matrix[][80], int field_width, int field_height) {
    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            int x, y;

            scanf("%d%d", &x, &y);
            field_matrix[x][y] = '*';
        }
    }
}

void print_field(char field_matrix[][80], int field_width, int field_height) {
    clear_screen();

    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            printf("%c", field_matrix[i][j]);
        }

        printf("\n");
    }
}

void speed_control(char c, int *speed_level, int *is_game_continue) {
    if (c == 'a' && *speed_level != 6) {
        *speed_level = *speed_level + 1;
    } else if (c == 'z' && *speed_level != 1) {
        *speed_level = *speed_level - 1;
    } else if (c == 'q') {
        *is_game_continue = 2;
    }
}

void next_genreation(char field_matrix[][80], char buffer_matrix[][80], int field_width, int field_height) {
    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            buffer_matrix[i][j] = field_matrix[i][j];
        }
    }

    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            int counter = count_neighbor(buffer_matrix, i, j);

            if (buffer_matrix[i][j] == ' ' && counter == 3) {
                field_matrix[i][j] = '*';
            }

            if (counter > 3 || counter < 2) {
                field_matrix[i][j] = ' ';
            }
        }
    }
}

int count_neighbor(char buffer_matrix[][80], int column, int row) {
    int counter = 0;
    int x, y;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (column + i < 0) {
                x = 24;
            } else if (column + i == 25) {
                x = 0;
            } else {
                x = column + i;
            }

            if (row + j < 0) {
                y = 79;
            } else if (row + j == 80) {
                y = 0;
            } else {
                y = row + j;
            }

            if (buffer_matrix[x][y] == '*') {
                counter++;
            }
        }
    }

    if (buffer_matrix[column][row] == '*') {
        counter--;
    }

    return counter;
}

void print_titres(int is_game_continue, int generation) {
    if (is_game_continue == 2) {
        char space = ' ';
        printf("%30cYOU ARE QUIT THE GAME\n", space);
    }

    if (is_game_continue == 3) {
        char space = ' ';
        printf("%30cGAME OVER (NUM OF GENERATION - %d)\n", space, generation);
    }
}

int is_keyboard_pressed(void) {
    struct termios old_terminal, new_terminal;
    int result = 0;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &old_terminal);
    new_terminal = old_terminal;
    new_terminal.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        result = 1;
    }

    return result;
}
