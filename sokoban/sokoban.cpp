#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
using namespace std;
const int ROWS = 7;
const int COLS = 7;
const int NUM_ACTIONS = 4;
const int NUM_EPISODES = 100;
const double ALPHA = 0.8;
const double EPSILON = 0.1;
const double PENALTY = -10.0;
const double REWARD = 10.0;
const int actions[] = { 0, 1, 2, 3 };
struct State {
    vector<vector<char>> board;
    int playerRow;
    int playerCol;
    bool operator==(const State& other) const {
        return playerRow == other.playerRow && playerCol == other.playerCol && board == other.board;
    }
    bool operator<(const State& other) const {
        if (playerRow != other.playerRow) return playerRow < other.playerRow;
        if (playerCol != other.playerCol) return playerCol < other.playerCol;
        return board < other.board;
    }
};
map<State, vector<double>> Q;
vector<State> stateVec;
void printState(const State& state) {
    for (const auto& row : state.board) {
        for (char cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
    cout << endl;
}
State init() {
    State initialState;
    initialState.board = {
        {'1', '1', '1', '1', '1', '1', '1'},
        {'1', '0', '0', '0', '0', '0', '1'},
        {'1', '0', '2', '0', '0', '0', '1'},
        {'1', '1', '1', '0', '1', '1', '1'},
        {'1', '3', '2', '0', '1', '1', '1'},
        {'1', '0', '0', '0', '3', '1', '1'},
        {'1', '1', '1', '1', '1', '1', '1'}
    };
    return initialState;
}
bool isGoal(const State& state) {
    for (const auto& row : state.board) {
        if (find(row.begin(), row.end(), '2') != row.end()) return false;
    }
    return true;
}
bool isDeadlock(const State& state) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (state.board[i][j] == '2') {
                if ((state.board[i - 1][j] == '1' || state.board[i + 1][j] == '1') &&
                    (state.board[i][j - 1] == '1' || state.board[i][j + 1] == '1')) {
                    return true;
                }
            }
        }
    }
    return false;
}
int chooseAction(int stateIndex) {
    if (Q[stateVec[stateIndex]].empty()) {
        Q[stateVec[stateIndex]] = vector<double>(NUM_ACTIONS, 0.0);
    }

    if ((double)rand() / RAND_MAX < EPSILON) {
        return actions[rand() % NUM_ACTIONS];
    }

    return distance(Q[stateVec[stateIndex]].begin(),
        max_element(Q[stateVec[stateIndex]].begin(), Q[stateVec[stateIndex]].end()));
}
void updateQValue(int stateIndex, int action, int nextStateIndex, double reward) {
    if (Q[stateVec[nextStateIndex]].empty()) {
        Q[stateVec[nextStateIndex]] = vector<double>(NUM_ACTIONS, 0.0);
    }
    double maxNextQValue = *max_element(Q[stateVec[nextStateIndex]].begin(), Q[stateVec[nextStateIndex]].end());
    Q[stateVec[stateIndex]][action] = reward + ALPHA * maxNextQValue;
}
State getNextState(const State& state, int action) {
    State nextState = state;
    int dRow[] = { -1, 1, 0, 0 };
    int dCol[] = { 0, 0, -1, 1 };
    int newRow = state.playerRow + dRow[action];
    int newCol = state.playerCol + dCol[action];
    if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS && state.board[newRow][newCol] != '1') {
        if (state.board[newRow][newCol] == '2' || state.board[newRow][newCol] == '4') {
            int boxRow = newRow + dRow[action];
            int boxCol = newCol + dCol[action];
            if (boxRow >= 0 && boxRow < ROWS && boxCol >= 0 && boxCol < COLS && state.board[boxRow][boxCol] != '1' && state.board[boxRow][boxCol] != '2' && state.board[boxRow][boxCol] != '4') {
                nextState.board[newRow][newCol] = '5';
                nextState.board[boxRow][boxCol] = (state.board[boxRow][boxCol] == '3') ? '4' : '2';
            }
            else {
                return state;
            }
        }
        else {
            nextState.board[newRow][newCol] = '5';
        }
        nextState.board[state.playerRow][state.playerCol] = (state.board[state.playerRow][state.playerCol] == '4') ? '3' : '0';
        nextState.playerRow = newRow;
        nextState.playerCol = newCol;
    }
    return nextState;
}
void Qlearning(State initialState) {
    stateVec.push_back(initialState);
    for (int episode = 0; episode < NUM_EPISODES; ++episode) {
        State currentState = initialState;
        int stateIndex = 0;
        while (!isGoal(currentState) && !isDeadlock(currentState)) {
            int action = chooseAction(stateIndex);
            State nextState = getNextState(currentState, action);
            int nextStateIndex = find(stateVec.begin(), stateVec.end(), nextState) - stateVec.begin();
            if (nextStateIndex == stateVec.size()) {
                stateVec.push_back(nextState);
            }
            double reward = isGoal(nextState) ? REWARD : (isDeadlock(nextState) ? PENALTY : -1.0);
            updateQValue(stateIndex, action, nextStateIndex, reward);
            stateIndex = nextStateIndex;
            currentState = nextState;
        }
    }
}
int main() {
    State initialState = init();
    printState(initialState);
    int playerRow, playerCol;
    cout << "Enter the row: ";
    cin >> playerRow;
    cout << "Enter the column: ";
    cin >> playerCol;
    while (playerRow < 0 || playerRow >= ROWS || playerCol < 0 || playerCol >= COLS || initialState.board[playerRow][playerCol] == '1' || initialState.board[playerRow][playerCol] == '2') {
        cout << "Invalid position. Please enter a valid position (row and column): ";
        cin >> playerRow >> playerCol;
    }
    initialState.playerRow = playerRow;
    initialState.playerCol = playerCol;
    initialState.board[playerRow][playerCol] = '5';
    cout << "Training..." << endl;
    Qlearning(initialState);
    cout << "Training completed." << endl;
    stateVec.push_back(initialState);
    int stateIndex = 0;
    while (!isGoal(stateVec[stateIndex]) && !isDeadlock(stateVec[stateIndex])) {
        printState(stateVec[stateIndex]);
        int action = chooseAction(stateIndex);
        State nextState = getNextState(stateVec[stateIndex], action);
        stateIndex = find(stateVec.begin(), stateVec.end(), nextState) - stateVec.begin();
        if (stateIndex == stateVec.size()) {
            stateVec.push_back(nextState);
        }
    }
    printState(stateVec[stateIndex]);
    if (isGoal(stateVec[stateIndex])) {
        cout << "Congratulations!" << endl;
    }
    else {
        cout << "DEADLOCK!" << endl;
    }
    return 0;
}