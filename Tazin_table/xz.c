#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>

#define SIZE 10
#define TOTAL_SHIP_CELLS 20 /* Всего палуб во флоте (4 + 3*2 + 2*3 + 1*4 = 20) */

typedef struct {
    int length;
    int row;          /* 0..9 (соответствует строкам 1..10) */
    int col;          /* 0..9 (соответствует столбцам A..J) */
    int isHorizontal; /* 1 - горизонтально, 0 - вертикально */
    int hits;
} Ship;

void initBoard(char board[SIZE][SIZE], char fillChar) {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            board[i][j] = fillChar;
        }
    }
}

void placeShips(char board[SIZE][SIZE], Ship ships[], int shipCount) {
    int s, k;
    for (s = 0; s < shipCount; s++) {
        ships[s].hits = 0;
        for (k = 0; k < ships[s].length; k++) {
            int r = ships[s].row;
            int c = ships[s].col;
            if (ships[s].isHorizontal) {
                if (r >= 0 && r < SIZE && c + k >= 0 && c + k < SIZE) {
                    board[r][c + k] = '#';
                }
            } else {
                if (r + k >= 0 && r + k < SIZE && c >= 0 && c < SIZE) {
                    board[r + k][c] = '#';
                }
            }
        }
    }
}

void printBoard(const char board[SIZE][SIZE], const char *title) {
    int i, j;
    printf("\n=== %s ===\n", title);
    printf("     ");
    for (j = 0; j < SIZE; j++) {
        printf("%c  ", (char)('A' + j));
    }
    printf("\n");

    printf("    ");
    for (j = 0; j < SIZE * 3; j++) {
        printf("-");
    }
    printf("\n");

    for (i = 0; i < SIZE; i++) {
        int in = i + 1;
        if (in < 10) {
            printf(" %d |", in);
        } else {
            printf("%d |", in);
        }

        for (j = 0; j < SIZE; j++) {
            printf("%c  ", board[i][j]);
        }
        printf("\n");
    }
}

/* Автоматическая проверка выстрела (используется, когда 2-й игрок стреляет по 1-му) */
int processShot(int r, int c, char targetFleet[SIZE][SIZE], char shooterTracking[SIZE][SIZE], Ship targetShips[], int shipCount) {
    if (targetFleet[r][c] == '#') {
        int s, k;
        targetFleet[r][c] = 'X';
        shooterTracking[r][c] = 'X';

        for (s = 0; s < shipCount; s++) {
            for (k = 0; k < targetShips[s].length; k++) {
                int sr = targetShips[s].isHorizontal ? targetShips[s].row : targetShips[s].row + k;
                int sc = targetShips[s].isHorizontal ? targetShips[s].col + k : targetShips[s].col;
                if (sr == r && sc == c) {
                    targetShips[s].hits++;
                    if (targetShips[s].hits >= targetShips[s].length) {
                        return 2;
                    }
                    return 1;
                }
            }
        }
        return 1;
    } else {
        if (targetFleet[r][c] == '.') {
            targetFleet[r][c] = '*';
        }
        shooterTracking[r][c] = '*';
        return 0;
    }
}

int allShipsSunk(const Ship ships[], int shipCount) {
    int s;
    for (s = 0; s < shipCount; s++) {
        if (ships[s].hits < ships[s].length) {
            return 0;
        }
    }
    return 1;
}

/* Подсчет попаданий по 2-му игроку */
int countHitsOnBoard(const char board[SIZE][SIZE]) {
    int i, j, hits = 0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board[i][j] == 'X') {
                hits++;
            }
        }
    }
    return hits;
}

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void renderScreen(const char p1Shots[SIZE][SIZE], const char p1Fleet[SIZE][SIZE], const char *status) {
    system("cls");
    printf("Статус: %s\n", status);
    printBoard(p1Shots, "Мины (Места где ты уже подорвал мину)");
    printBoard(p1Fleet, "Субмарины (твои подлодки и места куда стрелял противник)");
}

/* Ход 1-го игрока с ручным вводом результата попадания */
void takeManualShotP1(char shooterTracking[SIZE][SIZE], char statusOut[256]) {
    int rInput = 0;
    char colChar = 0;
    int r, c, res;

    while (1) {
        printf("\nПервый, введи координаты выстрела: ");
        fflush(stdout);

        if (scanf("%d %c", &rInput, &colChar) != 2) {
            clearInputBuffer();
            printf("Введи от 1 до 10 ярдов по вертикали и от A до J по горизонтали, например 3 A. Давай ещё раз..\n");
            continue;
        }
        clearInputBuffer();

        colChar = (char)toupper((unsigned char)colChar);
        r = rInput - 1;
        c = colChar - 'A';

        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
            printf("Ты пытаешься подорвать мину за пределами моря.....\n");
            continue;
        }

        if (shooterTracking[r][c] != '.') {
            printf("Мина на (%d, %c) уже взорвана, давай другую!\n", rInput, colChar);
            continue;
        }

        /* Ручной выбор результата попадания */
        while (1) {
            printf("Ты подорвал подлодку второго юнги? в (%d, %c)? (0 - мимо, 1 - ранил, 2 - потопил): ", rInput, colChar);
            fflush(stdout);
            if (scanf("%d", &res) == 1 && (res == 0 || res == 1 || res == 2)) {
                clearInputBuffer();
                break;
            }
            clearInputBuffer();
            printf("Чего..?\n");
        }

        if (res == 2) {
            shooterTracking[r][c] = 'X';
            snprintf(statusOut, 256, "Юнга 1 взорвал мину в (%d, %c): Есть пробитие (потопил)!", rInput, colChar);
        } else if (res == 1) {
            shooterTracking[r][c] = 'X';
            snprintf(statusOut, 256, "Юнга 1 взорвал мину в (%d, %c): Не пробил, но поцарапал (ранил)!", rInput, colChar);
        } else {
            shooterTracking[r][c] = '*';
            snprintf(statusOut, 256, "Юнга 1 взорвал мину в (%d, %c): Пустышка (мимо).", rInput, colChar);
        }

        return;
    }
}

/* Ход 2-го игрока (автоматически проверяет попадание по флоту 1-го игрока) */
void takeAutoShotP2(
    char targetFleet[SIZE][SIZE],
    char shooterTracking[SIZE][SIZE],
    Ship targetShips[],
    int shipCount,
    char statusOut[256]
) {
    int rInput = 0;
    char colChar = 0;
    int r, c, res;

    while (1) {
        printf("\nВторой, взрывай уже с 1 до 10 и с A до J! ");
        fflush(stdout);

        if (scanf("%d %c", &rInput, &colChar) != 2) {
            clearInputBuffer();
            printf("Введи от 1 до 10 ярдов по вертикали и от A до J по горизонтали например 3 A. Давай ещё раз..\n");
            continue;
        }
        clearInputBuffer();

        colChar = (char)toupper((unsigned char)colChar);
        r = rInput - 1;
        c = colChar - 'A';

        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
            printf("Ты пытаешься подорвать мину за пределами моря.....\n");
            continue;
        }

        if (shooterTracking[r][c] != '.') {
            printf("Мина на (%d, %c) уже взорвана, давай другую!\n", rInput, colChar);
            continue;
        }

        res = processShot(r, c, targetFleet, shooterTracking, targetShips, shipCount);

        if (res == 2) {
            snprintf(statusOut, 256, "Юнга 2 взорвал мину в (%d, %c): Есть пробитие!", rInput, colChar);
        } else if (res == 1) {
            snprintf(statusOut, 256, "Юнга 2 взорвал мину в (%d, %c): Не пробил, но поцарапал!", rInput, colChar);
        } else {
            snprintf(statusOut, 256, "Юнга 2 взорвал мину в (%d, %c): Пустышка.", rInput, colChar);
        }

        return;
    }
}

int main(void) {
    char p1Fleet[SIZE][SIZE];
    char p1Shots[SIZE][SIZE];
    char p2Shots[SIZE][SIZE];

    Ship p1Ships[10] = {
        {4, 0, 0, 0, 0},
        {3, 0, 2, 1, 0},
        {3, 2, 4, 0, 0},
        {2, 6, 1, 1, 0},
        {2, 0, 7, 1, 0},
        {2, 3, 8, 0, 0},
        {1, 8, 8, 1, 0},
        {1, 9, 0, 1, 0},
        {1, 9, 4, 1, 0},
        {1, 6, 6, 1, 0}
    };
    int p1ShipCount = 10;

    char status[256] = "Субмарины и мины расставлены.";
    int currentPlayer = 0;

    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    initBoard(p1Fleet, '.');
    initBoard(p1Shots, '.');
    initBoard(p2Shots, '.');

    placeShips(p1Fleet, p1Ships, p1ShipCount);

    while (1) {
        renderScreen(p1Shots, p1Fleet, status);

        /* Проверка победы 1-го игрока (по количеству отмеченных попаданий) */
        if (countHitsOnBoard(p1Shots) >= TOTAL_SHIP_CELLS) {
            printf("\nВсе субмарины второго Юнги потоплены! Победа первого!\n\n"
                   "   Юнга 1   \n"
                   "  +--------+   Юнга 2   \n"
                   "  | 1место |  +--------+\n"
                   "  |        |  | 2место |\n"
                   "  +--------+  +--------+\n");
            break;
        }

        /* Проверка победы 2-го игрока */
        if (allShipsSunk(p1Ships, p1ShipCount)) {
            printf("\nВсе субмарины первого Юнги потоплены! Победа второго!\n\n"
                   "   Юнга 2   \n"
                   "  +--------+   Юнга 1   \n"
                   "  | 1место |  +--------+\n"
                   "  |        |  | 2место |\n"
                   "  +--------+  +--------+\n");
            break;
        }

        /* Выбор, кто делает текущий ход */
        while (1) {
            printf("\nКто сейчас подрывает? (1 - Первый Юнга, 2 - Второй Юнга): ");
            fflush(stdout);
            if (scanf("%d", &currentPlayer) == 1 && (currentPlayer == 1 || currentPlayer == 2)) {
                clearInputBuffer();
                break;
            }
            clearInputBuffer();
            printf("Запиши 1 или 2!\n");
        }

        if (currentPlayer == 1) {
            takeManualShotP1(p1Shots, status);
        } else {
            takeAutoShotP2(p1Fleet, p2Shots, p1Ships, p1ShipCount, status);
        }
    }

    return 0;
}
