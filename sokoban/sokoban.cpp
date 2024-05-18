#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int ROWS = 7;
const int COLS = 7;
const int ACTIONS = 4;
const int EPOCHS = 1000;
const double ALPHA = 0.1;
const double GAMMA = 0.9;
const double PENALTY = -10.0;
const double REWARD = 10.0;
const int MAX_PATH_LENGTH = 100;
struct Position {
    int x, y;
};

char board[ROWS][COLS];
double Q[ROWS][COLS][ACTIONS];
Position solutionPath[MAX_PATH_LENGTH];
void init() {
    char tempBoard[ROWS][COLS] = {
        {'1', '1', '1', '1', '1', '1', '1'},
        {'1', '0', '0', '0', '0', '0', '1'},
        {'1', '0', '2', '0', '0', '0', '1'},
        {'1', '0', '0', '0', '0', '0', '1'},
        {'1', '3', '0', '0', '2', '0', '1'},
        {'1', '0', '0', '0', '0', '3', '1'},
        {'1', '1', '1', '1', '1', '1', '1'},
    };
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = tempBoard[i][j];
            for (int k = 0; k < ACTIONS; k++) {
                Q[i][j][k] = 0.0;
            }
        }
    }
}

bool isGoal(int x, int y) {
    return board[x][y] == '3';
}

int generateChildren(const Position& S, Position children[4]) {
    int count = 0;
    Position newPos;
    // Try moving up
    newPos = { S.x - 1, S.y };
    if (newPos.x >= 0 && board[newPos.x][newPos.y] != '1' && board[newPos.x][newPos.y] != '2') {
        children[count++] = newPos;
    }
    // Try moving down
    newPos = { S.x + 1, S.y };
    if (newPos.x < ROWS && board[newPos.x][newPos.y] != '1' && board[newPos.x][newPos.y] != '2') {
        children[count++] = newPos;
    }
    // Try moving left
    newPos = { S.x, S.y - 1 };
    if (newPos.y >= 0 && board[newPos.x][newPos.y] != '1' && board[newPos.x][newPos.y] != '2') {
        children[count++] = newPos;
    }
    // Try moving right
    newPos = { S.x, S.y + 1 };
    if (newPos.y < COLS && board[newPos.x][newPos.y] != '1' && board[newPos.x][newPos.y] != '2') {
        children[count++] = newPos;
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

void updateQValue(Position state, int action, double reward, Position nextState) {
    double oldQ = Q[state.x][state.y][action];
    double maxQ = 0.0;
    for (int i = 0; i < ACTIONS; i++) {
        maxQ = max(maxQ, Q[nextState.x][nextState.y][i]);
    }
    Q[state.x][state.y][action] = oldQ + ALPHA * (reward + GAMMA * maxQ - oldQ);
}

void qLearning(Position start) {
    srand(time(0));
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        Position current = start;
        while (!isGoal(current.x, current.y) && !isDeadlock()) {
            Position children[4];
            int numChildren = generateChildren(current, children);
            if (numChildren == 0) break;

            int action = rand() % numChildren;
            Position nextState = children[action];
            double reward = isGoal(nextState.x, nextState.y) ? REWARD : (isDeadlock() ? PENALTY : -1);
            updateQValue(current, action, reward, nextState);

            if (isGoal(nextState.x, nextState.y) || isDeadlock()) break;
            current = nextState;
        }
    }
}

int solve(Position start, Position solutionPath[]) {
    int pathLength = 0;
    Position current = start;
    solutionPath[pathLength++] = current;
    while (!isGoal(current.x, current.y) && !isDeadlock() && pathLength < MAX_PATH_LENGTH) {
        Position children[4];
        int numChildren = generateChildren(current, children);
        if (numChildren == 0) break;
        int bestAction = 0;
        double bestQ = Q[current.x][current.y][0];
        for (int i = 1; i < numChildren; i++) {
            if (Q[current.x][current.y][i] > bestQ) {
                bestQ = Q[current.x][current.y][i];
                bestAction = i;
            }
        }
        Position nextState = children[bestAction];
        solutionPath[pathLength++] = nextState;
        if (isGoal(nextState.x, nextState.y) || isDeadlock()) break;
        current = nextState;
    }

    return pathLength;
}

void printBoard() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << board[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void printBoardWithBox(Position box, Position player) {
    char tempBoard[ROWS][COLS];
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            tempBoard[i][j] = board[i][j];
        }
    }
    tempBoard[box.x][box.y] = '2';
    tempBoard[player.x][player.y] = '5';
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << tempBoard[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void findPositions(char target, Position positions[], int& count) {
    count = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == target) {
                positions[count++] = { i, j };
            }
        }
    }
}
bool moveBox(Position& box, Position& player, Position nextPos) {
    if (board[nextPos.x][nextPos.y] == '1') {
        return false;
    }
    if (board[nextPos.x][nextPos.y] == '0' || board[nextPos.x][nextPos.y] == '3') {
        board[box.x][box.y] = '0';
        board[player.x][player.y] = '5';
        if (board[nextPos.x][nextPos.y] == '3') {
            board[nextPos.x][nextPos.y] = '4';
        }
        else {
            board[nextPos.x][nextPos.y] = '2';
        }
        box = nextPos;
        return true;
    }
    if (board[nextPos.x][nextPos.y] == '5') {
        Position playerNextPos = { 2 * nextPos.x - player.x, 2 * nextPos.y - player.y };
        if (board[playerNextPos.x][playerNextPos.y] == '0' || board[playerNextPos.x][playerNextPos.y] == '3') {
            // Move the box
            board[box.x][box.y] = '0';
            board[nextPos.x][nextPos.y] = '2';
            box = nextPos;
            // Move the player
            board[player.x][player.y] = '0';
            board[playerNextPos.x][playerNextPos.y] = '5';
            player = playerNextPos;
            return true;
        }
    }
    return false;
}
int main() {
    init();
    Position boxes[ROWS * COLS];
    Position goals[ROWS * COLS];
    Position player;
    cout << "Enter Any Row: ";
    cin >> player.x;
    cout << "Enter Any Column: ";
    cin >> player.y;
    board[player.x][player.y] = '5';
    printBoard();
    int playerCount = 1;
    int boxCount, goalCount;
    findPositions('2', boxes, boxCount);
    findPositions('3', goals, goalCount);
    findPositions('5', &player, playerCount);
    cout << "Found " << boxCount << " boxes and " << goalCount << " goals." << endl;
    cout << "Player starts at (" << player.x << "," << player.y << ")." << endl;
    for (int i = 0; i < boxCount; i++) {
        Position box = boxes[i];
        qLearning(box);
        int pathLength = solve(box, solutionPath);
        cout << "Solution path for box at (" << box.x << "," << box.y << "):" << endl;
        for (int j = 1; j < pathLength; j++) {
            if (box.y != player.y) {
                if (solutionPath[j].y == box.y) {
                    if (box.y > player.y) {
                        for (int k = box.y - player.y; k > 0; k--) {
                            board[player.x][player.y] = '0';
                            if (player.y + 1 != box.y || player.x != box.x) {
                                player.y += 1;
                            }
                            else {
                                if (solutionPath[j].x > player.x) {
                                    player.x -= 1;
                                }
                                else {
                                    player.x += 1;
                                }
                                k++;
                            }
                            board[player.x][player.y] = '5';
                            printBoard();
                        }
                    }
                    else {
                        for (int k = player.y - box.y; k > 0; k--) {
                            board[player.x][player.y] = '0';
                            if (player.y - 1 != box.y || player.x != box.x) {
                                player.y -= 1;
                            }
                            else {
                                if (solutionPath[j].x > player.x) {
                                    player.x -= 1;
                                }
                                else {
                                    player.x += 1;
                                }
                                k++;
                            }
                            board[player.x][player.y] = '5';
                            printBoard();
                        }
                    }
                }
            }
            if (box.x != player.x) {
                if (solutionPath[j].x == box.x) {
                    if (box.x > player.x) {
                        for (int k = box.x - player.x; k > 0; k--) {
                            board[player.x][player.y] = '0';
                            if (player.y != box.y || player.x + 1 != box.x) {
                                player.x += 1;
                            }
                            else {
                                if (solutionPath[j].y > player.y) {
                                    player.y -= 1;
                                }
                                else {
                                    player.y += 1;
                                }
                                k++;
                            }
                            board[player.x][player.y] = '5';

                            printBoard();
                        }
                    }
                    else {
                        for (int k = player.x - box.x; k > 0; k--) {
                            board[player.x][player.y] = '0';
                            if (player.y != box.y || player.x - 1 != box.x) {
                                player.x -= 1;
                            }
                            else {
                                if (solutionPath[j].y > player.y) {
                                    player.y -= 1;
                                }
                                else {
                                    player.y += 1;
                                }
                                k++;
                            }
                            board[player.x][player.y] = '5';
                            printBoard();
                        }
                    }
                }
            }
            if (player.y == box.y && player.y == solutionPath[j].y) {
                if (box.x > player.x) {
                    for (int k = box.x - player.x; k > 1; k--) {
                        board[player.x][player.y] = '0';
                        player.x++;
                        board[player.x][player.y] = '5';
                        printBoard();
                    }
                }
                else {
                    for (int k = player.x - box.x; k > 1; k--) {
                        board[player.x][player.y] = '0';
                        player.x--;
                        board[player.x][player.y] = '5';
                        printBoard();
                    }
                }
            }
            if (player.x == box.x && player.x == solutionPath[j].x) {
                if (box.y > player.y) {
                    for (int k = box.y - player.y; k > 1; k--) {
                        board[player.x][player.y] = '0';
                        player.y++;
                        board[player.x][player.y] = '5';
                        printBoard();
                    }
                }
                else {
                    for (int k = player.y - box.y; k > 1; k--) {
                        board[player.x][player.y] = '0';
                        player.y--;
                        board[player.x][player.y] = '5';
                        printBoard();
                    }
                }
            }
            board[player.x][player.y] = '0';
            player = box;
            if (moveBox(box, player, solutionPath[j])) {
                printBoard();
            }
        }
    }
    return 0;
}