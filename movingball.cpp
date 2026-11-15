#include <windows.h>

int ballX = 100;
int ballY = 100;
int dx = 5;
int dy = 5;

LRESULT CALLBACK WndProc(HWND hwnd,
                         UINT msg,
                         WPARAM wParam,
                         LPARAM lParam)
{
    switch(msg)
    {
        case WM_TIMER:
            ballX += dx;
            ballY += dy;

            if(ballX < 0 || ballX > 740) dx = -dx;
            if(ballY < 0 || ballY > 540) dy = -dy;

            InvalidateRect(hwnd,NULL,TRUE);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);

            Ellipse(hdc,ballX,ballY,
                    ballX+50,ballY+50);

            EndPaint(hwnd,&ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
}

int main()
{
    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "BallClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "BallClass",
        "Moving Ball",
        WS_OVERLAPPEDWINDOW,
        100,100,800,600,
        NULL,NULL,hInst,NULL);

    ShowWindow(hwnd,SW_SHOW);

    SetTimer(hwnd,1,20,NULL);

    MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
