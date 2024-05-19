#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int ROWS = 7;
const int COLS = 7;
const int NUM_ACTIONS = 4;
const int NUM_EPISODES = 500;
const double ALPHA = 0.8;
const double EPSILON = 0.1;
const double PENALTY = -10.0;
const double REWARD = 10.0;
enum Actions { UP, DOWN, LEFT, RIGHT };

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

namespace std {
    template <>
    struct hash<State> {
        size_t operator()(const State& state) const {
            size_t rowHash = hash<int>()(state.playerRow);
            size_t colHash = hash<int>()(state.playerCol);
            size_t boardHash = 0;
            for (const auto& row : state.board) {
                for (char cell : row) {
                    boardHash ^= hash<char>()(cell) + 0x9e3779b9 + (boardHash << 6) + (boardHash >> 2);
                }
            }
            return rowHash ^ colHash ^ boardHash;
        }
    };
}
unordered_map<State, vector<double>> Q;
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

State initializeState() {
    State initialState;
    initialState.board = {
        {'1', '1', '1', '1', '1', '1', '1'},
        {'1', '0', '0', '0', '0', '0', '1'},
        {'1', '0', '2', '0', '0', '0', '1'},
        {'1', '1', '1', '0', '1', '0', '1'},
        {'1', '3', '2', '0', '1', '0', '1'},
        {'1', '0', '0', '3', '1', '0', '1'},
        {'1', '1', '1', '1', '1', '1', '1'}
    };
    return initialState;
}

bool isGoalState(const State& state) {
    for (const auto& row : state.board) {
        if (find(row.begin(), row.end(), '2') != row.end()) return false;
    }
    return true;
}

bool isDeadlockState(const State& state) {
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

Actions chooseAction(int stateIndex) {
    if (Q[stateVec[stateIndex]].empty()) {
        Q[stateVec[stateIndex]] = vector<double>(NUM_ACTIONS, 0.0);
    }

    if ((double)rand() / RAND_MAX < EPSILON) {
        return static_cast<Actions>(rand() % NUM_ACTIONS);
    }

    return static_cast<Actions>(distance(Q[stateVec[stateIndex]].begin(),
        max_element(Q[stateVec[stateIndex]].begin(), Q[stateVec[stateIndex]].end())));
}

void updateQValue(int stateIndex, Actions action, int nextStateIndex, double reward) {
    if (Q[stateVec[nextStateIndex]].empty()) {
        Q[stateVec[nextStateIndex]] = vector<double>(NUM_ACTIONS, 0.0);
    }
    double maxNextQValue = *max_element(Q[stateVec[nextStateIndex]].begin(), Q[stateVec[nextStateIndex]].end());
    Q[stateVec[stateIndex]][action] = reward + ALPHA * maxNextQValue;
}

State getNextState(const State& state, Actions action) {
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
        while (!isGoalState(currentState) && !isDeadlockState(currentState)) {
            Actions action = chooseAction(stateIndex);
            State nextState = getNextState(currentState, action);
            int nextStateIndex = find(stateVec.begin(), stateVec.end(), nextState) - stateVec.begin();
            if (nextStateIndex == stateVec.size()) {
                stateVec.push_back(nextState);
            }
            double reward = isGoalState(nextState) ? REWARD : (isDeadlockState(nextState) ? PENALTY : -1.0);
            updateQValue(stateIndex, action, nextStateIndex, reward);
            stateIndex = nextStateIndex;
            currentState = nextState;
        }
    }
}

void executePolicy(State initialState) {
    stateVec.push_back(initialState);
    int stateIndex = 0;
    while (!isGoalState(stateVec[stateIndex]) && !isDeadlockState(stateVec[stateIndex])) {
        printState(stateVec[stateIndex]);
        Actions action = chooseAction(stateIndex);
        State nextState = getNextState(stateVec[stateIndex], action);
        stateIndex = find(stateVec.begin(), stateVec.end(), nextState) - stateVec.begin();
        if (stateIndex == stateVec.size()) {
            stateVec.push_back(nextState);
        }
    }
    printState(stateVec[stateIndex]);
    if (isGoalState(stateVec[stateIndex])) {
        cout << "Congratulations! You have reached the goal state." << endl;
    }
    else {
        cout << "DEADLOCK!" << endl;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    State initialState = initializeState();
    int playerRow, playerCol;
    cout << "Enter the row: ";
    cin >> playerRow ;
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
    cout << "Executing policy..." << endl;
    executePolicy(initialState);
    return 0;
}
