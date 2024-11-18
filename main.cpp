

#include <windows.h>
#include "Diplom.h"

// �������� ������� ����������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
_Success_(TRUE) int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    // ����������� ������ ����
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("MyWindowClass");
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("�� ������� ���������������� ����� ����!"), TEXT("������"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // �������� ����
    hwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        TEXT("MyWindowClass"),
        TEXT("�� Windows-����"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 300,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd)
    {
        MessageBox(NULL, TEXT("�� ������� ������� ����!"), TEXT("������"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // ����������� ����
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // ���� ��������� ���������
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// ���������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}