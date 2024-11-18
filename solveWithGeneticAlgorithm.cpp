#include "solveWithGeneticAlgorithm.h"
#include <random>
#include <algorithm>

using namespace std;

bool isSafe2(std::vector<std::vector< long int>>& arr, int row, int col, int num, int h) {
    // ��������� ������
    for (int i = 0; i < h; i++) {
        if (arr[row][i] == num)
            return false;
    }

    // ��������� �������
    for (int i = 0; i < h; i++) {
        if (arr[i][col] == num)
            return false;
    }

    // ��������� �������
    int sqrtH = sqrt(h);
    int boxRowStart = row - row % sqrtH;
    int boxColStart = col - col % sqrtH;

    for (int i = 0; i < sqrtH; i++) {
        for (int j = 0; j < sqrtH; j++) {
            if (arr[i + boxRowStart][j + boxColStart] == num && (i + boxRowStart != row || j + boxColStart != col))
                return false;
        }
    }

    return true;
}

void fillRandomCells2(std::vector<std::vector<long int>>& arr, int h) {
    srand(time(NULL)); // �������������� ��������� ��������� �����

    // ��������� �������� �������� ����� ����� �������
    int numCells = h * h / 3;
    for (int i = 0; i < numCells; i++) {
        int row = rand() % h;
        int col = rand() % h;
        
            int num = rand() % h + 1;
            if (isSafe2(arr, row, col, num, h)) {
                arr[row][col] = num;
            }
        
    }
}

// ������� ������ ��������������� �������
int fitness(const vector<vector<long int>>& solution, int h) {
    int conflicts = 0;

    // �������� �����
    for (int row = 0; row < h; row++) {
        vector<bool> seen(h + 1, false);
        for (int col = 0; col < h; col++) {
            if (solution[row][col] != 0) {
                if (seen[solution[row][col]]) {
                    conflicts++;
                }
                else {
                    seen[solution[row][col]] = true;
                }
            }
        }
    }

    // �������� ��������
    for (int col = 0; col < h; col++) {
        vector<bool> seen(h + 1, false);
        for (int row = 0; row < h; row++) {
            if (solution[row][col] != 0) {
                if (seen[solution[row][col]]) {
                    conflicts++;
                }
                else {
                    seen[solution[row][col]] = true;
                }
            }
        }
    }

    // �������� ���������
    int sqrtH = static_cast<int>(sqrt(h));
    for (int sqRow = 0; sqRow < sqrtH; sqRow++) {
        for (int sqCol = 0; sqCol < sqrtH; sqCol++) {
            vector<bool> seen(h + 1, false);
            for (int row = sqRow * sqrtH; row < (sqRow + 1) * sqrtH; row++) {
                for (int col = sqCol * sqrtH; col < (sqCol + 1) * sqrtH; col++) {
                    if (solution[row][col] != 0) {
                        if (seen[solution[row][col]]) {
                            conflicts++;
                        }
                        else {
                            seen[solution[row][col]] = true;
                        }
                    }
                }
            }
        }
    }

    return conflicts;
}

// ������� ��� �������� ��������� ���������
vector<vector<vector<long int>>> initPopulation(int size, const vector<vector<long int>>& initialBoard, int h) {
    vector<vector<vector<long int>>> population(size);
    for (auto& solution : population) {
        solution = initialBoard;
    }
    return population;
}

// ������� ��� ����������� ���� �������
vector<vector<long int>> crossover(const vector<vector<long int>>& parent1, const vector<vector<long int>>& parent2, int h) {
    vector<vector<long int>> child(h, vector<long int>(h, 0));
    int sqrtH = static_cast<int>(sqrt(h));

    // �������� ���������� ����� �� ������������ �������
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < h; col++) {
            if (parent1[row][col] != 0) {
                child[row][col] = parent1[row][col];
            }
            else if (parent2[row][col] != 0) {
                child[row][col] = parent2[row][col];
            }
        }
    }

    // ��������� ������ ������, ������� ����������
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < h; col++) {
            if (child[row][col] == 0) {
                vector<bool> seen(h + 1, false);
                // ��������� ������
                for (int i = 0; i < h; i++) {
                    if (child[row][i] != 0) {
                        seen[child[row][i]] = true;
                    }
                }
                // ��������� �������
                for (int i = 0; i < h; i++) {
                    if (child[i][col] != 0) {
                        seen[child[i][col]] = true;
                    }
                }
                // ��������� �������
                int sqRow = row / sqrtH;
                int sqCol = col / sqrtH;
                for (int i = sqRow * sqrtH; i < (sqRow + 1) * sqrtH; i++) {
                    for (int j = sqCol * sqrtH; j < (sqCol + 1) * sqrtH; j++) {
                        if (child[i][j] != 0) {
                            seen[child[i][j]] = true;
                        }
                    }
                }
                // ������� ������ ��������� ����� � ��������� ������
                for (int num = 1; num <= h; num++) {
                    if (!seen[num]) {
                        child[row][col] = num;
                        break;
                    }
                }
            }
        }
    }

    return child;
}
// ������� ��� ������� �������
void mutate(vector<vector<long int>>& solution, int h) {
    random_device rd;
    srand(time(NULL)); // �������������� ��������� ��������� �����
    mt19937 gen(rd());
    uniform_int_distribution<> rowDist(0, h - 1);
    uniform_int_distribution<> colDist(0, h - 1);
    uniform_int_distribution<long int> numDist(1, h);

    // �������� ��������� ������ ��� �������
    int row = rowDist(gen);
    int col = colDist(gen);

    // ���������, ����� �� ��������� �������� �������� ������
    vector<bool> seen(h + 1, false);
    bool canMutate = true;

    // ��������� ������
    for (int i = 0; i < h; i++) {
        if (solution[row][i] != 0) {
            seen[solution[row][i]] = true;
        }
    }
    // ��������� �������
    for (int i = 0; i < h; i++) {
        if (solution[i][col] != 0) {
            seen[solution[i][col]] = true;
        }
    }
    // ��������� �������
    int sqrtH = static_cast<int>(sqrt(h));
    int sqRow = row / sqrtH;
    int sqCol = col / sqrtH;
    for (int i = sqRow * sqrtH; i < (sqRow + 1) * sqrtH; i++) {
        for (int j = sqCol * sqrtH; j < (sqCol + 1) * sqrtH; j++) {
            if (solution[i][j] != 0) {
                seen[solution[i][j]] = true;
            }
        }
    }

    // ���� � ������, ������� ��� �������� ���� ������������� �����, ������������ �� �������
    for (int num = 1; num <= h; num++) {
        if (seen[num]) {
            canMutate = false;
            break;
        }
    }

    // ��������� �������, ���� ��� ��������
    if (canMutate) {
        long int newValue;
        do {
            newValue = rand() % h + 1;;
        } while (seen[newValue]);
        solution[row][col] = newValue;
    }
}

// �������� ��������
vector<vector<long int>> solveWithGeneticAlgorithm( int populationSize, int generations, int h) {
    std::vector<std::vector<long int>> arr(h, std::vector<long int>(h, 0));
    fillRandomCells2(arr, h);
    vector<pair<int, int>> fitnessSolutions;
    vector<vector<vector<long int>>> population = initPopulation(populationSize, arr, h);
    for (int generation = 0; generation < generations; generation++) {
        // ������ ����������� ������� �������
        for (int i = 0; i < populationSize; i++) {
            fitnessSolutions.emplace_back(fitness(population[i], h), i);
        }
        sort(fitnessSolutions.begin(), fitnessSolutions.end());
        // ����� ��������� ��� �����������
        vector<vector<vector<long int>>> newPopulation;
        for (int i = 0; i < populationSize / 2; i++) {
            int parent1Index = fitnessSolutions[i].second;
            int parent2Index = fitnessSolutions[populationSize - i - 1].second;
            newPopulation.push_back(crossover(population[parent1Index], population[parent2Index], h));
            newPopulation.push_back(crossover(population[parent2Index], population[parent1Index], h));
        }
        // �������
        for (auto& solution : newPopulation) {
            mutate(solution, h);
        }
        population = move(newPopulation);
    }
    // ������� ������� � ��������� ��������� �����������
    int bestIndex = fitnessSolutions[0].second;
    return population[bestIndex];
}