#include "functiondeletecells.h"
#include <random>


std::vector<std::vector<long int>> deleteRandomCells(std::vector<std::vector<long int>>& arrF, int h,int d) {
    srand(time(NULL)); // »нициализируем генератор случайных чисел
    std::vector<std::vector<long int>> arr= arrF;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, h * h - 1);
    int numCells = d * h ;
    if (h == 4) {
        numCells = 10;
    }
    for (int i = 0; i < numCells; i++) {
        int index = dis(gen);
        int row = index / h;
        int col = index % h;
        if (arr[row][col] != 0) {
            arr[row][col] = 0;
        }
        else {
            i--;
        }
    }
    

    return arr;
}
