#include "functionsetpole.h"
#include <iostream>


// ������� ��� ��������, ����� �� ���������� ����� num � �������� �������
bool isSafe(std::vector<std::vector< long int>>& arr, int row, int col, int num, int h) {
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

// ������� ��� ���������� ���������� ����� �������
void baseInit(std::vector<std::vector<long int>>& arr, int h) {
    srand(time(NULL));
    int numCells = h * h / 3;
    for (int i = 0; i < numCells; i++) {
        int row = rand() % h;
        int col = rand() % h;
        if (arr[row][col] == 0) {
            int num = rand() % h + 1;
            if (isSafe(arr, row, col, num, h)) {
                arr[row][col] = num;
            }
        }
    }
}
// ������� ��� ���������� ���������� ������ �����
bool solveRemaining(std::vector<std::vector<long int>>& arr, int h) {
    int row = -1;
    int col = -1;
    bool isEmpty = true;

    // ������� ��������� ������ ������
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < h; j++) {
            if (arr[i][j] == 0) {
                row = i;
                col = j;
                isEmpty = false;
                break;
            }
        }
        if (!isEmpty)
            break;
    }

    // ���� ������ ����� ���, ������ ������ ��������
    if (isEmpty)
        return true;

    // ������� ���������� ����� �� 1 �� h
    for (int num = 1; num <= h; num++) {
        if (isSafe(arr, row, col, num, h)) {
            arr[row][col] = num;
            if (solveRemaining(arr, h))
                return true;
            arr[row][col] = 0; 
        }
    }

    return false; // ���� ���������� ���������� �����, ���������� false
}
// ������� ��� ���������� ������� ������� �� 1 �� h
bool backtracking(std::vector<std::vector<long int>>& arr, int h) {
    bool solved = false;

    std::vector<std::vector<long int>> temp(h, std::vector<long int>(h, 0)); // ������� ��������� ������

    baseInit(temp, h); // ��������� �������� ����� ���������� �������

    if (solveRemaining(temp, h)) {
       arr = temp; // �������� ������� �� ������� ������
       solved = true;
             
    }
 
    return solved;
}

