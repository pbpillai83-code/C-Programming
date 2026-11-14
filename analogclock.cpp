#include <windows.h>
#include <cmath>

const double PI = 3.14159265358979323846;
const UINT TIMER_ID = 1;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawClock(HDC hdc, RECT rc)
{
    // Dark background
    HBRUSH bg = CreateSolidBrush(RGB(18,18,18));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    int width  = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int cx = width / 2;
    int cy = height / 2 - 40;
    int radius = std::min(width, height) / 2 - 80;

    SetBkMode(hdc, TRANSPARENT);

    // Outer border
    HPEN borderPen = CreatePen(PS_SOLID, 4, RGB(0,255,255));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);

    Ellipse(
        hdc,
        cx - radius,
        cy - radius,
        cx + radius,
        cy + radius);

    DeleteObject(borderPen);

    // Tick marks
    HPEN tickPen = CreatePen(PS_SOLID, 1, RGB(0,180,255));
    SelectObject(hdc, tickPen);

    for(int i = 0; i < 60; ++i)
    {
        double angle = (i * 6 - 90) * PI / 180.0;

        int inner = (i % 5 == 0)
                  ? radius - 18
                  : radius - 8;

        int x1 = cx + (int)(inner * cos(angle));
        int y1 = cy + (int)(inner * sin(angle));

        int x2 = cx + (int)(radius * cos(angle));
        int y2 = cy + (int)(radius * sin(angle));

        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);
    }

    DeleteObject(tickPen);

    // Roman numerals
    const char* roman[] =
    {
        "XII","I","II","III",
        "IV","V","VI","VII",
        "VIII","IX","X","XI"
    };

    HFONT numeralFont = CreateFontA(
        28,0,0,0,
        FW_BOLD,
        FALSE,FALSE,FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH,
        "Segoe UI");

    HFONT oldFont =
        (HFONT)SelectObject(hdc, numeralFont);

    SetTextColor(hdc, RGB(0,255,255));

    for(int i = 0; i < 12; ++i)
    {
        double angle = (i * 30 - 90) * PI / 180.0;

        int tx =
            cx + (int)((radius - 50) * cos(angle));

        int ty =
            cy + (int)((radius - 50) * sin(angle));

        SIZE sz;

        GetTextExtentPoint32A(
            hdc,
            roman[i],
            lstrlenA(roman[i]),
            &sz);

        TextOutA(
            hdc,
            tx - sz.cx/2,
            ty - sz.cy/2,
            roman[i],
            lstrlenA(roman[i]));
    }

    SelectObject(hdc, oldFont);
    DeleteObject(numeralFont);

    // Get system time
    SYSTEMTIME st;
    GetLocalTime(&st);

    int hour   = st.wHour % 12;
    int minute = st.wMinute;
    int second = st.wSecond;

    double hourAngle =
        ((hour + minute / 60.0) * 30 - 90)
        * PI / 180.0;

    double minuteAngle =
        ((minute + second / 60.0) * 6 - 90)
        * PI / 180.0;

    double secondAngle =
        (second * 6 - 90)
        * PI / 180.0;

    // Hour hand
    HPEN hourPen =
        CreatePen(PS_SOLID, 8, RGB(255,255,255));

    SelectObject(hdc, hourPen);

    MoveToEx(hdc, cx, cy, NULL);

    LineTo(
        hdc,
        cx + (int)(radius * 0.50 * cos(hourAngle)),
        cy + (int)(radius * 0.50 * sin(hourAngle)));

    DeleteObject(hourPen);

    // Minute hand
    HPEN minutePen =
        CreatePen(PS_SOLID, 5, RGB(0,255,255));

    SelectObject(hdc, minutePen);

    MoveToEx(hdc, cx, cy, NULL);

    LineTo(
        hdc,
        cx + (int)(radius * 0.75 * cos(minuteAngle)),
        cy + (int)(radius * 0.75 * sin(minuteAngle)));

    DeleteObject(minutePen);

    // Second hand
    HPEN secondPen =
        CreatePen(PS_SOLID, 2, RGB(255,140,0));

    SelectObject(hdc, secondPen);

    MoveToEx(hdc, cx, cy, NULL);

    LineTo(
        hdc,
        cx + (int)(radius * 0.90 * cos(secondAngle)),
        cy + (int)(radius * 0.90 * sin(secondAngle)));

    DeleteObject(secondPen);

    // Center hub
    HBRUSH hub =
        CreateSolidBrush(RGB(0,255,255));

    HBRUSH oldBrush =
        (HBRUSH)SelectObject(hdc, hub);

    Ellipse(
        hdc,
        cx - 8,
        cy - 8,
        cx + 8,
        cy + 8);

    SelectObject(hdc, oldBrush);
    DeleteObject(hub);

    // Digital clock
    char timeBuffer[32];

    wsprintfA(
        timeBuffer,
        "%02d:%02d:%02d",
        st.wHour,
        st.wMinute,
        st.wSecond);

    HFONT digitalFont =
        CreateFontA(
            36,0,0,0,
            FW_BOLD,
            FALSE,FALSE,FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH,
            "Consolas");

    HFONT oldDigital =
        (HFONT)SelectObject(hdc, digitalFont);

    SetTextColor(hdc, RGB(0,255,0));

    SIZE txt;

    GetTextExtentPoint32A(
        hdc,
        timeBuffer,
        lstrlenA(timeBuffer),
        &txt);

    TextOutA(
        hdc,
        cx - txt.cx / 2,
        cy + radius + 25,
        timeBuffer,
        lstrlenA(timeBuffer));

    SelectObject(hdc, oldDigital);
    DeleteObject(digitalFont);

    SelectObject(hdc, oldPen);
}

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            SetTimer(hwnd, TIMER_ID, 1000, NULL);
            return 0;

        case WM_TIMER:
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc =
                BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            DrawClock(hdc, rc);

            EndPaint(hwnd, &ps);

            return 0;
        }

        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ID);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(
        hwnd,
        msg,
        wParam,
        lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPSTR,
    int nCmdShow)
{
    WNDCLASSEXA wc = {};

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = "CyberClockClass";

    if(!RegisterClassExA(&wc))
        return 0;

    HWND hwnd =
        CreateWindowExA(
            0,
            "CyberClockClass",
            "Cyber Analog Clock",
            WS_OVERLAPPEDWINDOW,
            100,
            50,
            800,
            850,
            NULL,
            NULL,
            hInstance,
            NULL);

    if(!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
