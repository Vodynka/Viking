#include <stdio.h>

// Структура корабля
struct Ship {
    int length;
    int row;
    int col;
    int isHorizontal; // В C нет типа bool, используем int (1 - горизонтально, 0 - вертикально)
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
    printBoard(playerBoard, "Pole vraga (kyda ctrelyaesh on)");
    printBoard(enemyBoard, "Tvoe pole(kyda ctrelyat on)");

    // 1) Ход игрока
    printf("\nTvoi xod, Coldat! Vvedi cordi dlya vistrila! (stroka stolbec ot 1 do 20): ");
    if (scanf("%d %d", &rCoord, &cCoord) == 2) {
        // Переводим в индексы массива (от 0 до 19)
        int r = rCoord - 1;
        int c = cCoord - 1;

        if (r >= 0 && r < 20 && c >= 0 && c < 20) {
            if (playerBoard[r][c] == '#') {
                printf("Ect probitie!\n");
                playerBoard[r][c] = 'X';
            } else if (playerBoard[r][c] == '.') {
                printf("Pydom, brat!\n");
                playerBoard[r][c] = 'o';
            } else {
                printf("Molniya 2 paza b odno i tozhe mecto ne byet!\n");
            }
        } else {
            printf("Kyda po grazhdanke!?\n");
        }
    }

    // 2) Ход противника
    printf("\nEgo xod... Coordinaty, kyda on shmalnet! (stroka stolbec): ");
    if (scanf("%d %d", &rCoord, &cCoord) == 2) {
        int r = rCoord - 1;
        int c = cCoord - 1;

        if (r >= 0 && r < 20 && c >= 0 && c < 20) {
            // Для примера отметим выстрел противника на вашем поле
            if (enemyBoard[r][c] == '.') {
                enemyBoard[r][c] = 'X'; // Пусть противник всегда попадает или отмечает выстрел
                printf("OH CTPELAL Tyda! [%d, %d] Metka!\n", rCoord, cCoord);
            }
        }
    }

    // Финальный вывод обновленных полей
 
    printBoard(playerBoard, "Pole vraga (posle tvoego xoda)");
    printBoard(enemyBoard, "Tvoe pole (posle xoda protivnika)");

    return 0;
}