#include <iostream>
#include <vector>
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
            return false; // Invalid move
        }
        board[(position - 1) / 3][(position - 1) % 3] = marker;
        return true;
    }

    bool checkWin(char marker) const {
        // Check rows and columns
        for (int i = 0; i < 3; ++i) {
            if (board[i][0] == marker && board[i][1] == marker && board[i][2] == marker) return true;
            if (board[0][i] == marker && board[1][i] == marker && board[2][i] == marker) return true;
        }
        // Check diagonals
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
public:
    ComputerPlayer(char marker) : Player(marker) {}

    int getMove() const override {
        // Simulate a simple AI move
        return rand() % 9 + 1;
    }
};

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
    Game() : currentPlayer(nullptr), gameOver(false) {
        board.reset();
    }

    void start() {
        char choice;
        cout << "Do you want to play against the computer? (y/n): ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            player1 = make_unique<HumanPlayer>('X');
            player2 = make_unique<ComputerPlayer>('O');
        }
        else {
            player1 = make_unique<HumanPlayer>('X');
            player2 = make_unique<HumanPlayer>('O');
        }

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
    Game game;
    game.start();
    return 0;
}
