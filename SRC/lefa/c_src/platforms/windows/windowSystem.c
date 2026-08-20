
#define _WIN32_WINNT 0x0501 // Windows XP or superior
#include<windows.h>

#include<stdio.h>
#include<stdarg.h>  /* for va_list and associated values and functions */

#include"core/window_manager.h"
#include"core/input_manager.h"


LE_Display* windows[MAX_WINDOWS];
LE_Display *currentWindow = NULL;
int windowCount = 0;
int windowClassIndex = 1;

int windowMovable = 1;

LRESULT CALLBACK
    WINAPI WindowProc(HWND hWnd, 
                      UINT uMsg, 
                      WPARAM wParam, 
                      LPARAM lParam)
{   
    int i;
    
    for(i = 0; i < windowCount; i++) 
    {
        if(windows[i]->platformHandle == hWnd) 
        {
            switch(uMsg) 
            {   /*
                case WM_INPUT:
                {
                    RAWINPUT raw;
                    UINT size = sizeof(RAWINPUT);
                    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

                    if(raw.header.dwType == RIM_TYPEMOUSE)
                    {
                        if (raw.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
                        {
                            printf("Botão lateral 4 pressionado\n");
                        }
                        if (raw.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
                        {
                            printf("Botão lateral 5 pressionado\n");
                        }
                    }
                    return 0;
                }*/
                case WM_MOUSEMOVE: 
                {
                }
                break;
                /*
                case WM_INPUT:
                {
                    processRawInput(lParam);
                }*/
                break;

                case WM_PAINT:
                break;
                case WM_SETTEXT:
                {
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
                }
                break;
                case WM_SIZE:
                    if(wParam != SIZE_MINIMIZED)
                    {

                    }
                break;
                case WM_SYSCOMMAND:
                    switch(wParam) 
                    {
                        case SC_MINIMIZE:
                            // Manipular minimização
                        break;
                        case SC_MAXIMIZE:
                            // Manipular maximização
                        break;
                        case SC_CLOSE:
                            // Manipular fechamento
                        break;
                    }
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
                break;
                case WM_CLOSE:
                    /* Find the window and remove it from the list */
                    engineDeleteDisplay(windows[i]);
                    
                    /* Note: engineDeleteDisplay will handle removing the window from the list */
                    /* and setting currentWindow to NULL if needed. */
                    
                    /* After destruction, break here to avoid further processing */
                    return 0;
                break;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                break;
                default:
                    if(windowMovable)
                    {
                        return DefWindowProc(hWnd, uMsg, wParam, lParam);
                    }
                    else
                    {
                        break;
                    }
            }
            //return 0;
            return DefWindowProc(hWnd, uMsg, wParam, lParam);  /* Passes the message to the default handler */
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);   /* Default message for unhandled cases */
}


LE_Display* engineCreateDisplay(const char *title, int width, int height)
{
    LE_Display *window = (LE_Display*)malloc(sizeof(LE_Display));

    int screenWidth;
    int screenHeight;

    char className[256];

    WNDCLASSEX wc = {0};
    RECT wr = {0};

    snprintf(className, sizeof(className), "LEFA_NT_%d", windowClassIndex++);
    
    if(windowCount >= MAX_WINDOWS) 
    {
        printf(" \n ~~[ ALERT! ]: Maximum number of windows reached! Window Limit =  %d \n", windowCount);
        return NULL;
    }

    if(!window)
    {
        printf("\n ~~[ErRoR]: Window Struct Not Defined Or have Problem ! \n");
        return NULL;
    }

    window->width = width;
    window->height = height;

    window->platformConnection = (void *)GetModuleHandle(NULL);
    
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName  = NULL;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 7, 0));
    wc.lpfnWndProc   = WindowProc;
    wc.lpszClassName = className;
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    wc.hInstance     = (HINSTANCE)window->platformConnection;

    if(!RegisterClassEx(&wc))
    {
        printf(" \n ~~[ERROR!]: function [RegisterClassEX()] in function [engineCreateDisplay] \n");
        free(window);
        return NULL;
    }

    ZeroMemory(&wr, sizeof(RECT));
    wr.left = 0;
    wr.top  = 0;
    wr.right  = window->width;
    wr.bottom = window->height;
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);


    /* Get screen dimensions */
    screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
     /* Calculate window coordinates to center it on the screen */
    window->windowPosX = (screenWidth  - window->width)  / 2;
    window->windowPosY = (screenHeight - window->height) / 2;

    window->platformHandle = CreateWindowEx(0,
                                            className,
                                            title,
                                            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
                                            window->windowPosX,
                                            window->windowPosY,
                                            width,
                                            height,
                                            NULL,
                                            NULL,
                                            (HINSTANCE)window->platformConnection,
                                            NULL
    );

    if(!window->platformHandle) 
    {
        printf(" \n ~~[ERROR!]: function [CreateWindowEx()] \n");
        free(window);
        return NULL;
    }

    ShowWindow((HWND)window->platformHandle, SW_SHOW);
    //ShowWindow((HWND)window->platformHandle, SW_SHOWNORMAL);
    UpdateWindow((HWND)window->platformHandle); /* need this for title top bar */
    windows[windowCount++] = window;
    currentWindow = window;
    return window;
}

void engineDeleteDisplay(LE_Display *display) 
{
    int i, j = 0; 
    if(display) 
    {
        // Remove the window from the list
        for (i = 0; i < windowCount; i++) 
        {
            if (windows[i] == display) 
            {
                // Remove the window from the array
                for (j = i; j < windowCount - 1; j++) 
                {
                    windows[j] = windows[j + 1];
                }
                windowCount--;
                break;
            }
        }

        // Verifique se a janela a ser destruída é a janela atual
        if (currentWindow == display) 
        {
            currentWindow = NULL; // Desvincule a janela atual
        }

        // Destroi a janela
        if(display->platformHandle) 
        {
            DestroyWindow((HWND)display->platformHandle);
        }
        if(windowCount == 0)
        {
            ExitProcess(0); // Encerra o processo e fecha o terminal
        }

        // Libera a memória da estrutura da janela
        free(display);
    }
}

void getWindowActiveStatus(LE_Display *display) 
{
    /* check if the window is active */
    display->windowActiveStatus = (GetForegroundWindow() == display->platformHandle);
}

int engineSetLoop(LE_Display *display, int *loopVar, void (*renderFunction)())
{

    /* Start Loop */
    MSG msg;
    

    if(renderFunction == NULL) 
    {
        printf("\n ~~[ERROR!]: [engineSetLoop()] Parameter 3 is NULL!\n");
        return -1;
    }


    //while(*loopVar) 
    for(;;)
    {
        /* check if the window is active */
        /*window->windowActiveStatus = (GetForegroundWindow() == window->platformHandle);


         //setInputEnabled(isWindowActive);

        /*if (isWindowActive)
        {
            //int nahx, nahy;
            //mouseInput(hwnd, TRUE, &nahx, &nahy);
        }*/

        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                *loopVar = 0;
            }
            else 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            renderFunction();
        }
        
        if(*loopVar == 0)
        {
            break;
        }
    }
    

    DestroyWindow(display->platformHandle);
    return 0;
}

void setWindowSize(LE_Display *display, int width, int height) 
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();


    // Atualiza os valores de largura e altura na estrutura da janela
    display->width = width;
    display->height = height;

    GetWindowRect(hDesktop, &desktop);

    display->windowPosX = (desktop.right  - display->width)  / 2;
    display->windowPosY = (desktop.bottom - display->height) / 2;

    if(display) 
    {
        SetWindowPos(display->platformHandle, 
                     HWND_TOP, 
                     display->windowPosX, 
                     display->windowPosY, 
                     width, 
                     height, 
                     SWP_NOZORDER);
    }
}


void getWindowSize(LE_Display *display, int *width, int *height) 
{
    RECT clientRect;
    GetClientRect(display->platformHandle, &clientRect);
    *width  = clientRect.right  - clientRect.left;
    *height = clientRect.bottom - clientRect.top;
}

void engineSetWindowTitle(LE_Display *display, const char *format, ...) 
{
    char title[256];
    va_list args;

    /* Initialize the argument list */
    va_start(args, format);

    /* Format the string */
    vsnprintf(title, sizeof(title), format, args);

    /* Clean up the argument list */
    va_end(args);

    /* Set the window title */
    SetWindowText((HWND)display->platformHandle, title);
}

void engineSetWindowIcon(LE_Display *display, const char* iconPath)
{
    /* Set the window icon */
    HICON hIcon = (HICON)LoadImage((HINSTANCE)display->platformConnection,
                                   iconPath,
                                   IMAGE_ICON,
                                   32,
                                   32,
                                   LR_LOADFROMFILE);
    if(hIcon) 
    {
        SendMessage(display->platformHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    } 
    else 
    {
        printf("Failed to load icon from path: %s\n", iconPath);
    }
}

void windowBorderless(LE_Display *display, int mode, ...)
{
    int windowWidth = 0;
    int windowHeight = 0;

    DWORD style = GetWindowLongPtr(display->platformHandle, GWL_STYLE);

    /* Initialize variable argument list */ 
    va_list args;
    va_start(args, mode);


    /* If the mode requires additional width and height */ 
    if(mode == 2) /* Let's assume 2 means full screen */
    {
        windowWidth = GetSystemMetrics(SM_CXSCREEN);
        windowHeight = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        /* Receives the width and height if provided */
        if(mode == 1) /* Let's assume 1 means border mode without specifying size */
        {
            windowWidth = va_arg(args, int);
            windowHeight = va_arg(args, int);
        }
    }

    if(mode == 0) 
    {
        /* Bordered mode */
        style |= (WS_CAPTION | WS_THICKFRAME);  /* Restores edges */
        SetWindowLongPtr(display->platformHandle, GWL_STYLE, style);
    }
    else if(mode == 1)
    {
        /* Borderless mode with specified width and height */
        style &= ~(WS_CAPTION | WS_THICKFRAME); /* Remove borders */
        SetWindowLongPtr(display->platformHandle, GWL_STYLE, style);
    }
    else if(mode == 2)
    {
        /* Full screen mode */
        style &= ~(WS_CAPTION | WS_THICKFRAME); /* Remove borders */
        SetWindowLongPtr(display->platformHandle, GWL_STYLE, style);
        windowWidth = GetSystemMetrics(SM_CXSCREEN);
        windowHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    /* Updates window position and size */
    SetWindowPos(display->platformHandle, 
                 HWND_TOP, 
                 0, 
                 0, 
                 windowWidth, 
                 windowHeight, 
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    /* Clears the list of variable arguments */
    va_end(args);
}

