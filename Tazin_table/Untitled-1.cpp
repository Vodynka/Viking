#include <stdio.h>

// Структура корабля
struct Ship {
    int length;
    int row;
    int col;
    int isHorizontal; // используем int (1 - горизонтально, 0 - вертикально)
};

// Функция для отрисовки одного игрового поля
void printBoard(char board[20][20], const char* title) {
    printf("\n=== %s ===\n", title);
    
    // Вывод шапки с номерами столбцов
    printf("     ");
    for (int j = 0; j < 20; j++) {
        int jn = j + 1;
        if (jn < 10) {
            printf("%d  ", jn);
        } else {
            printf("%d ", jn);
        }
    }
    printf("\n");

    // Вывод разделительной линии
    printf("    ");
    for (int j = 0; j < 60; j++) {
        printf("-");
    }
    printf("\n");

    // Вывод строк поля с номерами
    for (int i = 0; i < 20; i++) {
        int in = i + 1;
        if (in < 10) {
            printf(" %d |", in);
        } else {
            printf("%d |", in);
        }

        for (int j = 0; j < 20; j++) {
            printf("%c  ", board[i][j]);
        }
        printf("\n");
    }
}

int main() {
    // Два поля: 
    // playerBoard — поле противника, куда стреляете вы (будем отмечать попадания/промахи)
    // enemyBoard — ваше поле, куда стреляет противник (здесь стоят ваши корабли)
    char playerBoard[20][20];
    char enemyBoard[20][20];

    // Инициализация обоих полей пустыми точками
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            playerBoard[i][j] = '.';
            enemyBoard[i][j] = '.';
        }
    }

    // Ваши корабли на поле противника (для демонстрации)
    struct Ship ships[] = {
        {4, 1, 1, 1},   // Четырёхпалубный
        {3, 3, 5, 0},   // Трёхпалубный
        {2, 7, 2, 1},   // Двухпалубный
        {1, 9, 9, 1}    // Однопалубный
    };
    int shipCount = 4;

    // Размещение кораблей на поле противника
    for (int s = 0; s < shipCount; s++) {
        for (int k = 0; k < ships[s].length; k++) {
            int r = ships[s].row;
            int c = ships[s].col;
            if (ships[s].isHorizontal) {
                if (c + k < 20 && r < 20) {
                    playerBoard[r][c + k] = '#'; // Показываем корабль (в реальной игре можно скрывать)
                }
            } else {
                if (r + k < 20 && c < 20) {
                    playerBoard[r + k][c] = '#';
                }
            }
        }
    }

    int rCoord, cCoord;

    // Игровой цикл (пример на 1 ход)
    printBoard(playerBoard, "Vrashina");
    printBoard(enemyBoard, "Bogatir Batkovich");

    // 1) Ход игрока
    printf("\nTvoa ochered strelat s lyky ali ne zabyd cho vrag v pole 20 na 20 metrov: ");
    if (scanf("%d %d", &rCoord, &cCoord) == 2) {
        // Переводим в индексы массива (от 0 до 19)
        int r = rCoord - 1;
        int c = cCoord - 1;

        if (r >= 0 && r < 20 && c >= 0 && c < 20) {
            if (playerBoard[r][c] == '#') {
                printf("Probil ti cherep emy strelou!\n");
                playerBoard[r][c] = 'X';
            } else if (playerBoard[r][c] == '.') {
                printf("yvernylsa eretik!\n");
                playerBoard[r][c] = 'o';
            } else {
                printf("ox bratec ti yshe strelal v etot kyst!\n");
            }
        } else {
            printf("Basyrmane bi ne yspeli ybeshat v les strelau v pole!\n");
        }
    }

    // 2) Ход противника
    printf("\nBasurmane napadayt, akyratnee oni strelayt po xolmy 20 na 20 metrov: ");
    if (scanf("%d %d", &rCoord, &cCoord) == 2) {
        int r = rCoord - 1;
        int c = cCoord - 1;

        if (r >= 0 && r < 20 && c >= 0 && c < 20) {
            // Для примера отметим выстрел противника на вашем поле
            if (enemyBoard[r][c] == '.') {
                enemyBoard[r][c] = 'X'; // Пусть противник всегда попадает или отмечает выстрел
                printf("Basyrmanin metnyl kopie v [%d, %d] no pometil ego kraskoy\n", rCoord, cCoord);
            }
        }
    }

    // Финальный вывод обновленных полей
    printBoard(playerBoard, "Polushko");
    printBoard(enemyBoard, "Xolm");

    return 0;
}