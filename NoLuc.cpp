#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <limits>

const int BOARD_SIZE = 3;

using namespace std;

void changeTextColor(int colorCode) {
    cout << "\033[" << colorCode << "m";
}

void delay(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

class Board {
public:
    vector<vector<char>> grid;

    Board() {
        grid.assign(BOARD_SIZE, vector<char>(BOARD_SIZE, '.'));
        char num = '1';
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                grid[i][j] = num++;
            }
        }
    }

    void print() const {
        changeTextColor(35);
        cout << "\n\n";
        cout << "\t\t\t\t\t\t\t+-----------------------------------------------+\n";
        for (const auto& row : grid) {
            cout << "\t\t\t\t\t\t\t|";
            for (char cell : row) {
                cout << "\t \t";
                cout << "|";
            }
            cout << "\n\t\t\t\t\t\t\t|";
            for (char cell : row) {
                if (cell == 'X' ) {
                    changeTextColor(32);
                } else if (cell == 'O' ) {
                    changeTextColor(31);
                } else {
                    changeTextColor(35);
                }
                cout << "\t" << cell << "\t";
                changeTextColor(35);
                cout << "|";
            }
            cout << "\n\t\t\t\t\t\t\t|";
            for (char cell : row) {
                cout << "\t \t";
                cout << "|";
            }
            cout << "\n\t\t\t\t\t\t\t+-----------------------------------------------+\n";
        }
        changeTextColor(0);
        cout << "\n\n";
    }

    bool isCellEmpty(int num) const {
        char cellChar = num + '0';
        for (const auto& row : grid) {
            for (char cell : row) {
                if (cell == cellChar)
                    return true;
            }
        }
        return false;
    }

    void markCell(int num, char playerChar) {
        char cellChar = num + '0';
        for (auto& row : grid) {
            for (char& cell : row) {
                if (cell == cellChar)
                    cell = playerChar;
            }
        }
    }

    char checkWinner() const {
        char w = 'c';
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]) {
                if (grid[i][0] != '.') return grid[i][0];
            }
            if (grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i]) {
                if (grid[0][i] != '.') return grid[0][i];
            }
        }
        if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) {
            if (grid[0][0] != '.') return grid[0][0];
        }
        if (grid[2][0] == grid[1][1] && grid[1][1] == grid[0][2]) {
            if (grid[2][0] != '.') return grid[2][0];
        }
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] >= '1' && grid[i][j] <= '9')
                    return 'c';
            }
        }
        return 'd';
    }
};

class Player {
public:
    char marker;
    string name;

    Player(char mark, const string& playerName) : marker(mark), name(playerName) {}

    virtual int makeMove(const Board& board) const = 0;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char mark, const string& playerName) : Player(mark, playerName) {}

    int makeMove(const Board& board) const override {
        int cell;
        do {
            cout << "\t\t\t\t\t " << name << ", chon so cua o ma ban muon choi: ";
            cin >> cell;
            if (!cin) {
                cout << "\t\t\t\t\t Loi: Du lieu nhap khong hop le. Xin hay nhap lai.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (!board.isCellEmpty(cell)) {
                cout << "\t\t\t\t\t O nay da duoc chon, vui long chon o khac.\n";
            }
        } while (!board.isCellEmpty(cell));
        return cell;
    }

};

class ComputerPlayer : public Player {
public:
    string difficulty;

    ComputerPlayer(char mark, const string& aiDifficulty = "Kho") : Player(mark, "May"), difficulty(aiDifficulty) {}

    int makeMove(const Board& board) const override {
        int cell;
        if (difficulty == "k") {
            cell = minimax(board, marker).second;
        } else if (difficulty == "d") {
            cell = randomCell(board);
        }
        return cell;
    }

private:
    pair<int, int> minimax(const Board& board, char player) const {
        char opponent = (player == 'X') ? 'O' : 'X';

        char winner = board.checkWinner();
        if (winner == player) {
            return {1, -1};
        } else if (winner == opponent) {
            return {-1, -1};
        } else if (winner == 'd') {
            return {0, -1};
        }

        vector<int> emptyCells;
        for (int i = 1; i <= 9; ++i) {
            if (board.isCellEmpty(i)) {
                emptyCells.push_back(i);
            }
        }

        pair<int, int> bestMove = {-2, -1};
        for (int cell : emptyCells) {
            Board newBoard = board;
            newBoard.markCell(cell, player);
            pair<int, int> score = minimax(newBoard, opponent);
            score.first = -score.first;
            if (score.first >= bestMove.first) {
                bestMove = {score.first, cell};
            }
        }

        return bestMove;
    }

    int randomCell(const Board& board) const {
        vector<int> emptyCells;
        for (int i = 1; i <= 9; ++i) {
            if (board.isCellEmpty(i)) {
                emptyCells.push_back(i);
            }
        }
        if (emptyCells.empty()) {
            return -1; // No empty cell
        }
        int randomIndex = rand() % emptyCells.size();
        return emptyCells[randomIndex];
    }
};




class Game {
private:
    Board board;
    Board initialBoard;
    Player* player1;
    Player* player2;
    int player1Wins;
    int player2Wins;
    bool continueWithSameOpponent;

public:
    Game(Player* p1, Player* p2) : player1(p1), player2(p2), player1Wins(0), player2Wins(0), continueWithSameOpponent(false) {}

    void updateWins(char winner) {
        if (winner == player1->marker) {
            player1Wins++;
        } else if (winner == player2->marker) {
            player2Wins++;
        }
    }

    void resetBoard() {
        board = initialBoard;
    }


    void printScore() const {
        if (player1->name == "May") {
            cout << "\t\t\t\t\tMay 1: " << player1Wins << " \n";
        } else {
            cout << "\t\t\t\t\t" << player1->name << ": " << player1Wins << " \n";
        }

        if (player2->name == "May") {
            cout << "\t\t\t\t\tMay 2: " << player2Wins << " \n";
        } else {
            cout << "\t\t\t\t\t" << player2->name << ": " << player2Wins << " \n";
        }
    }


    void playGame() {
    system("cls");
    printScore();
    board.print();

    bool turn = true;
    while (board.checkWinner() == 'c') {
        int move;
        if (turn) {
            move = player1->makeMove(board);
        } else {
            move = player2->makeMove(board);
        }
        char currentPlayerMark = (turn ? player1->marker : player2->marker);
        board.markCell(move, currentPlayerMark);
        system("cls");
        printScore();
        board.print();
        delay(1000);
        turn = !turn;
    }

    delay(1000);
    char winner = board.checkWinner();
    changeTextColor(34);
    if (winner == player1->marker) {
        if (player1->name == "May" && player2->name == "May") {
            cout << "\t\t\t\t\t************* May 1 Chien Thang! *************\n";
        } else {
            cout << "\t\t\t\t\t************* Xin chuc mung chien thang.. " << player1->name << " Wins! *************\n";
        }
    } else if (winner == player2->marker) {
        if (player1->name == "May" && player2->name == "May") {
            cout << "\t\t\t\t\t************* May 2 Chien Thang********\n";
        } else {
            cout << "\t\t\t\t\t************* Xin chuc mung chien thang.. " << player2->name << " Wins! *************\n";
        }
    } else {
        cout << "\t\t\t\t\t*************************** Cac ban hoa nhau! ***************************\n";
    }
    delay(2000);
    system("cls");
    updateWins(winner);
    continueWithSameOpponent = shouldResetGame();
    system("cls");
    if (continueWithSameOpponent) {
        resetBoard();
    }
}


    bool shouldResetGame() {
        char choice;
        cout << "\t\t\t\t\t\tBan co muon choi tiep khong? ";
        cout << "\t\t\t\t\t\t\t\t\t\tYes Or No (y/n): ";
        cin >> choice;
        return (choice == 'y');
    }

    bool shouldContinue() const {
        return continueWithSameOpponent;
    }

};

string chooseDifficulty() {
    string difficulty;
    cout << "\t\t\t\t\t\tChon may de hay kho(d/k): ";
    cin >> difficulty;
    while (difficulty != "d" && difficulty != "k") {
        cout << "\t\t\t\t\t\tLua chon khong hop le. Chon do kho cua may (d/k): ";
        cin >> difficulty;
    }
    return difficulty;
}

char getPlayerMarker(const string& playerName) {
    char marker;
    cout << "\t\t\t\t\t\tChon X hoac O, " << playerName << ": ";
    cin >> marker;
    while (marker != 'X'  && marker != 'O' ) {
        cout << "\t\t\t\t\t\tLua chon khong hop le. Chon X hoac O: ";
        cin >> marker;
    }
    return marker;
}

void printTitle(){
    changeTextColor(33);
    cout << "\t\t\t..........             ...........                    ..........." << endl;
    cout << "\t\t\t.... ......             .... ......                    .... ......" << endl;
    cout << "\t\t\t   : :  ...  ......        : :     ...     ......         : :  .......  ......" << endl;
    cout << "\t\t\t   : :  : :  : ...:        : :    : . :    : ...:         : :  : :.: :  :  ...   " << endl;
    cout << "\t\t\t   : :  : :  : :...        : :   : :.: :   : :...         : :  : :.: :  :  :::" << endl;
    cout << "\t\t\t   :.:  :.:  :.....:       :.:  :  : :  :  :.....:        : :  :.....:  :.....      " << endl;
}

int main() {
    srand(time(0));
    system("Color 4");


    int choice;
    do {
        printTitle();
        changeTextColor(35);

        cout << "\t\t\t\t\t\t-------------------------------------\n";
        cout << "\t\t\t\t\t\t\t1 -> Nguoi Vs Nguoi\n";
        cout << "\t\t\t\t\t\t\t2 -> Nguoi Vs May\n";
        cout << "\t\t\t\t\t\t\t3 -> May Vs May\n";
        cout << "\t\t\t\t\t\t\t4 -> Thoat\n";
        cout << "\t\t\t\t\t\t-------------------------------------\n";
        changeTextColor(0);
        cout << "\t\t\t\t\t\t\tNhap su lua chon: ";
        cin >> choice;

        string player1Name, player2Name;
        char player1Marker;

        if (choice == 1) {
            cout << "\t\t\t\t\t\tNhap ten nguoi choi 1: ";
            cin >> player1Name;
            player1Marker = getPlayerMarker(player1Name);
            cout << "\t\t\t\t\t\tNhap ten nguoi choi 2: ";
            cin >> player2Name;

            HumanPlayer player1(player1Marker, player1Name);
            HumanPlayer player2(player1Marker == 'X' ? 'O' : 'X', player2Name);
            Game game(&player1, &player2);
            do {
                game.playGame();
            } while (game.shouldContinue());
        } else if (choice == 2) {
            cout << "\t\t\t\t\t\tNhap ten nguoi choi: ";
            cin >> player1Name;
            player1Marker = getPlayerMarker(player1Name);

            auto aiDifficulty = chooseDifficulty();
            string player2Name = "May ";

            if (aiDifficulty == "d") {
                player2Name += "De";
            } else {
                player2Name += "Kho";
            }

            HumanPlayer player1(player1Marker, player1Name);
            ComputerPlayer player2(player1Marker == 'X' ? 'O' : 'X', aiDifficulty);
            player2.name = player2Name;
            Game game(&player1, &player2);
            do {
                game.playGame();
            } while (game.shouldContinue());

        } else if (choice == 3) {
            auto aiDifficulty1 = "d";
            auto aiDifficulty2 = "k";

            ComputerPlayer player1('X', aiDifficulty1);
            ComputerPlayer player2('O', aiDifficulty2);
            Game game(&player1, &player2);
            do {
                game.playGame();
            } while (game.shouldContinue());
        } else if (choice == 4) {
            cout << "\t\t\t\t\tThoat khoi tro choi...\n";
        } else {
            cout << "\t\t\t\t\tLoi: Lua chon khong hop le!\n";
        }

    } while (choice != 4);

    return 0;
}
