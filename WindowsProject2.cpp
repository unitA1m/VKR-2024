#include <windows.h>
#include <tchar.h>
#include <vector>

#include <fstream>
#include <sstream>

#include <Psapi.h>

#include "Resource.h"
#include "Diplom.h"
#include "functiondeletecells.h"

using namespace std;

#define ID_BTN_OPEN_WINDOW 9001
#define ID_BTN_ACTION2 9002
#define ID_BTN_OPEN_WINDOW2 9003
#define ID_BTN_EXIT 2
#define ID_BTN_SAVE 3
#define IDC_BUTTON_CHECK 4
#define CELL_WIDTH 40
#define CELL_HEIGHT 30
UINT_PTR g_timerID = 0; 

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SecondWndProc(HWND hChildWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ThirdWndProc(HWND hChildWnd2, UINT msg, WPARAM wParam, LPARAM lParam);

std::vector<std::vector<long int>> matrixData;
std::vector<std::vector< long int>> otherData;

int Diff = 1, Size = 9, numAlgor = 0, bestDifficulty = 0,bestSize = 0;
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
RECT windowRect = { 0, 0, 500, 300 };

int windowWidth = windowRect.right - windowRect.left;
int windowHeight = windowRect.bottom - windowRect.top;
int windowX = (screenWidth - windowWidth) / 2;
int windowY = (screenHeight - windowHeight) / 2;

int** userInputData;
HWND** hEditControls = nullptr;
int g_elapsedSeconds = 0;
HWND hTimerControl = nullptr;
double bestTime = 0.0;
long long MemoryUsageBefore, MemoryUsageAfter,MemoryUsageTotal;
 
long long getTotalMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<long long>(pmc.WorkingSetSize);
    }
    else {
        return 0LL; // Если не удалось получить информацию, возвращаем 0
    }
}

double readBestTime(int difficulty, int fieldSize) {
    string filename = "best_times.txt";
    ifstream file(filename);
    if (!file) {
        return 0.0;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int diff1, size1;
        double time;
        if (iss >> diff1 >> size1 >> time && diff1 == difficulty && size1 == fieldSize) {
            file.close();
            return time;
        }
    }

    file.close();
    return 0.0; 
}

void writeBestTime(int difficulty, int fieldSize, double time) {
    string filename = "best_times.txt";
    ofstream file(filename, ios::app);
    if (file) {
        file << difficulty << " " << fieldSize << " " << time << endl;
        file.close();
    }
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
    HWND hChildWnd, hChildWnd2;
    static HINSTANCE hInstance;
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    switch (umsg) {
    case WM_CREATE:
        hChildWnd = (HWND)lParam;
        hChildWnd2 = (HWND)lParam;
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        break;
    
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_OPEN_WINDOW:

            hChildWnd = CreateWindowEx(0, L"SecondWindowClass", L"Игра", WS_OVERLAPPEDWINDOW, windowX, windowY, windowWidth * 2 , windowHeight * 2, hwnd, NULL, hInstance, hwnd);

            if (!hChildWnd) {
                MessageBox(NULL, _T("Не удалось создать второе окно!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
            }
            else {
               
                ShowWindow(hChildWnd, SW_SHOW);
                ShowWindow(hwnd, SW_HIDE);
            }
            break;

        case ID_BTN_OPEN_WINDOW2:
            hChildWnd2 = CreateWindowEx(0, L"ThirdWindowClass", L"Настройки",WS_OVERLAPPEDWINDOW, windowX, windowY,windowWidth+100, windowHeight+50, hwnd, NULL, hInstance, hwnd);

            if (!hChildWnd2) {
                MessageBox(NULL, _T("Не удалось создать третье окно!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
            }
            else {
                CreateWindow(_T("BUTTON"), _T("В главное меню"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 200, 250, 30, hChildWnd2, (HMENU)ID_BTN_EXIT, hInstance, NULL);

                ShowWindow(hChildWnd2, SW_SHOW);
                ShowWindow(hwnd, SW_HIDE);
            }
            break;

        case ID_BTN_ACTION2:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_CLOSE: 
        ShowWindow(hwnd, SW_HIDE);
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        ShowWindow(hwnd, SW_HIDE); 
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, umsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK SecondWndProc(HWND hChildWnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
    static HWND hMainWnd, hButtonCheck = nullptr;
    static HINSTANCE hInstance;
    int NUM_ROWS = Size;
    int NUM_COLS = Size;
   
    double elapsedTime = 0;
   
    
    HWND** hEditControls1 = nullptr;
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    int x = 10, y = 10;
    int x1 = 10, y1 = 10;
    int num = 1;
    int textX = 10 + NUM_COLS * CELL_WIDTH + 20;
    int textY = 10;
    int sqrtH = 0; 
    TCHAR timeBuffer[100] = {0};
    LARGE_INTEGER startTime, endTime, freq;
    TCHAR bufferTimeCreate[100];
    double elapsedSeconds = 0;
    int sqrtSize = static_cast<int>(sqrt(Size));

    switch (umsg) {
    case WM_CREATE:
        g_elapsedSeconds = 0;
        elapsedSeconds = 0;
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        hMainWnd = (HWND)lParam; 
        hMainWnd = CreateWindowEx(0,_T("MainWindowClass"),_T("Главное окно"),WS_OVERLAPPEDWINDOW,windowX, windowY,windowWidth, windowHeight,NULL, NULL,hInstance,NULL);

        
        hEditControls = new HWND * [NUM_ROWS];
        for (int i = 0; i < NUM_ROWS; i++) {
            hEditControls[i] = new HWND[NUM_COLS];
            memset(hEditControls[i], 0, NUM_COLS * sizeof(HWND));
        }
        

        QueryPerformanceCounter(&startTime);
        MemoryUsageBefore= getTotalMemoryUsage();
        otherData = mains( Size,numAlgor);
        MemoryUsageAfter= getTotalMemoryUsage();
        MemoryUsageTotal = MemoryUsageAfter - MemoryUsageBefore;
        QueryPerformanceCounter(&endTime);
        QueryPerformanceFrequency(&freq);
        elapsedSeconds = static_cast<double>(endTime.QuadPart - startTime.QuadPart) / freq.QuadPart;
        _stprintf_s(bufferTimeCreate, _T("Время выполнения: %.6f секунд.Использование памяти: %d байт"), elapsedSeconds, MemoryUsageTotal);
        MessageBox(hMainWnd, bufferTimeCreate, _T("Информация"), MB_OK);
        matrixData =deleteRandomCells(otherData,Size,Diff);
        

        NUM_ROWS = Size;
        NUM_COLS = Size;
        
        for (int row = 0; row < NUM_ROWS; row++) {
            for (int col = 0; col < NUM_COLS; col++) {
                if (matrixData[row][col] == 0) {
                    hEditControls[row][col] = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, x, y, CELL_WIDTH, CELL_HEIGHT, hChildWnd, NULL, hInstance, NULL);
                }
                else {
                    hEditControls[row][col] = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, x, y, CELL_WIDTH, CELL_HEIGHT, hChildWnd, NULL, hInstance, NULL);
                    TCHAR buffer[3];
                    _itow_s(matrixData[row][col], buffer, 3, 10);
                    SetWindowText(hEditControls[row][col], buffer);
                }
                x += CELL_WIDTH;
            }
            x = 10;
            y += CELL_HEIGHT;
        }

        // Создание сетки
        x = 10;
        y = 10 + CELL_HEIGHT;
        for (int row = 0; row < NUM_ROWS; row++) {
            CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, x, y, NUM_COLS * CELL_WIDTH, 1, hChildWnd, NULL, hInstance, NULL);
            y += CELL_HEIGHT;
        }

        x = 10 + CELL_WIDTH;
        y = 10;
        for (int col = 0; col < NUM_COLS; col++) {
            CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ETCHEDVERT, x, y, 1, NUM_ROWS * CELL_HEIGHT, hChildWnd, NULL, hInstance, NULL);
            x += CELL_WIDTH;
        }

        x1 = 10;
        y1 = 10;
        for (int i = 0; i < NUM_ROWS; i += sqrtSize) {
            for (int j = 0; j < NUM_COLS; j += sqrtSize) {
                CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,x1 + j * CELL_WIDTH, y1 + i * CELL_HEIGHT,sqrtSize * CELL_WIDTH, sqrtSize * CELL_HEIGHT,hChildWnd, NULL, hInstance, NULL);
            }
        }
        
        TCHAR diffStr[10], sizeStr[10];
        _itow_s(Diff, diffStr, 10, 10);
        _itow_s(Size, sizeStr, 10, 10);

        CreateWindow(L"STATIC",L"Правила игры:",WS_CHILD | WS_VISIBLE,x+230, textY,200, 20,hChildWnd,NULL,hInstance,NULL);
        CreateWindow(L"STATIC",L"Сложность:", WS_CHILD | WS_VISIBLE, x + 30, textY, 200, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC",diffStr,WS_CHILD | WS_VISIBLE,x + 130, textY,50, 20,hChildWnd,NULL,hInstance,NULL);

        textY += 30;

        CreateWindow(L"STATIC",L"Размеры поля:",WS_CHILD | WS_VISIBLE,x+30, textY,200, 20,hChildWnd,NULL, hInstance,NULL);
        CreateWindow(L"STATIC", L"Числа не должны повторяться в строке,столбце", WS_CHILD | WS_VISIBLE, x + 230, textY, 450, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC", L",а так же в квадрате.", WS_CHILD | WS_VISIBLE, x + 230, textY+30, 450, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC", L"Числа могут быть от 1 до значения размеров поля.", WS_CHILD | WS_VISIBLE, x + 230,textY + 60, 450, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC", L"Например:если размер поля 9х9,то в квадрате 3х3", WS_CHILD | WS_VISIBLE, x + 230, textY+90, 450, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC", L" числа не должны повторяться.", WS_CHILD | WS_VISIBLE, x + 230, textY + 120, 450, 20, hChildWnd, NULL, hInstance, NULL);
        CreateWindow(L"STATIC",sizeStr,WS_CHILD | WS_VISIBLE,x + 160, textY,50, 20,hChildWnd,NULL,hInstance,NULL);

        hButtonCheck = CreateWindow(L"BUTTON",L"Проверить",WS_CHILD | WS_VISIBLE, x+30, textY+30, 100, 30, hChildWnd,(HMENU)IDC_BUTTON_CHECK,hInstance,NULL);

        CreateWindow(_T("BUTTON"), _T("Играть"), WS_VISIBLE | WS_CHILD, 100, 10, 250, 30, hMainWnd, (HMENU)ID_BTN_OPEN_WINDOW, hInstance, NULL);
        CreateWindow(_T("BUTTON"), _T("Настройки игры"), WS_VISIBLE | WS_CHILD, 100, 100, 250, 30, hMainWnd, (HMENU)ID_BTN_OPEN_WINDOW2, hInstance, NULL);
        CreateWindow(_T("BUTTON"), _T("Выход"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 200, 250, 30, hMainWnd, (HMENU)ID_BTN_ACTION2, hInstance, NULL);

        hTimerControl = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE, x + 30, textY + 60, 100, 20, hChildWnd, NULL, hInstance, NULL);

        bestTime = readBestTime(Diff, Size);
        bestDifficulty = Diff;
        bestSize = Size;

        TCHAR bestTimeStr[20];
        if (bestTime > 0.0) {
            _stprintf_s(bestTimeStr, 20, _T("%.2f сек"), bestTime);
        }
        else {
            _tcscpy_s(bestTimeStr, 20, _T("Нет результата"));
        }
        textY += 90;
        CreateWindow(L"STATIC", L"Лучшее время:", WS_CHILD | WS_VISIBLE, x + 30, textY, 200, 20, hChildWnd, NULL, hInstance, NULL);
        textY += 30;
        CreateWindow(L"STATIC", bestTimeStr, WS_CHILD | WS_VISIBLE, x + 30, textY, 200, 20, hChildWnd, NULL, hInstance, NULL);
        
        CreateWindow(_T("BUTTON"), _T("В главное меню"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x + 130, textY + 30, 200, 30, hChildWnd, (HMENU)ID_BTN_EXIT, hInstance, NULL);



        g_timerID = SetTimer(hChildWnd, 1, 1000, nullptr); 

        break;
    case WM_TIMER:
        switch (wParam) {
        case 1: 
        {
            g_elapsedSeconds++; 
            wsprintf(timeBuffer, _T("%02d:%02d:%02d"), g_elapsedSeconds / 3600, (g_elapsedSeconds % 3600) / 60, g_elapsedSeconds % 60);         
            SetWindowText(hTimerControl, timeBuffer);
        }
        break;
        }
        break;
    case WM_CLOSE:
        KillTimer(hChildWnd, g_timerID); 
        delete[] hEditControls;
        hEditControls = nullptr;
        ShowWindow(hMainWnd, SW_SHOW);
        UpdateWindow(hMainWnd);
        DestroyWindow(hChildWnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_EXIT:
            KillTimer(hChildWnd, g_timerID); 
            SendMessage(hChildWnd, WM_CLOSE, 0, 0);
            DestroyWindow(hChildWnd);
            break;
        case IDC_BUTTON_CHECK: {
            bool isValid = true;
            int row = 0;
            int col = 0;
            int i=0, j=0;
            for ( row = 0; row < Size; row++) {
                for (col = 0; col < Size; col++) {
                    TCHAR buffer[10];
                    GetWindowText(hEditControls[row][col], buffer, 10);
                    int value = _wtoi(buffer);
                    if (value != int(otherData[row][col])) {
                        isValid = false;
                        break;
                    }
                }
                if (!isValid) break;
            }
            if (isValid) {
                KillTimer(hChildWnd, g_timerID);
                elapsedTime = g_elapsedSeconds;
                if (elapsedTime < bestTime || bestTime == 0.0) {
                    writeBestTime(Diff, Size, elapsedTime);
                    bestTime = elapsedTime;
                }
                TCHAR message[200];
                
                wsprintf(message, _T("Поле заполнено правильно, молодец! Ваше время: %02d:%02d:%02d"), 
                    g_elapsedSeconds / 3600, (g_elapsedSeconds % 3600) / 60, g_elapsedSeconds % 60);
                MessageBox(hChildWnd, message, L"Проверка", MB_OK | MB_ICONINFORMATION);
                EnableWindow(hButtonCheck, FALSE);
            }
            else {
                MessageBox(hMainWnd, L"Поле содержит ошибки:(", L"Проверка", MB_OK);
            }
            break;
            }
        }
        break;
    case WM_DESTROY:
        KillTimer(hChildWnd, g_timerID); 
        delete[] hEditControls;
        hEditControls = nullptr;
        DestroyWindow(hChildWnd);
        break; 
    default:
        return DefWindowProc(hChildWnd, umsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ThirdWndProc(HWND hChildWnd2, UINT umsg, WPARAM wParam, LPARAM lParam) {
    static HWND hMainWnd, hwndDiffEdit, hwndSizeEdit,hwndAlgorithmCombo;
    static HINSTANCE hInstance;

    switch (umsg) {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        hMainWnd = (HWND)lParam; 
        hMainWnd = CreateWindowEx(0,_T("MainWindowClass"),_T("Главное окно"),WS_OVERLAPPEDWINDOW,windowX, windowY,windowWidth, windowHeight,NULL, NULL,hInstance,NULL);
        
        CreateWindow(_T("STATIC"), _T("Сложность(от 1 до 5):"), WS_CHILD | WS_VISIBLE, 10, 50, 250, 20, hChildWnd2, NULL, NULL, NULL);
        hwndDiffEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 50, 100, 20, hChildWnd2, NULL, NULL, NULL);

        CreateWindow(_T("STATIC"), _T("Размеры поля( 4, 9, 16, 25):"), WS_CHILD | WS_VISIBLE, 10, 80, 250, 20, hChildWnd2, NULL, NULL, NULL);
        hwndSizeEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 80, 100, 20, hChildWnd2, NULL, NULL, NULL);

        CreateWindow(_T("STATIC"), _T("Выберите алгоритм:"), WS_CHILD | WS_VISIBLE, 10, 110, 250, 20, hChildWnd2, NULL, NULL, NULL);
        hwndAlgorithmCombo = CreateWindow(_T("COMBOBOX"), NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 255, 110, 250, 200, hChildWnd2, NULL, NULL, NULL);
    
        SendMessage(hwndAlgorithmCombo, CB_ADDSTRING, 0, (LPARAM)_T("1 - Back traking"));
        SendMessage(hwndAlgorithmCombo, CB_ADDSTRING, 0, (LPARAM)_T("2 - Dancing links"));
        SendMessage(hwndAlgorithmCombo, CB_ADDSTRING, 0, (LPARAM)_T("3 - Genetic algorithm"));

        CreateWindow(_T("BUTTON"), _T("Сохранить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 140, 100, 30, hChildWnd2, (HMENU)ID_BTN_SAVE, hInstance, NULL);

        CreateWindow(_T("BUTTON"), _T("Играть"), WS_VISIBLE | WS_CHILD, 100, 10, 250, 30, hMainWnd, (HMENU)ID_BTN_OPEN_WINDOW, hInstance, NULL);
        CreateWindow(_T("BUTTON"), _T("Настройки игры"), WS_VISIBLE | WS_CHILD, 100, 100, 250, 30, hMainWnd, (HMENU)ID_BTN_OPEN_WINDOW2, hInstance, NULL);
        CreateWindow(_T("BUTTON"), _T("Выход"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 200, 250, 30, hMainWnd, (HMENU)ID_BTN_ACTION2, hInstance, NULL);

        break;

    case WM_CLOSE:
        ShowWindow(hMainWnd, SW_SHOW);
        UpdateWindow(hMainWnd);
        DestroyWindow(hChildWnd2);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_EXIT:
            SendMessage(hChildWnd2, WM_CLOSE, 0, 0);
            break;
        case ID_BTN_SAVE: {
            
            TCHAR diffStr[10], sizeStr[10];
            GetWindowText(hwndDiffEdit, diffStr, 10);
            GetWindowText(hwndSizeEdit, sizeStr, 10);
            Diff = _tstoi(diffStr);
            Size = _tstoi(sizeStr);
            numAlgor= SendMessage(hwndAlgorithmCombo, CB_GETCURSEL, 0, 0);

           
            TCHAR message[100];
            if (Diff > 0 && Size > 0) {
                _stprintf_s(message, _T("Значения сохранены: Сложность = %d, Размеры поля = %d"), Diff, Size);
            }
            else {
                _stprintf_s(message, _T("Некорректные значения. Введите положительные числа."));
            }
            MessageBox(hChildWnd2, message, _T("Сохранение настроек"), MB_OK | MB_ICONINFORMATION);
            break;
            }
        }
        break;
    case WM_DESTROY:
        ShowWindow(hMainWnd, SW_SHOW);
        UpdateWindow(hMainWnd);
        DestroyWindow(hChildWnd2);
        break;
    default:
        return DefWindowProc(hChildWnd2, umsg, wParam, lParam);
    }
    return 0;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Регистрация класса окна для главного окна
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("MainWindowClass");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Не удалось зарегистрировать класс главного окна!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Регистрация класса окна для второго окна
    WNDCLASSEX wcChild;
    wcChild.cbSize = sizeof(WNDCLASSEX);
    wcChild.style = CS_HREDRAW | CS_VREDRAW;
    wcChild.lpfnWndProc = SecondWndProc;
    wcChild.cbClsExtra = 0;
    wcChild.cbWndExtra = 0;
    wcChild.hInstance = hInstance;
    wcChild.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcChild.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcChild.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcChild.lpszMenuName = NULL;
    wcChild.lpszClassName = _T("SecondWindowClass");
    wcChild.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcChild)) {
        MessageBox(NULL, _T("Не удалось зарегистрировать класс второго окна!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Регистрация класса окна для третьего окна
    WNDCLASSEX wcChild2;
    wcChild2.cbSize = sizeof(WNDCLASSEX);
    wcChild2.style = CS_HREDRAW | CS_VREDRAW;
    wcChild2.lpfnWndProc = ThirdWndProc;
    wcChild2.cbClsExtra = 0;
    wcChild2.cbWndExtra = 0;
    wcChild2.hInstance = hInstance;
    wcChild2.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcChild2.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcChild2.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcChild2.lpszMenuName = NULL;
    wcChild2.lpszClassName = _T("ThirdWindowClass");
    wcChild2.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcChild2)) {
        MessageBox(NULL, _T("Не удалось зарегистрировать класс второго окна!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Создание главного окна
    hwnd = CreateWindowEx(0,_T("MainWindowClass"),_T("Главное окно"),
        WS_OVERLAPPEDWINDOW,windowX, windowY,windowWidth, windowHeight,NULL, NULL,hInstance,NULL);
    if (!hwnd) {
        MessageBox(NULL, _T("Не удалось создать главное окно!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hChildWnd = NULL, hChildWnd2 = NULL;

    CreateWindow(_T("BUTTON"), _T("Играть"), WS_VISIBLE | WS_CHILD, 100, 10, 250, 30, hwnd, (HMENU)ID_BTN_OPEN_WINDOW, hInstance, NULL);
    CreateWindow(_T("BUTTON"), _T("Настройки игры"), WS_VISIBLE | WS_CHILD, 100, 100, 250, 30, hwnd, (HMENU)ID_BTN_OPEN_WINDOW2, hInstance, NULL);
    CreateWindow(_T("BUTTON"), _T("Выход"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 200, 250, 30, hwnd, (HMENU)ID_BTN_ACTION2, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hChildWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

