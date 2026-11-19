#include <iostream>
#include <vector>

using namespace std;

// Структура корабля
struct Ship {
    int length;
    int row;
    int col;
    bool isHorizontal;
    int hits; // Сколько палуб уже подбили
};

const int SIZE = 10;

// Функция для отрисовки поля (скрываем корабли, показываем только выстрелы)
void drawBoard(const vector<vector<char>>& board) {
    cout << "\n 0 1 2 3 4 5 6 7 8 9" << endl;
    for (int i = 0; i < SIZE; ++i) {
        cout << i << " ";
        for (int j = 0; j < SIZE; ++j) {
            char cell = board[i][j];
            // Если там корабль ('O'), скрываем его от игрока, показывая воду ('~')
            if (cell == 'O') {
                cout << "~ ";
            } else {
                cout << cell << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    // Настраиваем русскую локаль для консоли
    setlocale(LC_ALL, "Russian");

    // Инициализируем игровое поле: '~' — вода, 'O' — корабль
    vector<vector<char>> board(SIZE, vector<char>(SIZE, '~'));

    // Создаем корабли
    vector<Ship> ships = {
        {3, 1, 1, true, 0}, // Трехпалубный (строка 1, столбцы 1-3)
        {2, 4, 6, false, 0}, // Двухпалубный (строки 4-5, столбец 6)
        {1, 8, 2, true, 0} // Однопалубный (строка 8, столбец 2)
    };

    // Расставляем корабли на поле
    for (const auto& ship : ships) {
        for (int i = 0; i < ship.length; ++i) {
            int r = ship.row;
            int c = ship.col;
            if (ship.isHorizontal) c += i;
            else r += i;
            board[r][c] = 'O';
        }
    }

    int totalShips = ships.size();
    int sunkShips = 0;

    cout << "=== МОРСКОЙ БОЙ ===" << endl;
    cout << "Стреляй по координатам (строка и столбец от 0 до 9)." << endl;

    while (sunkShips < totalShips) {
        drawBoard(board);

        int r, c;
        cout << "Введите строку (0-9): ";
        if (!(cin >> r)) break;
        cout << "Введите столбец (0-9): ";
        if (!(cin >> c)) break;

        // Проверка на корректность ввода
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
            cout << "❌ Ошибка: выход за границы поля! Введите числа от 0 до 9.\n";
            continue;
        }

        // Проверяем, куда попал игрок
        if (board[r][c] == 'O') {
            cout << "🎯 Попил! (Ранил)\n";
            board[r][c] = 'X'; // Отмечаем попадание

            // Проверяем, какой именно корабль задели и потоплен ли он целиком
            for (auto& ship : ships) {
                // Проверяем, принадлежит ли эта точка конкретному кораблю
                bool belongsToShip = false;
                for (int i = 0; i < ship.length; ++i) {
                    int sr = ship.row;
                    int sc = ship.col;
                    if (ship.isHorizontal) sc += i;
                    else sr += i;

                    if (sr == r && sc == c) {
                        belongsToShip = true;
                        break;
                    }
                }

                if (belongsToShip) {
                    ship.hits++;
                    if (ship.hits == ship.length) {
                        cout << "🔥 Корабль потоплен!\n";
                        sunkShips++;
                    }
                }
            }

        } else if (board[r][c] == '~') {
            cout << "🌊 Мимо!\n";
            board[r][c] = '.'; // Отмечаем промах
        } else {
            cout << "⚠️ Вы уже стреляли в эту точку!\n";
        }

        if (sunkShips == totalShips) {
            cout << "\n🏆 Победа! Все корабли противника потоплены!\n";
            break;
        }
    }

    return 0;
}
