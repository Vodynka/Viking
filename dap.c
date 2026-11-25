#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>

#define SIZE 10

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

/* Возвращает:
 *  0 - мимо
 *  1 - попал, но корабль ещё плавает
 *  2 - корабль потоплен
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
    printBoard(p1Shots, "Мины (Места где ты уже подорвал мину)");
    printBoard(p1Fleet, "Субмарины (твои подлодки и места куда стрелял противник)");
}

void takeSingleShot(
    int playerNum,
    const char *promptText,
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
        printf("\n%s", promptText);
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
            printf("ты пытаешься подорвать мину за пределами моря.....\n");
            continue;
        }

        if (shooterTracking[r][c] != '.') {
            printf("Мина на (%d, %c) уже взорвана, давай дрругую\n", rInput, colChar);
            continue;
        }

        res = processShot(r, c, targetFleet, shooterTracking, targetShips, shipCount);

        if (res == 2) {
            snprintf(statusOut, 256, "Юнга %d взорвал мину в (%d, %c): Есть пробитие!", playerNum, rInput, colChar);
        } else if (res == 1) {
            snprintf(statusOut, 256, "Юнга %d взорвал мину в (%d, %c): не пробил но поцарапал!", playerNum, rInput, colChar);
        } else {
            snprintf(statusOut, 256, "Юнга %d взорвал мину в (%d, %c): Пустышка.", playerNum, rInput, colChar);
        }

        return;
    }
}

int main(void) {
    char p1Fleet[SIZE][SIZE];
    char p1Shots[SIZE][SIZE];
    char p2Fleet[SIZE][SIZE];
    char p2Shots[SIZE][SIZE];

    Ship p1Ships[] = {
        {4, 0, 0, 0, 0},   /* 1A..4A */
        {3, 2, 4, 0, 0},   /* 3E..5E */
        {2, 6, 1, 1, 0},   /* 7B..7C */
        {1, 8, 8, 1, 0}    /* 9I */
    };
    int p1ShipCount = 4;

    Ship p2Ships[] = {
        {4, 1, 5, 1, 0},   /* 2F..2I */
        {3, 4, 2, 0, 0},   /* 5C..7C */
        {2, 7, 6, 1, 0},   /* 8G..8H */
        {1, 9, 0, 1, 0}    /* 10A */
    };
    int p2ShipCount = 4;

    char status[256] = "Субмарины и мины расставлены.";

    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    initBoard(p1Fleet, '.');
    initBoard(p1Shots, '.');
    initBoard(p2Fleet, '.');
    initBoard(p2Shots, '.');

    placeShips(p1Fleet, p1Ships, p1ShipCount);
    placeShips(p2Fleet, p2Ships, p2ShipCount);

    while (1) {
        renderScreen(p1Shots, p1Fleet, status);
        if (allShipsSunk(p2Ships, p2ShipCount)) {
        printf("Все субмарины первого Юнги потоплены! Победа второго!\n\n"
           "   Юнга 1   \n"
           "  +--------+   Юнга 2   \n"
           "  | 1место |  +--------+\n"
           "  |        |  | 2место |\n"
           "  +--------+  +--------+\n");            break;
        }
        if (allShipsSunk(p1Ships, p1ShipCount)) {
         printf("Все субмарины первого Юнги потоплены! Победа второго!\n\n"
           "   Юнга 1   \n"
           "  +--------+   Юнга 2   \n"
           "  | 1место |  +--------+\n"
           "  |        |  | 2место |\n"
           "  +--------+  +--------+\n");
            break;
        }

        takeSingleShot(
            1,
            "Первый, взрывай уже с 1 да 10 и с A до J! ",
            p2Fleet,
            p1Shots,
            p2Ships,
            p2ShipCount,
            status
        );

        renderScreen(p1Shots, p1Fleet, status);
        if (allShipsSunk(p2Ships, p2ShipCount)) {
        printf("Все субмарины первого Юнги потоплены! Победа второго!\n\n"
           "   Юнга 1   \n"
           "  +--------+   Юнга 2   \n"
           "  | 1место |  +--------+\n"
           "  |        |  | 2место |\n"
           "  +--------+  +--------+\n");
           break;
        }

        takeSingleShot(
            2,
            "Второй, взрывай уже с 1 да 10 и с A до J! ",
            p1Fleet,
            p2Shots,
            p1Ships,
            p1ShipCount,
            status
        );

        renderScreen(p1Shots, p1Fleet, status);
        if (allShipsSunk(p1Ships, p1ShipCount)) {
             printf("Все субмарины первого Юнги потоплены! Победа второго!\n\n"
           "   Юнга 2   \n"
           "  +--------+   Юнга 1   \n"
           "  | 1место |  +--------+\n"
           "  |        |  | 2место |\n"
           "  +--------+  +--------+\n");
            break;
        }
    }

    system("pause");
    return 0;
}