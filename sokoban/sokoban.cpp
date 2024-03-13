#include <iostream>
using namespace std;
const int ROWS = 7;
const int COLS = 5;
struct Position {
    int x, y;
};
char board[ROWS][COLS];
void init() {
    char tempBoard[ROWS][COLS] = {
        {'1', '1', '1', '1', '1'},
        {'1', '5', '0', '0', '1'},
        {'1', '0', '2', '0', '1'},
        {'1', '1', '1', '0', '1'},
        {'1', '3', '2', '0', '1'},
        {'1', '0', '0', '3', '1'},
        {'1', '1', '1', '1', '1'},
    };
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = tempBoard[i][j];
        }
    }
}
bool isGoal(int x, int y) {
    if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
        return board[x][y] == '3';
    }
    return false;
}
int generateChildren(const Position& S, Position children[4]) {
    int count = 0;
    // Try moving up
    Position newPos = { S.x - 1, S.y };
    if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '2') {
        if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '1') {
            children[count++] = newPos;
        }
    }
    else if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] == '2') {
        if (board[newPos.x - 1][newPos.y] == '2' || board[newPos.x - 1][newPos.y] == '1') {
        }
        else {
            children[count++] = newPos;
        }
    }
    // Try moving down
    newPos = { S.x + 1, S.y };
    if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '2') {
        if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '1') {
            children[count++] = newPos;
        }
    }
    else if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] == '2') {
        if (board[newPos.x + 1][newPos.y] == '2' || board[newPos.x + 1][newPos.y] == '1') {
        }
        else {
            children[count++] = newPos;
        }
    }
    // Try moving left
    newPos = { S.x, S.y - 1 };
    if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '2') {
        if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '1') {
            children[count++] = newPos;
        }
    }
    else if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] == '2') {
        if (board[newPos.x][newPos.y-1] == '2' || board[newPos.x][newPos.y-1] == '1') {
        }
        else {
            children[count++] = newPos;
        }
    }
    // Try moving right
    newPos = { S.x, S.y + 1 };
    if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '2') {
        if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] != '1') {
            children[count++] = newPos;
        }
    }
    else if (newPos.x >= 0 && newPos.x < ROWS && newPos.y >= 0 && newPos.y < COLS && board[newPos.x][newPos.y] == '2') {
        if (board[newPos.x][newPos.y + 1] == '2' || board[newPos.x][newPos.y + 1] == '1') {
        }
        else {
            children[count++] = newPos;
        }
    }
    return count; 
}

bool isDeadlock() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == '2' && !isGoal(i, j)) {
                bool isBoxHorizontally = (board[i][j - 1] == '1' || board[i][j + 1] == '1');
                bool isBoxVertically = (board[i - 1][j] == '1' || board[i + 1][j] == '1');
                if (isBoxHorizontally && isBoxVertically) {
                    return true;
                }
            }
        }
    }
    return false; 
}
int main() {
    init(); 
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << board[i][j];
        }
        cout << endl;
    }
    cout << "Is position (" << 5 << "," << 3 << ") a goal? " << (isGoal(5, 3) ? "Yes" : "No") << endl;
    cout << "Is position (" << 4 << "," << 3 << ") a goal? " << (isGoal(4, 3) ? "Yes" : "No") << endl;
    Position currentPos = { 4, 1 };
    Position children[4];
    int numChildren = generateChildren(currentPos, children);
    cout << "Possible moves from (" << currentPos.x << "," << currentPos.y << "): " << numChildren << endl;
    for (int i = 0; i < numChildren; i++) {
        cout << "(" << children[i].x << "," << children[i].y << ")" << endl;
    }
    cout << "Is there a deadlock? " << (isDeadlock() ? "Yes" : "No") << endl;

    return 0;
}

/*
students :- 
Fakher Talahmeh

Majid Qattosh

*/