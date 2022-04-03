// lab_4.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab_4.h"
#include <math.h>
//#include <conio.h>

#define MAX_LOADSTRING 100

SYSTEMTIME s;

//Ручки:)
HPEN clock_Pen = CreatePen(PS_SOLID, 30, RGB(0, 0, 0));
HPEN min_Pen = CreatePen(PS_SOLID, 20, RGB(0, 0, 0));
HPEN sec_Pen = CreatePen(PS_SOLID, 10, RGB(0, 0, 0));
HPEN Main_PEN;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
RECT Main;
HWND hWnd;

//Координаты середины окна и длина стрелок и градус отклонения;
int x, y;
int c_radius, c_grad; 
int m_radius, m_grad;
int s_radius, s_grad;
double Ex, Ey;
int wheeldelta = 0;
bool is_Killed = false;

double ofx, ofy; //для минутной стрелки
double cofx, cofy; //для часовой

HDC hdc;
HDC memDC;
HBITMAP memBTM;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL Line(HDC hdc, int x1, int y1, int x2, int y2);              //Рисует линию(стрелку)
BOOL setDigits(HDC hdc, int x, int y, LPCWSTR text, int lenght); //Рисует цифру на циферблате
void setLines(HDC hdc, int i);
void setLines_sm(HDC hdc, int i);
void MoveTime(HDC hdc);                                          //Перерисовывет каждую секунду время.
void CALLBACK Timer_Proc(HWND hWnd, UINT msg, UINT idTimer, DWORD dwTime); //Процедура таймера
void setTime_2(HDC hdc);                                         //Устанавливает часы в исходное положение + рисует циферблат
void MoveForward(HDC hdc);                                       //Перевод часов на минуту вперед
void MoveBack(HDC hdc);                                          //Перевод часов на минуту назад
void Draw(void Move(HDC hdc));                                   //Рисует все это дело(Речь про операции связаные с переводом часов)

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4));
    SetTimer(hWnd, 1, 1000, (TIMERPROC)Timer_Proc);
    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            //Logger(msg, msgCodes);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    KillTimer(hWnd, 1);
    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+2);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      100, 100, 800, 840, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_NORMAL);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    PAINTSTRUCT ps;
    //HDC hdc;
   
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps); //Это будет контекст фона
        //Получение размеров клиентской области окна.
        GetClientRect(hWnd, &Main);

        //Координаты точки середины клиентской области окна
        //Также являются центром окружности.
        x = (Main.left + Main.right) / 2;
        y = (Main.top + Main.bottom) / 2;
 
        Ellipse(hdc, Main.left+5, Main.top+5, Main.right-5, Main.bottom-5); //Буду использовать как ось вращения стрелок.

        //Отрисовываем основу циферблата
        setDigits(hdc, x - 80, Main.top + 40, L"12", 2);
        setDigits(hdc, (Main.right - 80) - 30, (Main.bottom / 2 - 80 / 2), L"3", 1);
        setDigits(hdc, x - 80 / 2-5, (Main.bottom - 80) - 25, L"6", 1);
        setDigits(hdc, Main.left + 20, (Main.bottom / 2 - 80 / 2), L"9", 1);
        
        for (int i = 1; i < 13; ++i)
        {
            if (i == 3 || i == 6 || i == 9 || i == 12)
                continue;

            setLines(hdc, i);
        }

        for (int i = 1; i < 61; ++i)
        {
            if (i % 5 == 0)
                continue;
            setLines_sm(hdc, i);
        }
       
        //Устанавливаем стрелки согласно системному времени.
        setTime_2(hdc);
     
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        if (is_Killed)
        {
            if ((wheeldelta = (int)wParam) > 0)
            {
                Draw(MoveForward);
            }
            else
            {
                Draw(MoveBack);
            }
        }
        break;
    }
    case WM_KEYDOWN:
    {
        switch(wParam)
        {
        case VK_SPACE:
        {
            if (is_Killed)
            {
                is_Killed = false;
                SetTimer(hWnd, 1, 1000, (TIMERPROC)Timer_Proc);
            }
            else
            {
                KillTimer(hWnd, 1); //Ликвидирую таймер после удара по кнопке "ПРОБЕЛ".
                is_Killed = true;
            }
            break;
        }
        case VK_RIGHT:
        {
            if (is_Killed)
            {
                Draw(MoveForward);
                break;
            }
            break;
        }
        case VK_LEFT:
        {
            if(is_Killed)
                Draw(MoveBack);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
    return LineTo(hdc, x2, y2);
}

BOOL setDigits(HDC hdc, int x, int y, LPCWSTR text, int lenght)
{
    LOGFONT lf; //для шрифта
    HFONT hFont; //для шрифта

    lf.lfHeight = 80;
    lf.lfItalic = 0;
    lf.lfStrikeOut = 0;
    lf.lfUnderline = 0;
    lf.lfWidth = 80;
    lf.lfWeight = 700;
    lf.lfCharSet = DEFAULT_CHARSET; //значение по умолчанию
    lf.lfPitchAndFamily = DEFAULT_PITCH; //значения по умолчанию
    lf.lfEscapement = 0;

    hFont = CreateFontIndirect(&lf);
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    return TextOut(hdc, x, y, text, lenght);
    //Шрифт конец
}

void MoveTime(HDC hdc)
{
    GetSystemTime(&s);

    //Стрелка часовая.
    Main_PEN = (HPEN)SelectObject(hdc, clock_Pen);
  
    Ex = cofx * cos(0.000144862) - cofy * sin(0.000144862);  //Вычисляем x точки в которую переходит стрелка при *таком-то* угле поворота
    Ey = cofx * sin(0.000144862) + cofy * cos(0.000144862);  //Вычисляем у точки в которую переходит стрелка при *таком-то* угле поворота

    cofx = Ex; //Сохраняем старую точку, чтоб на след 
    cofy = Ey; //тике таймера посчитать новую и сохранить сюда.

    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    //Минутная
    Main_PEN = (HPEN)SelectObject(hdc, min_Pen);
    
    Ex = ofx * cos(0.00174533) - ofy * sin(0.00174533);  //Вычисляем x точки в которую переходит стрелка при *таком-то* угле поворота
    Ey = ofx * sin(0.00174533) + ofy * cos(0.00174533);  //Вычисляем у точки в которую переходит стрелка при *таком-то* угле поворота

    ofx = Ex; //Сохраняем старую точку, чтоб на след 
    ofy = Ey; //тике таймера посчитать новую и сохранить сюда.
    
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    //Секундная
    Main_PEN = (HPEN)SelectObject(hdc, sec_Pen);
    s_radius = 150;
    s_grad = s.wSecond - 15;
    Ex = s_radius * cos(0.10472 * s_grad);
    Ey = s_radius * sin(0.10472 * s_grad);

    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);
}

void setLines_sm(HDC hdc, int i)
{
    int rad = (Main.right - 50) / 2;

    double Lx = (rad - 15) * cos(0.10472 * i);
    double Ly = (rad - 15) * sin(0.10472 * i);

    double Bx = rad * cos(0.10472 * i);
    double By = rad * sin(0.10472 * i);

    Main_PEN = (HPEN)SelectObject(hdc, sec_Pen);

    Line(hdc, x + (int)Bx, y + (int)By, x + (int)Lx, y + (int)Ly);
}

void setLines(HDC hdc,int i)
{
    int rad = (Main.right - 50) / 2;

    double Lx = (rad-50) * cos(0.523599*i);
    double Ly = (rad-50)* sin(0.523599*i);

    double Bx = rad * cos(0.523599*i);
    double By = rad * sin(0.523599*i);

    Main_PEN = (HPEN)SelectObject(hdc, min_Pen);

    Line(hdc, x+(int)Bx ,y+(int)By, x+(int)Lx, y+(int)Ly);
}

void setTime_2(HDC hdc)
{
    GetSystemTime(&s);
  
    //Секундная
    Main_PEN = (HPEN)SelectObject(hdc, sec_Pen);
    s_radius = 150;
    s_grad = s.wSecond - 15;  //Если что - вернуть "-15"!!!!!!!!!!!!!!!!!!!!!
    Ex = s_radius * cos(0.10472 * s_grad);
    Ey = s_radius * sin(0.10472 * s_grad);
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    //Минутная
    Main_PEN = (HPEN)SelectObject(hdc, min_Pen);
    m_radius = 150;
    m_grad = s.wMinute - 15;
    Ex = m_radius * cos(0.10472 * m_grad);
    Ey = m_radius * sin(0.10472 * m_grad);

    double xof = Ex * cos(0.00174533 * s.wSecond) - Ey * sin(0.00174533 * s.wSecond);
    double yof = Ex * sin(0.00174533 * s.wSecond) + Ey * cos(0.00174533 * s.wSecond);

    Line(hdc, x, y, x + (int)xof, y + (int)yof);

    //
    ofx = xof;  //После установки стрелок запоминаем точку  
    ofy = yof;  //конца стрелки, чтоб потом продолжать движение.
    //

    //Стрелка часовая.
    Main_PEN = (HPEN)SelectObject(hdc, clock_Pen);
    c_radius = 75;
    c_grad = s.wHour;
    Ex = c_radius * cos(0.523599 * c_grad);
    Ey = c_radius * sin(0.523599 * c_grad);

    xof = Ex * cos(0.00872665 * s.wMinute) - Ey * sin(0.00872665 * s.wMinute);
    yof = Ex * sin(0.00872665 * s.wMinute) + Ey * cos(0.00872665 * s.wMinute);

    Line(hdc, x, y, x + (int)xof, y + (int)yof);

    cofx = xof;
    cofy = yof; 
}

void CALLBACK Timer_Proc(HWND hWnd, UINT msg, UINT idTimer, DWORD dwTime)
{
    hdc = GetDC(hWnd);

    memDC = CreateCompatibleDC(hdc);
    memBTM = CreateCompatibleBitmap(hdc, 800, 800);

    HPEN newPen = CreatePen(PS_SOLID, 15, RGB(255, 255, 255));
    HBRUSH newBrush = CreateSolidBrush(RGB(255, 255, 255));
    HGDIOBJ oldBTM = SelectObject(memDC, memBTM);

    HGDIOBJ oldPen = SelectObject(memDC, newPen);
    HGDIOBJ oldBrush = SelectObject(memDC, newBrush);

    Rectangle(memDC, x-200, y-200, (x-200)+800, (y-200)+800 );

    SelectObject(memDC, oldPen);

    Ellipse(memDC, Main.left + 5, Main.top + 5, Main.right - 5, Main.bottom - 5); 

    MoveTime(memDC);
    //////////////////////////

    BitBlt(hdc, x - 200, y - 200, 400, 400, memDC, x-200, y-200, SRCCOPY);

    SelectObject(hdc, oldBTM);
    SelectObject(hdc, oldBrush);
    

    DeleteObject(newPen);
    DeleteObject(newBrush);
    DeleteObject(memBTM);
    DeleteDC(memDC);

    ReleaseDC(hWnd, hdc);
}

void MoveForward(HDC hdc)
{
    GetSystemTime(&s);
    //clock
    Main_PEN = (HPEN)SelectObject(hdc, clock_Pen);

    Ex = cofx * cos(0.00872665) - cofy * sin(0.00872665);
    Ey = cofx * sin(0.00872665) + cofy * cos(0.00872665);
    
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    cofx = Ex; //Сохраняем старую точку, чтоб на след 
    cofy = Ey; //тике таймера посчитать новую и сохранить сюда.

    //Минутная
    Main_PEN = (HPEN)SelectObject(hdc, min_Pen);
    
    Ex = ofx * cos(0.10472) - ofy * sin(0.10472);
    Ey = ofx * sin(0.10472) + ofy * cos(0.10472);
    
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    ofx = Ex; //Сохраняем старую точку, чтоб на след 
    ofy = Ey; //тике таймера посчитать новую и сохранить сюда.

    //Секундная
    Main_PEN = (HPEN)SelectObject(hdc, sec_Pen);
   
    s_radius = 150;
    s_grad = s.wSecond - 15;
    Ex = s_radius * cos(0.10472 * s_grad);
    Ey = s_radius * sin(0.10472 * s_grad);
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);
}

void MoveBack(HDC hdc)
{
    GetSystemTime(&s);
    //clock
    Main_PEN = (HPEN)SelectObject(hdc, clock_Pen);

    Ex = cofx * cos(-0.00872665) - cofy * sin(-0.00872665);
    Ey = cofx * sin(-0.00872665) + cofy * cos(-0.00872665);

    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    cofx = Ex; //Сохраняем старую точку, чтоб на след 
    cofy = Ey; //тике таймера посчитать новую и сохранить сюда.

    //Минутная
    Main_PEN = (HPEN)SelectObject(hdc, min_Pen);

    Ex = ofx * cos(-0.10472) - ofy * sin(-0.10472);
    Ey = ofx * sin(-0.10472) + ofy * cos(-0.10472);

    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);

    ofx = Ex; //Сохраняем старую точку, чтоб на след 
    ofy = Ey; //тике таймера посчитать новую и сохранить сюда.

    //Секундная
    Main_PEN = (HPEN)SelectObject(hdc, sec_Pen);

    s_radius = 150;
    s_grad = s.wSecond - 15;
    Ex = s_radius * cos(0.10472 * s_grad);
    Ey = s_radius * sin(0.10472 * s_grad);
    Line(hdc, x, y, x + (int)Ex, y + (int)Ey);
}

void Draw(void Move(HDC hdc))
{
    hdc = GetDC(hWnd);

    memDC = CreateCompatibleDC(hdc);
    memBTM = CreateCompatibleBitmap(hdc, Main.right, Main.bottom);

    HPEN newPen = CreatePen(PS_SOLID, 15, RGB(255, 255, 255));
    HBRUSH newBrush = CreateSolidBrush(RGB(255, 255, 255));
    HGDIOBJ oldBTM = SelectObject(memDC, memBTM);

    HGDIOBJ oldPen = SelectObject(memDC, newPen);
    HGDIOBJ oldBrush = SelectObject(memDC, newBrush);

    Rectangle(memDC, x - 200, y - 200, (x - 200) + 800, (y - 200) + 800);

    SelectObject(memDC, oldPen);

    Ellipse(memDC, Main.left + 5, Main.top + 5, Main.right - 5, Main.bottom - 5); //Буду использовать как ось вращения стрелок.

    
    //Тут рисуются стрелки!!!

    Move(memDC);
    
    //////////////////////////

    BitBlt(hdc, x - 200, y - 200, 400, 400, memDC, x - 200, y - 200, SRCCOPY);

    SelectObject(hdc, oldBTM);
    SelectObject(hdc, oldBrush);


    DeleteObject(newPen);
    DeleteObject(newBrush);
    DeleteObject(memBTM);
    DeleteDC(memDC);

    ReleaseDC(hWnd, hdc);

}