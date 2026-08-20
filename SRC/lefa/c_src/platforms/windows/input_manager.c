
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // Windows XP or superior
#include<windows.h>


#include<stdio.h>

#include"core/input_manager.h"

void convertWindowCoordsToOpenGL(int winX, 
                                 int winY, 
                                 int winWidth, 
                                 int winHeight, 
                                 float *glX, 
                                 float *glY) 
{
    // Convertendo coordenadas da janela para o intervalo de [0, 1]
    float normX = (float)winX / (float)winWidth;
    float normY = (float)winY / (float)winHeight;

    // Convertendo coordenadas normalizadas para o intervalo de [-1, 1]
    *glX = normX * 2.0f - 1.0f;
    *glY = 1.0f - normY * 2.0f;
}

void updateMouseState(HWND hwnd, MouseState *mouseState) 
{
    // Obter as coordenadas atuais do mouse
    POINT currentPos;
    RECT windowRect;
    int newX , 
        newY;
    int winWidth, 
        winHeight;
    GetCursorPos(&currentPos);
    ScreenToClient(hwnd, &currentPos);

    GetClientRect(hwnd, &windowRect);
    winWidth  = windowRect.right - windowRect.left;
    winHeight = windowRect.bottom - windowRect.top;

    // Atualizar as coordenadas atuais do mouse
        newX = currentPos.x;
        newY = currentPos.y;

    // Converter coordenadas da janela para coordenadas OpenGL
    convertWindowCoordsToOpenGL(currentPos.x, 
                                currentPos.y, 
                                winWidth, 
                                winHeight, 
                                &mouseState->currentX, 
                                &mouseState->currentY);

    // Calcular o movimento relativo do mouse desde a última atualização
    mouseState->xrel = newX - mouseState->prevX;
    mouseState->yrel = newY - mouseState->prevY;

    // Atualizar as coordenadas anteriores do mouse para a próxima atualização
    mouseState->prevX = newX;
    mouseState->prevY = newY;

    // Se o mouse se moveu, imprimir as novas coordenadas e o movimento relativo
    if (mouseState->xrel != 0 || mouseState->yrel != 0) 
    {
        //printf("Mouse moved to: (%d, %d)\n", newX, newY);
        //printf("Relative movement: (%d, %d)\n", mouseState->xrel, mouseState->yrel);
    }
}

void confineMouseToWindow(HWND hwnd) 
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    ClientToScreen(hwnd, (POINT*)&rect.left);
    ClientToScreen(hwnd, (POINT*)&rect.right);
    ClipCursor(&rect); // Restringe o cursor à área da janela
}

void resetMouseToCenter(HWND hwnd) 
{
    RECT rect;
    POINT center;
    GetClientRect(hwnd, &rect);
    // Calcular o centro da janela
    center.x = (rect.right - rect.left) / 2;
    center.y = (rect.bottom - rect.top) / 2;
    ClientToScreen(hwnd, &center);
    SetCursorPos(center.x, center.y); // Move o cursor para o centro
}

void initMouseEvent(struct MouseEvent *event) 
{
    if(!event) return;
        event->curX = 0;
        event->curY = 0;
        event->xrel = 0;
        event->yrel = 0;
        event->lastMouseX = 0;
        event->lastMouseY = 0;
        event->mouseMotion = 0;
        event->mouseLeft = 0;
        event->mouseRight = 0;
        event->mouseMiddle = 0;
        event->buttonState = 0;
        event->clicks = 0;
        event->windowFocusID = 0;
        event->mouseInstanceID = 0;
}

void updateMouseEvent(HWND hwnd, struct MouseEvent *event) 
{
    POINT mousePos;
    RECT windowRect;
    
    if (!event) return;


    GetCursorPos(&mousePos);
    ScreenToClient(hwnd, &mousePos);

    GetClientRect(hwnd, &windowRect);

    if (PtInRect(&windowRect, mousePos)) 
    {
        event->curX = mousePos.x;
        event->curY = mousePos.y;
        event->xrel = mousePos.x - event->lastMouseX;
        event->yrel = mousePos.y - event->lastMouseY;
        event->mouseMotion = (event->xrel != 0 || event->yrel != 0);
        event->lastMouseX = mousePos.x;
        event->lastMouseY = mousePos.y;
        
        // Estado dos botões do mouse
        event->mouseLeft   = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
        event->mouseRight  = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
        event->mouseMiddle = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;

        // Atualizar botãoState usando uma máscara de bits
        event->buttonState = 0;
        if (event->mouseLeft) event->buttonState   |= 0x01;
        if (event->mouseRight) event->buttonState  |= 0x02;
        if (event->mouseMiddle) event->buttonState |= 0x04;
    }
}

int mouseInputPos(HWND hwnd, BOOL inputEnabled, int *x, int *y)
{
    RECT windowRect;
    POINT mousePos;

    if(inputEnabled)
    {
        /* Get the x and y coordinates of the mouse relative to the screen */
        GetCursorPos(&mousePos);

        /* Convert coordinates to window-relative coordinates */
        ScreenToClient(hwnd, &mousePos);

        GetClientRect(hwnd, &windowRect);

        if (PtInRect(&windowRect, mousePos))
        {
            /* The mouse is within the window limits */
            static int lastMouseX = 0;
            static int lastMouseY = 0;

            if (mousePos.x != lastMouseX || mousePos.y != lastMouseY)
            {
                lastMouseX = mousePos.x;
                lastMouseY = mousePos.y;
                *x = lastMouseX; // Atualiza o valor apontado pelo ponteiro x
                *y = lastMouseY; // Atualiza o valor apontado pelo ponteiro y
                return 1; // Retorna 1 indicando que o mouse está em movimento
            }
        }
    }
    return 0; // Retorna 0 indicando que o mouse está parado
}

void setInputEnabled(int enabled)
{
    if (enabled == true) 
    {
        /* Enable input */
    } 
    else if (enabled == false) 
    {       /* Disable input*/ 
	
	} 
     else 
    {
        /* Treat if it is an invalid value (optional) */
    }

    inputEnabled = enabled;
}

/* DirectInput VARIABLES & CODE-------------------------------------------------------
*/

 
long initdirectinput(HWND hwnd)
{
    HRESULT hr;
    char buf[256];

    if((hr = DirectInput8Create(GetModuleHandle(NULL), 
                                 DIRECTINPUT_VERSION, 
                                 &IID_IDirectInput8A,
                                 (LPVOID *)&gpdi, 
                                 NULL)) >= 0) 
    {
        return(1);
    }
    wsprintf(buf, "initdirectinput failed: %08lx\n", hr);
    MessageBox(hwnd, buf, "ERROR", MB_OK);
    return(0);
}


void uninitdirectinput()
{
    if(gpdi) 
    { 
        gpdi->lpVtbl->Release(gpdi); 
        gpdi = 0; 
    }
}

/* DirectInput (MOUSE) VARIABLES & CODE------------------------------------------------------- 
*/


void uninitmouse()
{
    if(gpMouse)
    {
        if (dinputevent[0])
        {
            gpMouse->lpVtbl->SetEventNotification(gpMouse, dinputevent[0]);
            CloseHandle(dinputevent[0]); dinputevent[0] = 0;
        }

        gpMouse->lpVtbl->Unacquire(gpMouse); 
        gpMouse->lpVtbl->Release(gpMouse); 
        gpMouse = 0;
    }
}

/*
long initmouse(HWND hwnd)
{
    HRESULT hr;
    DIPROPDWORD dipdw;
    char buf[256];

    if((hr = gpdi->lpVtbl->CreateDevice(gpdi, &GUID_SysMouse, &gpMouse,0)) < 0)
    {
        goto initmouse_bad;
    }

    if((hr = gpMouse->lpVtbl->SetDataFormat(gpMouse, &c_dfDIMouse)) < 0)
    {
        goto initmouse_bad;
    }
        
    if((hr = gpMouse->lpVtbl->SetDataFormat(gpMouse, &c_dfDIMouse2)) < 0) 
    {
        goto initmouse_bad;
    }

    if((hr = gpMouse->lpVtbl->SetCooperativeLevel(gpMouse, hwnd, dinputmouseflags)) < 0) 
    {
        goto initmouse_bad;
    }

    dinputevent[0] = CreateEvent(0,0,0,0); 
    
    if(!dinputevent[0]) 
    {
        goto initmouse_bad;
    }
        
    if((hr = gpMouse->lpVtbl->SetEventNotification(gpMouse, dinputevent[0])) < 0) 
    {
        goto initmouse_bad;
    }

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = MOUSBUFFERSIZE;
    
    if((hr = gpMouse->lpVtbl->SetProperty(gpMouse, DIPROP_BUFFERSIZE, &dipdw.diph)) < 0) 
    {
        goto initmouse_bad;
    }

    if(mouse_acquire) 
    { 
        gpMouse->lpVtbl->Acquire(gpMouse); 
        gbstatus = 0; 
    }
    
    mousper = 1.0; 
    mousince = mougoalx = mougoaly = mougoalz = 0.0;
    moult[0] = -1; 
    moultavg = moultavgcnt = 0;
    readklock(&dmoutsc);
    return(1);

initmouse_bad:;
    uninitmouse();
    wsprintf(buf,"initdirectinput(mouse) failed: %08lx\n",hr);
    MessageBox(hwnd, buf,"ERROR",MB_OK);
    return(0);
}*/


/* DirectInput (KEYBOARD) VARIABLES & CODE------------------------------------------------------- 
*/

long dinputkeyboardflags = DISCL_NONEXCLUSIVE|DISCL_FOREGROUND;

LPDIRECTINPUTDEVICE8 gpKeyboard = 0;
#define KBDBUFFERSIZE 256
DIDEVICEOBJECTDATA KbdBuffer[KBDBUFFERSIZE];

void uninitkeyboard()
{
    if(gpKeyboard)
    {
        if(dinputevent[1])
        {
            gpKeyboard->lpVtbl->SetEventNotification(gpKeyboard, dinputevent[1]);
            CloseHandle(dinputevent[1]); 
            dinputevent[1] = 0;
        }
        gpKeyboard->lpVtbl->Unacquire(gpKeyboard); 
        gpKeyboard->lpVtbl->Release(gpKeyboard); 
        gpKeyboard = 0;
    }
}
 
long initkeyboard(HWND hwnd)
{
    HRESULT hr;
    DIPROPDWORD dipdw;
    char buf[256];

    if((hr = gpdi->lpVtbl->CreateDevice(gpdi, &GUID_SysKeyboard, &gpKeyboard,0)) < 0) 
    {
        goto initkeyboard_bad;
    }
    
    if((hr = gpKeyboard->lpVtbl->SetDataFormat(gpKeyboard, &c_dfDIKeyboard)) < 0) 
    {
        goto initkeyboard_bad;
    }
    
    if((hr = gpKeyboard->lpVtbl->SetCooperativeLevel(gpKeyboard, hwnd, dinputkeyboardflags)) < 0) 
    {
        goto initkeyboard_bad;
    }

    dinputevent[1] = CreateEvent(0,0,0,0); 

    if(!dinputevent[1])
    {
        goto initkeyboard_bad;
    }
    
    if((hr = gpKeyboard->lpVtbl->SetEventNotification(gpKeyboard, dinputevent[1])) < 0) 
    {
        goto initkeyboard_bad;
    }

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = KBDBUFFERSIZE;
    
    if((hr = gpKeyboard->lpVtbl->SetProperty(gpKeyboard, DIPROP_BUFFERSIZE, &dipdw.diph)) < 0) 
    {
        goto initkeyboard_bad;
    }
    
    if(kbd_acquire)
    { 
        gpKeyboard->lpVtbl->Acquire(gpKeyboard); 
        shkeystatus = 0; 
    }
    return(1);

initkeyboard_bad:;
    uninitkeyboard();
    wsprintf(buf,"initdirectinput(keyboard) failed: %08lx\n",hr);
    MessageBox(hwnd, buf, "ERROR", MB_OK);
    return(0);
}

long readkeyboard()
{
    HRESULT hr;
    long i;
    unsigned long dwItems;
    DIDEVICEOBJECTDATA *lpdidod;

    dwItems = KBDBUFFERSIZE;
    hr = gpKeyboard->lpVtbl->GetDeviceData(gpKeyboard, sizeof(DIDEVICEOBJECTDATA), KbdBuffer, &dwItems,0);
    //if (hr == DI_BUFFEROVERFLOW) ?;
    if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    {
        gpKeyboard->lpVtbl->Acquire(gpKeyboard); 
        shkeystatus = 0;
        hr = gpKeyboard->lpVtbl->GetDeviceData(gpKeyboard, sizeof(DIDEVICEOBJECTDATA),KbdBuffer,&dwItems,0);
    }

    if(hr < 0) 
    {
        return(0);
    }
    
    for(i=0;i<(long)dwItems;i++)
    {
        lpdidod = &KbdBuffer[i];

        if(lpdidod->dwData&128) 
        {
            keystatus[lpdidod->dwOfs] = 1;
        }                         
        else 
        {
            keystatus[lpdidod->dwOfs] = 0;
        }

        //event occured "GetTickCount()-lpdidod->dwTimeStamp" milliseconds ago

        // +TD:
        if(lpdidod->dwData&128) 
        {
            ext_keystatus[lpdidod->dwOfs] = 1|2;
        }
        else 
        {
            ext_keystatus[lpdidod->dwOfs] &= ~1; // preserve bit 2 only
        }
    }
    return(dwItems);
}

DWORD keyInputDInput(int keyCode, InputMode mode) 
{
    static DWORD keyPressTimes[256] = {0}; // Armazena os tempos de pressionamento
    static DWORD keyReleaseTimes[256] = {0}; // Armazena os tempos de soltura
    static char prevKeystatus[256] = {0}; // Armazena o estado anterior de cada tecla

    // Atualiza o estado do teclado
    readkeyboard();

    // Lógica de entrada para SINGLE
    if(mode == SINGLE) 
    {
        if(keystatus[keyCode] && !prevKeystatus[keyCode]) 
        {
            prevKeystatus[keyCode] = 1; // Marca como processado
            return 1; // Tecla pressionada pela primeira vez
        }
        if (!keystatus[keyCode]) 
        {
            prevKeystatus[keyCode] = 0; // Reset ao soltar
        }
    }

    // Lógica de entrada para KEEP
    if (mode == KEEP) 
    {
        return keystatus[keyCode];
    }

    // Lógica de entrada para KEEP_TIME
    if (mode == KEEP_TIME) 
    {
        if (keystatus[keyCode] && !prevKeystatus[keyCode]) 
        {
            keyPressTimes[keyCode] = GetTickCount(); // Registra o tempo de início
            prevKeystatus[keyCode] = 1; // Marca como pressionado
        }
        if (!keystatus[keyCode] && prevKeystatus[keyCode]) 
        {
            keyReleaseTimes[keyCode] = GetTickCount(); // Registra o tempo de soltura
            prevKeystatus[keyCode] = 0; // Reset
            return keyReleaseTimes[keyCode] - keyPressTimes[keyCode]; // Retorna a duração
        }
    }

    return 0; // Nenhum evento detectado
}

//============================================
// Joysticks / testando com o dualshock 4 e 5

    // Suporte para até 8 joysticks, não acho que mais que isso é necessario
static LPDIRECTINPUTDEVICE8 gpJoysticks[8] = {0}; 

    // não acho que alguem vai trabalhar com o joy fora da instancia
static DWORD joystickFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;

static DIDEVCAPS joystickCapabilities;


 //pre definicao de funcao
//static void uninitJoystick(int joystickIndex);

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCEA* pdidInstance, VOID* pContext) 
{
    JoystickContext *ctx = (JoystickContext*)pContext;

    if (ctx->joystickIndex <= 0) 
    {
        HRESULT hr = gpdi->lpVtbl->CreateDevice(gpdi,
                                                &pdidInstance->guidInstance,
                                                &ctx->gpJoysticks[ctx->joystickIndex],
                                                NULL);

        if (FAILED(hr)) 
        {
            return DIENUM_STOP; // Erro na criação do dispositivo
        }

        return DIENUM_STOP; // Dispositivo configurado
    }

    ctx->joystickIndex--; // Decrementa o índice para próximo dispositivo
    return DIENUM_CONTINUE; // Continua a busca por dispositivos
}


long initJoystick(HWND hwnd, int joystickIndex) 
{
    HRESULT hr;
    DIPROPDWORD dipdw;
    JoystickContext ctx;
    ctx.joystickIndex = joystickIndex;
    ctx.gpJoysticks = gpJoysticks;

    if(joystickIndex >= 8 || gpJoysticks[joystickIndex]) 
    {
        printf("\n [ !ERROR ]: initJoystick() -> indice invalido ou joystick ja iniciado \n");
        return 0; // Índice inválido ou joystick já inicializado
    }

    ctx.joystickIndex = joystickIndex;
    ctx.gpJoysticks = gpJoysticks;

    // Enumera dispositivos conectados
    hr = gpdi->lpVtbl->EnumDevices(gpdi,
                                  DI8DEVCLASS_GAMECTRL,
                                  EnumJoysticksCallback,
                                  &ctx,
                                  DIEDFL_ATTACHEDONLY);

    if(FAILED(hr) || !gpJoysticks[joystickIndex]) 
    {
        printf("\n [ !ERROR ]: initJoystick() -> Falha na enumeraco ou joystick nao encontrado \n");
        uninitJoystick(joystickIndex); // Garante limpeza em erro
        return 0; // Falha na enumeração ou joystick não encontrado
    }

    // Configurar formato de dados
    hr = gpJoysticks[joystickIndex]->lpVtbl->SetDataFormat(gpJoysticks[joystickIndex], 
                                                           &c_dfDIJoystick2);

    if (FAILED(hr)) 
    {
        printf("\n [ !ERROR ]: initJoystick() -> Falha ao configurar formato \n");
        return 0; // Falha ao configurar formato
    }

    // Configurar nível de cooperação
    hr = gpJoysticks[joystickIndex]->lpVtbl->SetCooperativeLevel(gpJoysticks[joystickIndex], 
                                                                hwnd, 
                                                                joystickFlags);

    if (FAILED(hr)) 
    {
        printf("\n [ !ERROR ]: initJoystick() -> Falha ao configurar nível de cooperação \n");
        return 0; // Falha ao configurar nível de cooperação
    }

    // Obter capacidades do dispositivo
    joystickCapabilities.dwSize = sizeof(DIDEVCAPS);
    gpJoysticks[joystickIndex]->lpVtbl->GetCapabilities(gpJoysticks[joystickIndex], 
                                                        &joystickCapabilities);

    // Configurar buffer de entrada
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = 16; // Buffer para 16 eventos

    gpJoysticks[joystickIndex]->lpVtbl->SetProperty(gpJoysticks[joystickIndex], 
                                                    DIPROP_BUFFERSIZE, 
                                                    &dipdw.diph);

    // Adquirir controle do joystick
    gpJoysticks[joystickIndex]->lpVtbl->Acquire(gpJoysticks[joystickIndex]);
    
    return 1; // Sucesso
}

// Ler estado de um joystick
long readJoystick(int joystickIndex, DIJOYSTATE2* joyState) 
{
    HRESULT hr;

    if (joystickIndex >= 8 || !gpJoysticks[joystickIndex]) 
    {
        return 0; // Indice invalido ou dispositivo inexistente
    }

    hr = gpJoysticks[joystickIndex]->lpVtbl->Poll(gpJoysticks[joystickIndex]);
    if(FAILED(hr)) 
    {
        gpJoysticks[joystickIndex]->lpVtbl->Acquire(gpJoysticks[joystickIndex]);
        hr = gpJoysticks[joystickIndex]->lpVtbl->Poll(gpJoysticks[joystickIndex]);
    }

    if(FAILED(hr)) 
    {
        return 0; // Falha ao acessar estado
    }


    hr = gpJoysticks[joystickIndex]->lpVtbl->GetDeviceState(gpJoysticks[joystickIndex],
                                                            sizeof(DIJOYSTATE2),
                                                            joyState);

    return(SUCCEEDED(hr));
}




// Uninitialize joystick
void uninitJoystick(int joystickIndex) 
{
    if (joystickIndex < 8 && gpJoysticks[joystickIndex]) 
    {
        gpJoysticks[joystickIndex]->lpVtbl->Unacquire(gpJoysticks[joystickIndex]);
        gpJoysticks[joystickIndex]->lpVtbl->Release(gpJoysticks[joystickIndex]);
        gpJoysticks[joystickIndex] = NULL;
    }
}

// Função principal para inicializar múltiplos joysticks
long initJoysticks(HWND hwnd) 
{
    int i;
    for (i = 0; i < 8; i++) 
    {
        if (!initJoystick(hwnd, i)) 
        {
            break; // Para quando não encontrar mais joysticks
        }
    }
    return 1;
}

// Desconectar todos os joysticks
void uninitAllJoysticks() 
{
    int i;
    for(i = 0; i < 8; i++) 
    {
        uninitJoystick(i);
    }
}


/*
DWORD keyInput(int keyCode, InputMode mode) 
{
    int keyState[256] = { FALSE };  // Array para controlar o estado das teclas
    int prevKeyState[256] = { FALSE }; // Array para armazenar o estado anterior das teclas
    DWORD keyPressTimes[256] = { 0 }; // Array para registrar os tempos de pressão das teclas
    DWORD releaseTime = 0;
    DWORD pressTime = 0;
    DWORD pressDuration = 0;

    // Obter o estado atual da tecla
    int currentKeyState = GetAsyncKeyState(keyCode) & PRESS_STATE;

    // Verificar se a tecla está pressionada no momento atual
    if(currentKeyState) 
    {
        // Verificar se a tecla estava solta no frame anterior
        if(!prevKeyState[keyCode]) 
        {
            // Marcar a tecla como pressionada no frame atual
            keyState[keyCode] = true;
            keyPressTimes[keyCode] = GetTickCount(); // Registra o tempo de pressão da tecla
            prevKeyState[keyCode] = true;

            if (mode == SINGLE) 
            {
               return true;
            }
        }
    } 
    else 
    {
        if (keyState[keyCode]) 
        {
            prevKeyState[keyCode] = FALSE; // Marcar a tecla como solta no frame atual

            if (mode == SINGLE) 
            {
                keyState[keyCode] = FALSE;
            }

            releaseTime = GetTickCount();
            pressTime = keyPressTimes[keyCode];
            pressDuration = releaseTime - pressTime;

           if (mode == KEEP_TIME) 
            {
                return pressDuration;
                keyState[keyCode] = false;
            }            

        }
    }

    // Retornar 0 se o modo KEEP ou KEEP_TIME for usado e a tecla estiver pressionada
    return (mode == KEEP) && currentKeyState ? 1 : 0;

}
*/


