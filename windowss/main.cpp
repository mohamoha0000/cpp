#include <windows.h>

// دالة المعالجة الرئيسية للرسائل
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutA(hdc, 50, 50, "Hello, Windows API!", 19);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// نقطة الدخول الرئيسية للبرنامج
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SimpleWindowClass";

    // تسجيل صنف النافذة
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    // إنشاء النافذة
    HWND hwnd = CreateWindowExA(
        0,                      // أنماط إضافية
        CLASS_NAME,            // اسم الصنف
        "Simple Win32 App",    // عنوان النافذة
        WS_OVERLAPPEDWINDOW,   // نوع النافذة
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    // حلقة الرسائل
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

//g++ main.cpp -o main.exe -mwindows 
