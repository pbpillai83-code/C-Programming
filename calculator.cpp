#include <windows.h>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

HWND hEdit;

double firstNum = 0.0;
char operation = '\0';

#define IDC_EDIT   100
#define IDC_BTN0   101
#define IDC_BTN1   102
#define IDC_BTN2   103
#define IDC_BTN3   104
#define IDC_BTN4   105
#define IDC_BTN5   106
#define IDC_BTN6   107
#define IDC_BTN7   108
#define IDC_BTN8   109
#define IDC_BTN9   110
#define IDC_ADD    111
#define IDC_SUB    112
#define IDC_MUL    113
#define IDC_DIV    114
#define IDC_EQUAL  115
#define IDC_CLEAR  116
#define IDC_SIN    117
#define IDC_COS    118
#define IDC_TAN    119
#define IDC_SQRT   120
#define IDC_SQR    121
#define IDC_LOG    122
#define IDC_DOT    123

string GetEditText()
{
    char buffer[256];
    GetWindowTextA(hEdit, buffer, sizeof(buffer));
    return string(buffer);
}

void SetEditText(const string& text)
{
    SetWindowTextA(hEdit, text.c_str());
}

double ToDouble(const string& s)
{
    stringstream ss(s);
    double value = 0;
    ss >> value;
    return value;
}

string ToString(double value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
                         WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            hEdit = CreateWindowA(
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
                20,20,340,35,
                hwnd,
                (HMENU)IDC_EDIT,
                NULL,
                NULL);

            struct ButtonInfo
            {
                const char* text;
                int id;
                int x;
                int y;
            };

            ButtonInfo buttons[] =
            {
                {"7",IDC_BTN7,20,80},
                {"8",IDC_BTN8,90,80},
                {"9",IDC_BTN9,160,80},
                {"/",IDC_DIV,230,80},

                {"4",IDC_BTN4,20,130},
                {"5",IDC_BTN5,90,130},
                {"6",IDC_BTN6,160,130},
                {"*",IDC_MUL,230,130},

                {"1",IDC_BTN1,20,180},
                {"2",IDC_BTN2,90,180},
                {"3",IDC_BTN3,160,180},
                {"-",IDC_SUB,230,180},

                {"0",IDC_BTN0,20,230},
                {".",IDC_DOT,90,230},
                {"=",IDC_EQUAL,160,230},
                {"+",IDC_ADD,230,230},

                {"sin",IDC_SIN,20,290},
                {"cos",IDC_COS,90,290},
                {"tan",IDC_TAN,160,290},
                {"sqrt",IDC_SQRT,230,290},

                {"x^2",IDC_SQR,20,340},
                {"log",IDC_LOG,90,340},
                {"C",IDC_CLEAR,160,340}
            };

            for(size_t i=0;i<sizeof(buttons)/sizeof(buttons[0]);i++)
            {
                CreateWindowA(
                    "BUTTON",
                    buttons[i].text,
                    WS_CHILD | WS_VISIBLE,
                    buttons[i].x,
                    buttons[i].y,
                    60,
                    40,
                    hwnd,
                    (HMENU)(INT_PTR)buttons[i].id,
                    NULL,
                    NULL);
            }
            return 0;
        }

        case WM_COMMAND:
        {
            int id = LOWORD(wParam);

            if(id >= IDC_BTN0 && id <= IDC_BTN9)
            {
                string s = GetEditText();

                switch(id)
                {
                    case IDC_BTN0: s += "0"; break;
                    case IDC_BTN1: s += "1"; break;
                    case IDC_BTN2: s += "2"; break;
                    case IDC_BTN3: s += "3"; break;
                    case IDC_BTN4: s += "4"; break;
                    case IDC_BTN5: s += "5"; break;
                    case IDC_BTN6: s += "6"; break;
                    case IDC_BTN7: s += "7"; break;
                    case IDC_BTN8: s += "8"; break;
                    case IDC_BTN9: s += "9"; break;
                }

                SetEditText(s);
            }

            else if(id == IDC_DOT)
            {
                SetEditText(GetEditText() + ".");
            }

            else if(id == IDC_CLEAR)
            {
                SetEditText("");
                firstNum = 0;
                operation = '\0';
            }

            else if(id == IDC_ADD ||
                    id == IDC_SUB ||
                    id == IDC_MUL ||
                    id == IDC_DIV)
            {
                firstNum = ToDouble(GetEditText());

                if(id == IDC_ADD) operation = '+';
                if(id == IDC_SUB) operation = '-';
                if(id == IDC_MUL) operation = '*';
                if(id == IDC_DIV) operation = '/';

                SetEditText("");
            }

            else if(id == IDC_EQUAL)
            {
                double secondNum = ToDouble(GetEditText());
                double result = 0;

                switch(operation)
                {
                    case '+':
                        result = firstNum + secondNum;
                        break;

                    case '-':
                        result = firstNum - secondNum;
                        break;

                    case '*':
                        result = firstNum * secondNum;
                        break;

                    case '/':
                        if(secondNum != 0)
                            result = firstNum / secondNum;
                        else
                        {
                            MessageBoxA(
                                hwnd,
                                "Division by zero",
                                "Error",
                                MB_OK);
                            return 0;
                        }
                }

                SetEditText(ToString(result));
            }

            else if(id == IDC_SIN)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(sin(n)));
            }

            else if(id == IDC_COS)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(cos(n)));
            }

            else if(id == IDC_TAN)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(tan(n)));
            }

            else if(id == IDC_SQRT)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(sqrt(n)));
            }

            else if(id == IDC_SQR)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(n*n));
            }

            else if(id == IDC_LOG)
            {
                double n = ToDouble(GetEditText());
                SetEditText(ToString(log10(n)));
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE,
                   LPSTR,
                   int nCmdShow)
{
    WNDCLASSEXA wc = {0};

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "CalculatorClass";

    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        "CalculatorClass",
        "Scientific Calculator",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        400,
        470,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
