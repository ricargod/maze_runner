#include <iostream>
#include <thread>
#include <vector>
#include <stack>
#include <mutex>

char** maze;
int num_rows;
int num_cols;
std::mutex mtx;

struct pos_t {
    int i;
    int j;
};

std::stack<pos_t> valid_positions;

pos_t load_maze(const char* file_name) {
    pos_t initial_pos;
    FILE* arq;
    arq = fopen(file_name, "r");

    fscanf(arq, "%d %d", &num_rows, &num_cols);

    maze = new char*[num_rows];
    for (int i = 0; i < num_rows; ++i) {
        maze[i] = new char[num_cols];
        for (int j = 0; j < num_cols; ++j) {
            fscanf(arq, " %c", &maze[i][j]);
            if (maze[i][j] == 'e') {
                initial_pos.i = i;
                initial_pos.j = j;
            }
        }
    }
    fclose(arq);  // Close the file
    return initial_pos;
}

void print_maze() {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            std::cout << maze[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool explore(pos_t pos) {
    mtx.lock();
    valid_positions.push(pos);
    mtx.unlock();
    print_maze();
    while (!valid_positions.empty()) {
        pos_t position;
        mtx.lock();
        if (!valid_positions.empty()) {
            position = valid_positions.top();
            valid_positions.pop();
        } else {
            mtx.unlock();
            continue;
        }
        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate exploration time

        if (maze[position.i][position.j] == 's') {
            return true;
        }

        pos_t next_positions[4] = {
            {position.i + 1, position.j},
            {position.i - 1, position.j},
            {position.i, position.j + 1},
            {position.i, position.j - 1}
        };

        std::vector<std::thread> threads;

        for (int k = 0; k < 4; ++k) {
            int i = next_positions[k].i;
            int j = next_positions[k].j;

            if (i >= 0 && i < num_rows && j >= 0 && j < num_cols && maze[i][j] != '#' && maze[i][j] != 'o') {
                if (maze[i][j] == 's') {
                    std::cout << "Saida!" << std::endl;
                    return true;
                }
                maze[i][j] = 'o';
                threads.emplace_back(explore, next_positions[k]);
            }
        }

        for (std::thread& t : threads) {
            t.join(); // Wait for all child threads to finish exploration
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);

    bool exit_found = explore(initial_pos);

    if (exit_found) {
        std::cout << "Saida encontrada!" << std::endl;
    } else {
        std::cout << "Não foi encontrada a saida." << std::endl;
    }

    // Free allocated memory
    for (int i = 0; i < num_rows; ++i) {
        delete[] maze[i];
    }
    delete[] maze;

    return 0;
}