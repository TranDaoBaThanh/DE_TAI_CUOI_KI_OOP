#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
using namespace std;

class Board {
private:
    vector<vector<char>> board = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
public:
    void draw() const {
        cout << "-------------" << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "| ";
            for (int j = 0; j < 3; ++j) {
                cout << board[i][j] << " | ";
            }
            cout << endl << "-------------" << endl;
        }
    }

    bool placeMarker(int position, char marker) {
        if (position < 1 || position > 9 || board[(position - 1) / 3][(position - 1) % 3] != ' ') {
            return false; 
        }
        board[(position - 1) / 3][(position - 1) % 3] = marker;
        return true;
    }

    bool checkWin(char marker) const {
        
        for (int i = 0; i < 3; ++i) {
            if (board[i][0] == marker && board[i][1] == marker && board[i][2] == marker) return true;
            if (board[0][i] == marker && board[1][i] == marker && board[2][i] == marker) return true;
        }
        
        if (board[0][0] == marker && board[1][1] == marker && board[2][2] == marker) return true;
        if (board[0][2] == marker && board[1][1] == marker && board[2][0] == marker) return true;
        return false;
    }

    bool isFull() const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') return false;
            }
        }
        return true;
    }

    void reset() {
        board = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
    }

    char getCell(int row, int col) const {
        return board[row][col];
    }

    bool checkRow(int row, char marker) const {
        return (board[row][0] == marker && board[row][1] == marker && board[row][2] == marker);
    }

    bool checkColumn(int col, char marker) const {
        return (board[0][col] == marker && board[1][col] == marker && board[2][col] == marker);
    }

    bool checkDiagonal(char marker) const {
        return ((board[0][0] == marker && board[1][1] == marker && board[2][2] == marker) ||
            (board[0][2] == marker && board[1][1] == marker && board[2][0] == marker));
    }
};

class Player {
protected:
    char marker;
public:
    Player(char marker) : marker(marker) {}

    virtual int getMove() const = 0;

    char getMarker() const {
        return marker;
    }
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char marker) : Player(marker) {}

    int getMove() const override {
        int move;
        cout << "Enter your move (1-9): ";
        cin >> move;
        return move;
    }
};

class ComputerPlayer : public Player {
private:
    const Board& board;
public:
    ComputerPlayer(char marker, const Board& gameBoard) : Player(marker), board(gameBoard) {}

    int getMove() const override {
        
        for (int move = 1; move <= 9; ++move) {
            if (isValidMove(move)) {
                if (isWinningMove(move))
                    return move;
            }
        }

        
        char opponentMarker = (marker == 'X') ? 'O' : 'X';
        for (int move = 1; move <= 9; ++move) {
            if (isValidMove(move)) {
                if (isWinningMove(move, opponentMarker))
                    return move;
            }
        }

        
        vector<int> preferredMoves = { 5, 1, 3, 7, 9 };
        for (int move : preferredMoves) {
            if (isValidMove(move))
                return move;
        }

        
        int move;
        do {
            move = rand() % 9 + 1;
        } while (!isValidMove(move));

        return move;
    }

private:
    bool isValidMove(int move) const {
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        return (move >= 1 && move <= 9 && board.getCell(row, col) == ' ');
    }

    bool isWinningMove(int move, char playerMarker = ' ') const {
        if (playerMarker == ' ')
            playerMarker = marker;

        char opponentMarker = (playerMarker == 'X') ? 'O' : 'X';

        int row = (move - 1) / 3;
        int col = (move - 1) % 3;

        
        Board tempBoard = board;
        tempBoard.placeMarker(move, opponentMarker);

        
        if (tempBoard.checkRow(row, opponentMarker))
            return true;

        
        if (tempBoard.checkColumn(col, opponentMarker))
            return true;

        
        if ((row == col && tempBoard.checkDiagonal(opponentMarker)) ||
            (row + col == 2 && tempBoard.checkDiagonal(opponentMarker)))
            return true;

        return false;
    }

};


enum GameMode { HumanVsHuman, HumanVsComputer, ComputerVsComputer };

class Game {
private:
    void takeTurn() {
        int move;
        bool validMove = false;
        do {
            move = currentPlayer->getMove();
            validMove = board.placeMarker(move, currentPlayer->getMarker());
            if (!validMove) {
                cout << "Invalid move. Try again." << endl;
            }
        } while (!validMove);
    }

    Board board;
    unique_ptr<Player> player1;
    unique_ptr<Player> player2;
    Player* currentPlayer;
    bool gameOver;
public:
    Game(GameMode mode) : currentPlayer(nullptr), gameOver(false), board() {
        board.reset();
        if (mode == HumanVsHuman) {
            player1 = make_unique<HumanPlayer>('X');
            player2 = make_unique<HumanPlayer>('O');
        }
        else if (mode == HumanVsComputer) {
            player1 = make_unique<HumanPlayer>('X');
            player2 = make_unique<ComputerPlayer>('O', board);
        }
        else if (mode == ComputerVsComputer) {
            player1 = make_unique<ComputerPlayer>('X', board);
            player2 = make_unique<ComputerPlayer>('O', board);
        }
    }

    void start() {
        while (!gameOver) {
            currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
            takeTurn();
            board.draw();
            if (board.checkWin(currentPlayer->getMarker())) {
                cout << "Player " << currentPlayer->getMarker() << " wins!" << endl;
                gameOver = true;
            }
            else if (board.isFull()) {
                cout << "It's a tie!" << endl;
                gameOver = true;
            }
        }
    }
};

int main() {
    char choice;
    cout << "Cac che do choi:\n";
    cout << "1. Nguoi vs Nguoi\n";
    cout << "2. Nguoi vs May\n";
    cout << "3. May vs May\n";
    cout << "Chon che do (1, 2, or 3): ";
    cin >> choice;
    GameMode mode;
    switch (choice) {
    case '1':
        mode = HumanVsHuman;
        break;
    case '2':
        mode = HumanVsComputer;
        break;
    case '3':
        mode = ComputerVsComputer;
        break;
    default:
        cout << "Lua chon khong hop le...\n";
        return 1;
    }
    srand(time(nullptr)); // Seed the random number generator
    Game game(mode);
    game.start();
    return 0;
}
