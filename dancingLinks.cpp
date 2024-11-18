#include <random>
#include "dancingLinks.h"

// Структура узла в списке Dancing Links
struct Node;
struct Header;

std::vector<Node*> nodes;
std::mt19937 gen(std::random_device{}()); // Определение генератора случайных чисел

struct Node {
    int row, col, value;
    Node* up, * down, * left, * right;
    Header* head;

    Node(int r, int c, int v) : row(r), col(c), value(v), 
    up(nullptr), down(nullptr), left(nullptr), right(nullptr), head(nullptr) {}
};

// Структура заголовка узла
struct Header {
    int size;
    Node* head;

    Header() : size(0), head(nullptr) {}
};


// Вспомогательная функция для связывания узла с заголовком
void linkNode(Header& header, Node* node) {
    node->head = &header;
    if (header.head == nullptr) {
        header.head = node->left = node->right = node;
    }
    else {
        Node* oldHead = header.head;
        node->left = oldHead;
        node->right = oldHead->right;
        oldHead->right->left = node;
        oldHead->right = node;
    }
    header.size++;
}

void removeNode(Node* node) {
    if (node == nullptr || node->left == nullptr || node->right == nullptr || node->head == nullptr) {
        return; // Проверка на корректность инициализации узла и его указателей
    }

    node->left->right = node->right;
    node->right->left = node->left;
    node->head->size--;

    for (Node* curr = node->up; curr != nullptr && curr != node; curr = curr->up) {
        Node* temp = curr->left;
        while (temp != curr) {
            if (temp->head == nullptr) {
                // Проверка на корректность указателя head
                return;
            }
            temp->head->size--;
            temp = temp->left;
            if (temp == nullptr) {
                // Проверка на корректность указателя left
                return;
            }
        }
    }
}

void recoverNode(Node* node) {
    if (node == nullptr || node->left == nullptr || node->right == nullptr || node->head == nullptr) {
        return; // Проверка на корректность инициализации узла и его указателей
    }

    if (node->up != nullptr) {
        for (Node* curr = node->up; curr != node; curr = curr->up) {
            if (curr != nullptr && curr->left != nullptr) {
                for (Node* temp = curr->left; temp != curr; temp = temp->left) {
                    if (temp != nullptr && temp->head != nullptr) {
                        // Проверка на корректность указателя head
                        temp->head->size++;
                    }
                }
            }
        }
    }

    if (node->head != nullptr) {
        // Проверка на корректность указателя head
        node->head->size++;
        node->left->right = node;
        node->right->left = node;
    }
}

// Функция для заполнения двумерного вектора случайными числами с использованием Dancing Links
std::vector<std::vector<long int>> generateDancingLinks(int h) {
    int size = h;
    int headerCount = 4 * size + 1; // Количество заголовков (строки, столбцы, квадраты, дополнительный заголовок)

    std::vector<Header> headers(headerCount); // Вектор заголовков

    // Проверка корректности инициализации заголовков
    for (const Header& header : headers) {
        if (header.head != nullptr) {
            // Обработка ошибки некорректной инициализации заголовков
            return {}; // Возвращаем пустой вектор
        }
    }

    // Создаем узлы и связываем их с соответствующими заголовками
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            for (int value = 1; value <= size; value++) {
                nodes.push_back(new Node(row, col, value)); // Создаем новый узел
                Node* node = nodes.back();

                // Связываем узел с заголовками строки, столбца и квадрата
                linkNode(headers[row], node);
                linkNode(headers[size + col], node);
                linkNode(headers[2 * size + (row / h) * h + col / h], node);
            }
        }
    }

    // Создаем дополнительный заголовок и связываем его со всеми узлами
    Header& rootHeader = headers.back();
    for (Node* node : nodes) {
        linkNode(rootHeader, node);
    }

    
    std::vector<std::vector<long int>> result(size, std::vector<long int>(size, 0));
    std::uniform_int_distribution<long int> dist(1, size);
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            std::vector<bool> used(size + 1, false); // Вектор для отслеживания использованных чисел
            int value;
            do {
                value = dist(gen);
            } while (used[value]); // Продолжаем генерацию, пока не найдем уникальное число
            result[row][col] = value;
            used[value] = true;

            // Удаляем узлы, конфликтующие с выбранным числом
            for (Node* node : nodes) {
                if ((node->row == row || node->col == col || 
                    (node->row / h) * h + node->col / h == (row / h) * h + col / h) 
                    && node->value != value) {
                    removeNode(node);
                }
            }
        }
    }

    // Восстанавливаем удаленные узлы
    for (Node* node : nodes) {
        recoverNode(node);
    }

    return result;
}