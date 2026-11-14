#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawTemple(HDC hdc);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE,
                   LPSTR,
                   int nCmdShow)
{
    WNDCLASSA wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TempleWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "TempleWindow",
        "Colourful Temple",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1000,
        700,
        NULL,
        NULL,
        hInstance,
        NULL);

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

void DrawTemple(HDC hdc)
{
    // Sky
    HBRUSH sky = CreateSolidBrush(RGB(135,206,250));
    RECT skyRect = {0,0,1000,700};
    FillRect(hdc,&skyRect,sky);
    DeleteObject(sky);

    // Sun
    HBRUSH sun = CreateSolidBrush(RGB(255,215,0));
    SelectObject(hdc,sun);
    Ellipse(hdc,70,50,170,150);
    DeleteObject(sun);

    // Grass
    HBRUSH grass = CreateSolidBrush(RGB(50,180,50));
    RECT grassRect = {0,500,1000,700};
    FillRect(hdc,&grassRect,grass);
    DeleteObject(grass);

    // Temple Base
    HBRUSH baseBrush =
        CreateSolidBrush(RGB(255,228,181));
    SelectObject(hdc,baseBrush);

    Rectangle(hdc,250,250,750,500);

    DeleteObject(baseBrush);

    // Main Dome
    HBRUSH domeBrush =
        CreateSolidBrush(RGB(255,140,0));

    SelectObject(hdc,domeBrush);

    Pie(
        hdc,
        350,
        80,
        650,
        330,
        350,
        210,
        650,
        210);

    DeleteObject(domeBrush);

    // Left Dome
    HBRUSH leftDome =
        CreateSolidBrush(RGB(255,99,71));

    SelectObject(hdc,leftDome);

    Pie(
        hdc,
        240,
        150,
        380,
        300,
        240,
        225,
        380,
        225);

    DeleteObject(leftDome);

    // Right Dome
    HBRUSH rightDome =
        CreateSolidBrush(RGB(255,99,71));

    SelectObject(hdc,rightDome);

    Pie(
        hdc,
        620,
        150,
        760,
        300,
        620,
        225,
        760,
        225);

    DeleteObject(rightDome);

    // Temple Door
    HBRUSH door =
        CreateSolidBrush(RGB(139,69,19));

    SelectObject(hdc,door);

    Rectangle(
        hdc,
        450,
        330,
        550,
        500);

    DeleteObject(door);

    // Stairs
    HBRUSH stair =
        CreateSolidBrush(RGB(180,180,180));

    SelectObject(hdc,stair);

    Rectangle(hdc,390,500,610,530);
    Rectangle(hdc,370,530,630,560);
    Rectangle(hdc,350,560,650,590);

    DeleteObject(stair);

    // Pillars
    HBRUSH pillar =
        CreateSolidBrush(RGB(255,248,220));

    SelectObject(hdc,pillar);

    Rectangle(hdc,290,280,330,500);
    Rectangle(hdc,380,280,420,500);
    Rectangle(hdc,580,280,620,500);
    Rectangle(hdc,670,280,710,500);

    DeleteObject(pillar);

    // Temple Flag Pole
    HPEN polePen =
        CreatePen(PS_SOLID,4,RGB(0,0,0));

    SelectObject(hdc,polePen);

    MoveToEx(hdc,500,80,NULL);
    LineTo(hdc,500,10);

    DeleteObject(polePen);

    // Flag
    HBRUSH flag =
        CreateSolidBrush(RGB(255,69,0));

    SelectObject(hdc,flag);

    POINT pts[3] =
    {
        {500,20},
        {580,45},
        {500,70}
    };

    Polygon(hdc,pts,3);

    DeleteObject(flag);

    // Decorative Windows
    HBRUSH winBrush =
        CreateSolidBrush(RGB(0,191,255));

    SelectObject(hdc,winBrush);

    Ellipse(hdc,320,330,360,380);
    Ellipse(hdc,640,330,680,380);

    DeleteObject(winBrush);

    // Temple Name
    SetBkMode(hdc,TRANSPARENT);
    SetTextColor(hdc,RGB(255,0,0));

    HFONT font =
        CreateFontA(
            32,0,0,0,
            FW_BOLD,
            FALSE,FALSE,FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH,
            "Arial");

    SelectObject(hdc,font);

    TextOutA(
        hdc,
        380,
        620,
        "COLOURFUL TEMPLE",
        16);

    DeleteObject(font);
}

LRESULT CALLBACK WndProc(HWND hwnd,
                         UINT msg,
                         WPARAM wParam,
                         LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);

            DrawTemple(hdc);

            EndPaint(hwnd,&ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(
        hwnd,
        msg,
        wParam,
        lParam);
}
