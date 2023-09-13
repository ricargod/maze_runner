#include <stdio.h>
#include <stack>
#include <stdlib.h>
#include <thread>       
#include <chrono>  

char** maze;
int num_rows;
int num_cols;

struct pos_t {
    int i;
    int j;
};

bool exit_found = false;


//função para ler o labirinto do arquivo e armazenar na matriz e retornar a posição inicial
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
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

//funçao para percorrer o labirinto
void walk(pos_t pos) {
    std::stack<pos_t> valid_positions; //pilha para armazerar as posiçoes validas
    valid_positions.push(pos);

    while (!valid_positions.empty()) {
        pos_t position = valid_positions.top();
        valid_positions.pop();
        maze[position.i][position.j]='o';
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        maze[position.i][position.j]='.';
        //vetor com as 4 posições em volta da atual
        pos_t next_positions[4] = {
            {position.i + 1, position.j},
            {position.i - 1, position.j},
            {position.i, position.j + 1},
            {position.i, position.j - 1}
        };

        //percorre as 4 posiçoes, verifica se estao no labirinto, e se são validas.
        for (int k = 0; k < 4; ++k) {
            if (next_positions[k].i >= 0 && next_positions[k].i < num_rows && next_positions[k].j >= 0 && next_positions[k].j < num_cols && maze[next_positions[k].i][next_positions[k].j] != '#' && maze[next_positions[k].i][next_positions[k].j] != '.') {
				//caso seja valida, verifica se é a saida
                if (maze[next_positions[k].i][next_positions[k].j] == 's') {
                    maze[next_positions[k].i][next_positions[k].j] = 'o';
                    exit_found=true;
                }
                //se nao fora saida, salva na pilha e marca a posição como ja navegada
                valid_positions.push(next_positions[k]);
                maze[next_positions[k].i][next_positions[k].j] = '.';
            }
        }
        // if (valid_positions.size()==1){
        //     position= valid_positions.top();
        // }else
        if (valid_positions.size()==2){
            std::thread t1(walk,valid_positions.top());
            t1.detach();
            valid_positions.pop();
        }else
        if (valid_positions.size()==3){
            std::thread t2(walk,valid_positions.top());
            t2.detach();
            valid_positions.pop();
            std::thread t3(walk,valid_positions.top());
            t3.detach();
            valid_positions.pop();
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <maze_file>\n", argv[0]);
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);
    
    std::thread t(walk, initial_pos);
    t.detach();

    while(!exit_found){
        print_maze();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        system("clear");
    }
    print_maze();

    if (exit_found) {
        printf("Saída Encontrada!\n");
    } else {
        printf("Saida não encontrada.\n");
    }

    // Free allocated memory
    for (int i = 0; i < num_rows; ++i) {
        delete[] maze[i];
    }
    delete[] maze;

    return 0;
}
