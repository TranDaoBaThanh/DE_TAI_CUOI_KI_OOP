#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <Windows.h>
using namespace std;

void setPosition(int x, int y)
{
    COORD CursorPosition;
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

void textColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void printCentered(string text) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    int pos = (columns - text.length()) / 2;
    for (int i = 0; i < pos; ++i) {
        cout << " ";
    }
    cout << text << endl;
}



class Board {
private:
    vector<vector<char>> board = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
public:
    void draw() const {

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;


        int startX = (consoleWidth - 13) / 2;
        int startY = (consoleHeight - 7) / 2; 


        setPosition(startX, startY);

        // Draw the board
        cout << "-------------" << endl;
        for (int i = 0; i < 3; ++i) {
            setPosition(startX, startY + i * 2 + 1);
            cout << "| ";
            for (int j = 0; j < 3; ++j) {
                setMarkerColor(board[i][j]);
                cout << board[i][j] << " ";
                textColor(15);
                cout << "| ";
            }
            cout << endl;
            setPosition(startX, startY + i * 2 + 2);
            cout << "-------------" << endl;
        }
    }

    void setMarkerColor(char marker) const {
        if (marker == 'X')
            textColor(9); 
        else if (marker == 'O')
            textColor(12);
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
private:
    const Board& board; 
public:
    HumanPlayer(char marker, const Board& gameBoard) : Player(marker), board(gameBoard) {}

    int getMove() const override {
        int move;
        bool validMove = false;
        while (!validMove) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;


            int startX = (consoleWidth - 13) / 2;
            int startY = (consoleHeight - 7) / 2;


            setPosition(startX - 5, startY + 7);
            cout << "Enter your move (1-9): ";
            if (!(cin >> move)) {
                
                cin.clear();
                while (cin.get() != '\n');
                setPosition(startX -9, startY + 9);
                textColor(14);
                cout << "Invalid input. Please enter a number." << endl;
            }
            else if (move < 1 || move > 9) {
                setPosition(startX - 9, startY + 9);
                textColor(14);
                cout << "Invalid move. Move must be between 1 and 9." << endl;
            }
            else {
                int row = (move - 1) / 3;
                int col = (move - 1) % 3;
                if (board.getCell(row, col) != ' ') {
                    setPosition(startX - 9, startY + 9);
                    textColor(14);
                    cout << "Invalid move. Cell is already occupied." << endl;
                }
                else {
                    validMove = true;
                }
            }
        }
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
            Sleep(200);
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
            player1 = make_unique<HumanPlayer>('X', board);
            player2 = make_unique<HumanPlayer>('O', board);
        }
        else if (mode == HumanVsComputer) {
            player1 = make_unique<HumanPlayer>('X', board);
            player2 = make_unique<ComputerPlayer>('O', board);
        }
        else if (mode == ComputerVsComputer) {
            player1 = make_unique<ComputerPlayer>('X', board);
            player2 = make_unique<ComputerPlayer>('O', board);
        }
    }

    void start() {
        while (!gameOver) {
            system("cls");

            board.draw();

            currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
            takeTurn();


            system("cls");
            board.draw();

            if (board.checkWin(currentPlayer->getMarker())) {
                textColor(6);
                setPosition(52, 5);
                cout << "!!!Player " << currentPlayer->getMarker() << " wins!!!" << endl;
                gameOver = true;
                setPosition(42, 8);
                cout << "Press Enter to return to the main menu...";
                while (true) {
                    system("pause>nul");
                    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                        system("cls");
                        break;
                    }
                }
            }
            else if (board.isFull()) {
                textColor(6);
                setPosition(52, 5);
                cout << "!!!It's a tie!!!" << endl;
                gameOver = true;

                setPosition(42, 8);
                cout << "Press Enter to return to the main menu...";
                while (true) {
                    system("pause>nul");
                    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                        system("cls");
                        break;
                    }
                }
            }
        }
    }
};





int main() {
    while (true) {
        vector<string> options = { "Nguoi vs Nguoi", "Nguoi vs May", "May vs May" };
        int selectedIndex = 0;
        int yCord = 12;

        while (true) {
            system("cls");
            textColor(11);
            setPosition(42, 10); cout << "* * * * * * * CAC CHE DO CHOI * * * * * * * ";
            for (int i = 11; i <= 17; ++i) {
                setPosition(42, i); cout << "*";
            }
            for (int i = 11; i <= 17; ++i) {
                setPosition(84, i); cout << "*";
            }
            setPosition(42, 17); cout << "* * * * * * * * * * * * * * * * * * * * * *";

            for (int i = 0; i < options.size(); ++i) {
                if (i == selectedIndex) textColor(10);
                else textColor(15);
                setPosition(55, yCord + i);
                cout << options[i];
            }

            while (true) {
                system("pause>nul");
                if (GetAsyncKeyState(VK_UP) & 0x8000) {
                    selectedIndex = (selectedIndex - 1 + options.size()) % options.size();
                    break;
                }
                
                if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                    selectedIndex = (selectedIndex + 1) % options.size();
                    break;
                }
                
                if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                    system("cls");
                    break;
                }

            }
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                break;
            }
        }

        GameMode mode;
        switch (selectedIndex) {
        case 0:
            mode = HumanVsHuman;
            break;
        case 1:
            mode = HumanVsComputer;
            break;
        case 2:
            mode = ComputerVsComputer;
            break;
        default:
            cout << "Lua chon khong hop le...\n";
            return 1;
        }
        srand(time(nullptr));

        Game game(mode);
        game.start();
    }
    return 0;
}
