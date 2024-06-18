#include <windows.h>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <cstdlib>

#define SIZE 5

// Heurística: número de reinas amenazadas
int heuristic(const std::vector<int>& queens) {
    int threats = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = i + 1; j < SIZE; ++j) {
            if (queens[i] == queens[j] || abs(queens[i] - queens[j]) == abs(i - j)) {
                threats++;
            }
        }
    }
    return threats;
}

// Genera estados vecinos moviendo una reina en la misma fila
std::vector<std::vector<int>> getNeighbors(const std::vector<int>& queens) {
    std::vector<std::vector<int>> neighbors;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (queens[i] != j) {
                std::vector<int> newQueens = queens;
                newQueens[i] = j;
                neighbors.push_back(newQueens);
            }
        }
    }
    return neighbors;
}

// Dibuja el tablero y las reinas
void drawBoard(HDC hdc, const std::vector<int>& queens) {
    RECT rect;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            rect.left = j * 100;
            rect.top = i * 100;
            rect.right = rect.left + 100;
            rect.bottom = rect.top + 100;

            // Cambia los colores del tablero como en la imagen
            HBRUSH brush = CreateSolidBrush(((i + j) % 2 == 0) ? RGB(233, 174, 95) : RGB(128, 128, 128));
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);

            // Dibuja las reinas
            if (queens[i] == j) {
                Ellipse(hdc, rect.left + 20, rect.top + 20, rect.right - 20, rect.bottom - 20);
            }
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::vector<int> initialQueens1 = {4, 0, 4, 0, 4};
    static std::vector<int> queens = initialQueens1; // Empezamos con initialQueens1
    static bool searching = false;
    static int solutions = 0;

    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            drawBoard(hdc, queens);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) { // ID del botón "Iniciar Búsqueda"
                searching = true;
                solutions = 0;
                SetTimer(hwnd, 1, 500, NULL); // Iniciar búsqueda al presionar el botón
            } else if (LOWORD(wParam) == 2) { // ID del botón "Reiniciar"
                queens = initialQueens1; // Reiniciar con la primera configuración inicial
                searching = false;
                InvalidateRect(hwnd, NULL, TRUE); // Redibujar el tablero
            }
            return 0;
        }
        case WM_TIMER: {
            if (!searching) {
                return 0;
            }
            int minHeuristic = heuristic(queens);
            std::vector<int> nextQueens = queens;

            for (const auto& neighbor : getNeighbors(queens)) {
                int h = heuristic(neighbor);
                if (h < minHeuristic) {
                    minHeuristic = h;
                    nextQueens = neighbor;
                }
            }

            if (minHeuristic == heuristic(queens)) {
                std::random_shuffle(nextQueens.begin(), nextQueens.end());
            }

            queens = nextQueens;
            InvalidateRect(hwnd, NULL, TRUE);

            if (minHeuristic == 0) {
                solutions++;
                KillTimer(hwnd, 1);
                searching = false;
                MessageBox(hwnd, (std::to_string(solutions) + " solución(es) encontrada(s)").c_str(), "Resultado", MB_OK);
            }

            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "FiveQueensClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Problema de las 5 Reinas",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, SIZE * 100 + 16, SIZE * 100 + 100, // Ajuste del tamaño de la ventana
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    // Crear botón "Iniciar Búsqueda"
    CreateWindow(
        "BUTTON",
        "Iniciar Búsqueda",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, SIZE * 100 + 10,
        150, 30,
        hwnd,
        (HMENU)1,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);

    // Crear botón "Reiniciar"
    CreateWindow(
        "BUTTON",
        "Reiniciar",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        170, SIZE * 100 + 10,
        150, 30,
        hwnd,
        (HMENU)2,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
