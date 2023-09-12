
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

std::stack<pos_t> valid_positions; //pilha para armazerar as posiçoes validas

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
bool walk(pos_t pos) {
    valid_positions.push(pos);

    while (!valid_positions.empty()) {
        pos_t position = valid_positions.top();
        valid_positions.pop();
        
        maze[position.i][position.j] = 'o';
        
        
        system("clear");  
        print_maze();
        
        //loop para criar delay de 50ms entre as impressoes do labirinto atualizado
        auto start_time = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() < 50) {
            // Loop vazio
        }

        //vetor com as 4 posições em volta da atual
        pos_t next_positions[4] = {
            {position.i + 1, position.j},
            {position.i - 1, position.j},
            {position.i, position.j + 1},
            {position.i, position.j - 1}
        };

        //percorre as 4 posiçoes, verifica se estao no labirinto, e se são validas.
        for (int k = 0; k < 4; ++k) {
            if (next_positions[k].i >= 0 && next_positions[k].i < num_rows && next_positions[k].j >= 0 && next_positions[k].j < num_cols && maze[next_positions[k].i][next_positions[k].j] != '#' && maze[next_positions[k].i][next_positions[k].j] != 'o') {
				//caso seja valida, verifica se é a saida
                if (maze[next_positions[k].i][next_positions[k].j] == 's') {
                    maze[next_positions[k].i][next_positions[k].j] = '.';
                    return true;
                }
                //se nao fora saida, salva na pilha e marca a posição como ja navegada
                valid_positions.push(next_positions[k]);
                maze[next_positions[k].i][next_positions[k].j] = 'o';
            }
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <maze_file>\n", argv[0]);
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);
    bool exit_found = walk(initial_pos);

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


    return 0;
}
