#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SIZE 10
#define SHIP_COUNT 10 /* 1x4, 2x3, 3x2, 4x1 (всего 10 кораблей) */

typedef struct {
    int length;
    int row;
    int col;
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

/* Проверяет, можно ли поставить корабль без выхода за границы и касания других кораблей */
int canPlaceShip(const char board[SIZE][SIZE], int r, int c, int length, int isHorizontal) {
    int k, dr, dc;
    for (k = 0; k < length; k++) {
        int curR = isHorizontal ? r : r + k;
        int curC = isHorizontal ? c + k : c;

        if (curR < 0 || curR >= SIZE || curC < 0 || curC >= SIZE) {
            return 0;
        }

        /* Проверка самой клетки и всех 8 соседних вокруг неё */
        for (dr = -1; dr <= 1; dr++) {
            for (dc = -1; dc <= 1; dc++) {
                int nr = curR + dr;
                int nc = curC + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    if (board[nr][nc] != '.') {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/* Случайно расставляет флот (1x4, 2x3, 3x2, 4x1) и заполняет массив Ship */
void generateRandomFleet(char board[SIZE][SIZE], Ship ships[], int shipCount) {
    int lengths[SHIP_COUNT] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int s, k;

    for (s = 0; s < shipCount; s++) {
        int len = lengths[s];
        int placed = 0;

        while (!placed) {
            int r = rand() % SIZE;
            int c = rand() % SIZE;
            int isHorizontal = rand() % 2;

            if (canPlaceShip(board, r, c, len, isHorizontal)) {
                ships[s].length = len;
                ships[s].row = r;
                ships[s].col = c;
                ships[s].isHorizontal = isHorizontal;
                ships[s].hits = 0;

                for (k = 0; k < len; k++) {
                    if (isHorizontal) {
                        board[r][c + k] = '#';
                    } else {
                        board[r + k][c] = '#';
                    }
                }
                placed = 1;
            }
        }
    }
}

void printBoard(const char board[SIZE][SIZE], const char *title) {
    int i, j;
    printf("\n=== %s ===\n", title);
    printf("   ");
    for (j = 0; j < SIZE; j++) {
        int jn = j + 1;
        if (jn < 10) {
            printf("%d  ", jn);
        } else {
            printf("%d ", jn);
        }
    }
    printf("\n");

    printf("   ");
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

/* Возвращает:
 * 0 - мимо
 * 1 - попал, но корабль ещё плавает
 * 2 - корабль потоплен
 */
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

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void renderScreen(const char p1Shots[SIZE][SIZE], const char p1Fleet[SIZE][SIZE], const char *status) {
    system("cls");
    printf("Статус: %s\n", status);
    printBoard(p1Shots, "ПОЛЕ 1: ВЫСТРЕЛЫ ИГРОКА 1 (. - не стрелял, * - мимо, X - попал)");
    printBoard(p1Fleet, "ПОЛЕ 2: КОРАБЛИ ИГРОКА 1 И ВЫСТРЕЛЫ ИГРОКА 2 (# - корабль, X - попал 2-й игрок, * - мимо)");
}

/* Ждёт ввода двух координат (1..10) и обрабатывает один выстрел */
int takeSingleShot(
    int playerNum,
    const char *promptText,
    char targetFleet[SIZE][SIZE],
    char shooterTracking[SIZE][SIZE],
    Ship targetShips[],
    int shipCount,
    char statusOut[256]
) {
    int rInput = 0, cInput = 0;
    int r, c, res;

    while (1) {
        printf("\n%s", promptText);
        fflush(stdout);

        if (scanf("%d %d", &rInput, &cInput) != 2) {
            clearInputBuffer();
            printf("Нужно ввести два целых числа через пробел (например: 3 5). Попробуй ещё раз.\n");
            continue;
        }
        clearInputBuffer();

        r = rInput - 1;
        c = cInput - 1;

        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
            printf("Координаты должны быть от 1 до 10. Попробуй ещё раз.\n");
            continue;
        }

        if (shooterTracking[r][c] != '.') {
            printf("В клетку (%d, %d) уже стреляли. Выбери другую.\n", rInput, cInput);
            continue;
        }

        res = processShot(r, c, targetFleet, shooterTracking, targetShips, shipCount);
        if (res == 2) {
            snprintf(statusOut, 256, "Игрок %d выстрелил в (%d, %d): Убит! Корабль потоплен!", playerNum, rInput, cInput);
        } else if (res == 1) {
            snprintf(statusOut, 256, "Игрок %d выстрелил в (%d, %d): Есть пробитие!", playerNum, rInput, cInput);
        } else {
            snprintf(statusOut, 256, "Игрок %d выстрелил в (%d, %d): Промах.", playerNum, rInput, cInput);
        }
        return 1;
    }
}

int main(void) {
    char p1Fleet[SIZE][SIZE]; /* Поле 2: корабли Игрока 1 и выстрелы Игрока 2 */
    char p1Shots[SIZE][SIZE]; /* Поле 1: куда стрелял Игрок 1 */
    char p2Fleet[SIZE][SIZE]; /* Корабли Игрока 2 */
    char p2Shots[SIZE][SIZE]; /* Выстрелы Игрока 2 */

    Ship p1Ships[SHIP_COUNT];
    Ship p2Ships[SHIP_COUNT];

    char status[256] = "Игра началась. Сначала стреляет Игрок 1, затем Игрок 2.";

    srand((unsigned int)time(NULL)); /* Инициализация генератора случайных чисел */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    initBoard(p1Fleet, '.');
    initBoard(p1Shots, '.');
    initBoard(p2Fleet, '.');
    initBoard(p2Shots, '.');

    /* Случайная генерация флота (1x4, 2x3, 3x2, 4x1) для обоих игроков */
    generateRandomFleet(p1Fleet, p1Ships, SHIP_COUNT);
    generateRandomFleet(p2Fleet, p2Ships, SHIP_COUNT);

    while (1) {
        /* 1. Ход Игрока 1 -> сразу обновление обоих полей */
        renderScreen(p1Shots, p1Fleet, status);

        if (allShipsSunk(p2Ships, SHIP_COUNT)) {
            printf("\nВсе корабли Игрока 2 потоплены! Победа!\n");
            break;
        }
        if (allShipsSunk(p1Ships, SHIP_COUNT)) {
            printf("\nВсе корабли Игрока 1 потоплены!\n");
            break;
        }

        takeSingleShot(
            1,
            "Ход Игрока 1 — введи строку и столбец (1..10, например: 3 5): ",
            p2Fleet,
            p1Shots,
            p2Ships,
            SHIP_COUNT,
            status
        );

        renderScreen(p1Shots, p1Fleet, status);
        if (allShipsSunk(p2Ships, SHIP_COUNT)) {
            printf("\nВсе корабли Игрока 2 потоплены! Победа!\n");
            break;
        }

        /* 2. Ход Игрока 2 -> сразу обновление обоих полей */
        takeSingleShot(
            2,
            "Ход Игрока 2 — введи строку и столбец (1..10, например: 2 2): ",
            p1Fleet,
            p2Shots,
            p1Ships,
            SHIP_COUNT,
            status
        );

        renderScreen(p1Shots, p1Fleet, status);
        if (allShipsSunk(p1Ships, SHIP_COUNT)) {
            printf("\nВсе корабли Игрока 1 потоплены!\n");
            break;
        }
    }

    system("pause");
    return 0;
}
