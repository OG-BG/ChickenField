#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#ifdef APIENTRY
	#undef APIENTRY
#endif

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // Windows XP or superior
#include<windows.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>

//#include"core/window_manager.h"

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED  1

static bool inputEnabled = true;

typedef struct 
{
    float prevX;
    float prevY;
    float currentX;
    float currentY;
    float xrel;
    float yrel;
}MouseState;

struct KeyState 
{
    bool isActive;
    bool isReleased;
    int  mouseX;
    int  mouseY;

};

typedef struct MouseEvent 
{
    int curX;                       // Posição atual do mouse em X
    int curY;                       // Posição atual do mouse em Y
    int xrel;                       // Movimento relativo do mouse em X
    int yrel;                       // Movimento relativo do mouse em Y
    int lastMouseX;                 // Última posição X do mouse
    int lastMouseY;                 // Última posição Y do mouse
    int mouseMotion;                // Flag para indicar movimento do mouse
    int mouseLeft;                  // Estado do botão esquerdo do mouse
    int mouseRight;                 // Estado do botão direito do mouse
    int mouseMiddle;                // Estado do botão do meio do mouse
    int buttonState;                // Estado dos botões do mouse (bitmask)
    unsigned char clicks;           // Número de cliques (simples ou duplo)
    unsigned int windowFocusID;     // ID da janela com foco
    unsigned int mouseInstanceID;   // ID da instância do mouse
} MouseEvent;

/*
enum keyMaps 
{
/* Mouse *
    KEY_MS1  = 0x01,   /* Left button   *
    KEY_MS2  = 0x02,   /* Right button  *
    KEY_MS3  = 0x04,   /* Middle button *
  /*KEY_MS4  = 0000,   /* otherbuttons  *
  /*KEY_MS5  = 0000,   /* otherbuttons  *

/* Keyboard */
    /* Special Keys *
    KEY_ESC  = 0x1B,   /* Escape *
    KEY_F1   = 0x70,   /* F1 *
    KEY_F2   = 0x71,   /* F2 *
    KEY_F3   = 0x72,   /* F3 *
    KEY_F4   = 0x73,   /* F4 *
    KEY_F5   = 0x74,   /* F5 *
    KEY_F6   = 0x75,   /* F6 *
    KEY_F7   = 0x76,   /* F7 *
    KEY_F8   = 0x77,   /* F8 *
    KEY_F9   = 0x78,   /* F9 *
    KEY_F10  = 0x79,   /* F10 *
    KEY_F11  = 0x7A,   /* F11 *
    KEY_F12  = 0x7B,   /* F12 *
    KEY_BSPC = 0x08,   /* Backspace *
    KEY_TAB  = 0x09,   /* TAB Key *
    KEY_CPSL = 0x14,   /* Caps lock *
    KEY_SHIFT = 0x10,  /* Shift Key *
    KEY_CTRL = 0x11,   /* Control Key *
    KEY_ALT  = 0x12,   /* Alt Keys *
    KEY_SPC  = 0x20,   /* Space Key *
    KEY_ENTR = 0x0D,   /* Enter Key *
    KEY_LFAR = 0x25,   /* <  left arrow *
    KEY_UPAR = 0x26,   /* ^  Up arrow *
    KEY_RIAR = 0x27,   /* >  Right Arrow  *
    KEY_DWAR = 0x28,   /* ˅  Down Arrow *
    /* double Keys/key combination/secondary function *
    KEY_HYPH = 0xBD,   /* OEM_MINUS (_ -) *
    /*
    *
    KEY_QUOT = 0xDE,   /* Quote ' " *
    /* KEY_DQ   0x22   /* Double Quote " = KEY_DQ *
    /*
    *
    KEY_0    = 0x30,   /* 0 *
    KEY_1    = 0x31,   /* 1 *
    KEY_2    = 0x32,   /* 2 *
    KEY_3    = 0x33,   /* 3 *
    KEY_4    = 0x34,   /* 4 *
    KEY_5    = 0x35,   /* 5 *
    KEY_6    = 0x36,   /* 6 *
    KEY_7    = 0x37,   /* 7 *
    KEY_8    = 0x38,   /* 8 *
    KEY_9    = 0x39,   /* 9 *
    /*
    *
    KEY_A    = 0x41,
    KEY_B    = 0x42,
    KEY_C    = 0x43,
    KEY_D    = 0x44,
    KEY_E    = 0x45,
    KEY_F    = 0x46,
    KEY_G    = 0x47,
    KEY_H    = 0x48,
    KEY_I    = 0x49,
    KEY_J    = 0x4A,
    KEY_K    = 0x4B,
    KEY_L    = 0x4C,
    KEY_M    = 0x4D,
    KEY_N    = 0x4E,
    KEY_O    = 0x4F,
    KEY_P    = 0x50,
    KEY_Q    = 0x51,
    KEY_R    = 0x52,
    KEY_S    = 0x53,
    KEY_T    = 0x54,
    KEY_U    = 0x55,
    KEY_V    = 0x56,
    KEY_W    = 0x57,
    KEY_X    = 0x58,
    KEY_Y    = 0x59,
    KEY_Z    = 0x5A,

};
*/

enum keyMaps {
    /* Mouse */
    KEY_MS1  = 0x00,   /* Left button   */
    KEY_MS2  = 0x01,   /* Right button  */
    KEY_MS3  = 0x02,   /* Middle button */
    KEY_MS4  = 0x03,   /* Side button 1 (Mouse 4) */
    KEY_MS5  = 0x04,   /* Side button 2 (Mouse 5) */

    /* Special Keys */
    KEY_ESC  = 0x01,   /* Escape */
    KEY_ESCAPE  = 0x01,   /* Escape */
    KEY_F1   = 0x3B,   /* F1 */
    KEY_F2   = 0x3C,   /* F2 */
    KEY_F3   = 0x3D,   /* F3 */
    KEY_F4   = 0x3E,   /* F4 */
    KEY_F5   = 0x3F,   /* F5 */
    KEY_F6   = 0x40,   /* F6 */
    KEY_F7   = 0x41,   /* F7 */
    KEY_F8   = 0x42,   /* F8 */
    KEY_F9   = 0x43,   /* F9 */
    KEY_F10  = 0x44,   /* F10 */
    KEY_F11  = 0x57,   /* F11 */
    KEY_F12  = 0x58,   /* F12 */
    KEY_BSPC = 0x0E,   /* Backspace */
    KEY_BACKSPACE = 0x0E,   /* Backspace */
    KEY_TAB  = 0x0F,   /* TAB Key */
    KEY_CPSL = 0x3A,   /* Caps Lock */
    KEY_CAPSLOCK = 0x3A,   /* Caps Lock */
    KEY_LSHIFT = 0x2A,  /* Left Shift */
    KEY_RSHIFT = 0x36,  /* Right Shift */
    KEY_LCTRL = 0x1D,  /* Left Control */
    KEY_RCTRL = 0xE01D, /* Right Control */
    KEY_LALT  = 0x38,   /* Left Alt */
    KEY_RALT  = 0xE038, /* Right Alt */
    KEY_SPC  = 0x39,   /* Space */
    KEY_SPACE  = 0x39,   /* Space */
    KEY_ENTR = 0x1C,   /* Enter */
    KEY_ENTER = 0x1C,   /* Enter */
    KEY_LFAR = 0xCB,   /* Left Arrow */
    KEY_LEFT_ARROW = 0xCB,   /* Left Arrow */
    KEY_UPAR = 0xC8,   /* Up Arrow */
    KEY_UP_ARROW = 0xC8,   /* Up Arrow */
    KEY_RIAR = 0xCD,   /* Right Arrow */
    KEY_RIGHT_ARROW = 0xCD,   /* Right Arrow */
    KEY_DWAR = 0xD0,   /* Down Arrow */
    KEY_DOWN_ARRROW = 0xD0,   /* Down Arrow */

    /* Numbers */
    KEY_0    = 0x0B,
    KEY_1    = 0x02,
    KEY_2    = 0x03,
    KEY_3    = 0x04,
    KEY_4    = 0x05,
    KEY_5    = 0x06,
    KEY_6    = 0x07,
    KEY_7    = 0x08,
    KEY_8    = 0x09,
    KEY_9    = 0x0A,

    /* Letters */
    KEY_A    = 0x1E,
    KEY_B    = 0x30,
    KEY_C    = 0x2E,
    KEY_D    = 0x20,
    KEY_E    = 0x12,
    KEY_F    = 0x21,
    KEY_G    = 0x22,
    KEY_H    = 0x23,
    KEY_I    = 0x17,
    KEY_J    = 0x24,
    KEY_K    = 0x25,
    KEY_L    = 0x26,
    KEY_M    = 0x32,
    KEY_N    = 0x31,
    KEY_O    = 0x18,
    KEY_P    = 0x19,
    KEY_Q    = 0x10,
    KEY_R    = 0x13,
    KEY_S    = 0x1F,
    KEY_T    = 0x14,
    KEY_U    = 0x16,
    KEY_V    = 0x2F,
    KEY_W    = 0x11,
    KEY_X    = 0x2D,
    KEY_Y    = 0x15,
    KEY_Z    = 0x2C
};


#define PRESS_STATE 0x8000

typedef enum 
{
    SINGLE,
    KEEP,
    KEEP_TIME
} InputMode;

static 
float mouseDeltaX = 0.0f, 
      mouseDeltaY = 0.0f;



void initMouseEvent(struct MouseEvent *event);

void updateMouseEvent(HWND hwnd, struct MouseEvent *event);

int mouseInputPos(HWND hwnd, BOOL inputEnabled, int *x, int *y);

void setInputEnabled(int enabled);

// DWORD keyInput(int keyCode, InputMode mode); /* windows winuser keys */ 

void convertWindowCoordsToOpenGL(int winX, 
                                 int winY, 
                                 int winWidth, 
                                 int winHeight, 
                                 float *glX, 
                                 float *glY);


void updateMouseState(HWND hwnd, MouseState *mouseState);

void confineMouseToWindow(HWND hwnd);

void resetMouseToCenter(HWND hwnd);

static 
void registerRawInput(HWND hwnd) 
{
    RAWINPUTDEVICE rid;

    rid.usUsagePage = 0x01;  // Genérico
    rid.usUsage = 0x02;      // Mouse
    rid.dwFlags = RIDEV_INPUTSINK; // Capturar mesmo quando fora de foco
    rid.hwndTarget = hwnd;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) 
    {
        MessageBox(hwnd, "Falha ao registrar Raw Input.", "Erro", MB_OK | MB_ICONERROR);
    }
}

static 
void processRawInput(LPARAM lParam) /* FIXME! : lag input */
{
    UINT dwSize = 0;
    RAWINPUT *raw = NULL;

    // Obtenha o tamanho necessário para armazenar os dados do RawInput
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    if(dwSize == 0) return;

    raw = (RAWINPUT *)malloc(dwSize);
    if(!raw) 
    {
        fprintf(stderr, "Falha ao alocar memória para RAWINPUT.\n");
        return;
    }

    // Obtenha os dados do RawInput
    if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) 
    {
        if(raw->header.dwType == RIM_TYPEMOUSE) 
        {
            // Obtenha os movimentos relativos do mouse
            mouseDeltaX = (float)raw->data.mouse.lLastX;
            mouseDeltaY = (float)raw->data.mouse.lLastY;

            // Opcional: Adicione depuração para ver os valores
            printf("Movimento relativo: DeltaX = %.2f, DeltaY = %.2f\n", mouseDeltaX, mouseDeltaY);
        }
    } 
    else 
    {
        fprintf(stderr, "Falha ao processar WM_INPUT.\n");
    }

    free(raw); // Libere a memória
}

#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

#include"test_demo/kenklock.h"



//DirectInput VARIABLES & CODE-------------------------------------------------------
char keystatus[256];
long shkeystatus = 0;
#define KEYBUFSIZ 256
long keybuf[KEYBUFSIZ], keybufr = 0, keybufw = 0, keybufw2 = 0;

char ext_keystatus[256]; // +TD
char ext_mbstatus[8] = {0}; // +TD extended mouse button status
long ext_mwheel = 0;
//#ifdef NOINPUT
//long mouse_acquire = 0;
//#else
long mouse_acquire = 1, kbd_acquire = 1;
void (*setmousein)(long, long) = NULL;
long mouse_out_x, 
     mouse_out_y;
HANDLE dinputevent[2] = {0,0};


LPDIRECTINPUT8A gpdi = 0;

DIMOUSESTATE2 mouseState; // Suporte para até 8 botões

long initdirectinput(HWND hwnd);

void uninitdirectinput();


/* mouse */
     //long dinputmouseflags = DISCL_EXCLUSIVE|DISCL_FOREGROUND;
long dinputmouseflags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

LPDIRECTINPUTDEVICE8 gpMouse = 0;
#define MOUSBUFFERSIZE 64
DIDEVICEOBJECTDATA MousBuffer[MOUSBUFFERSIZE];
long gbstatus = 0, gkillbstatus = 0;

//Mouse smoothing variables:
long mousmoth = 1;
double dmoutsc;
float mousper;
float mousince, mougoalx, mougoaly, mougoalz, moutscale;
long moult[4], moultavg, moultavgcnt;

void uninitmouse();

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
}


    void readmouse(float *fmousx, float *fmousy, float *fmousz, long *bstatus)
    {
        double odmoutsc;
        float f, fmousynctics;
        long i, got, ltmin, ltmax, nlt0, nlt1, nlt2;
        long mousx, mousy, mousz;
        HRESULT hr;
        unsigned long dwItems = MOUSBUFFERSIZE;
        DIDEVICEOBJECTDATA *lpdidod;


        if ((!mouse_acquire) || (!gpMouse)) 
        { 
            *fmousx = 0; 
            *fmousy = 0; 
            *fmousz = 0; 
            *bstatus = 0; 
            return; 
        }

        hr = gpMouse->lpVtbl->GetDeviceState(gpMouse, sizeof(DIMOUSESTATE2), &mouseState);
        if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) 
        {
            gpMouse->lpVtbl->Acquire(gpMouse); // Reaquira o dispositivo
            *fmousx = 0;
            *fmousy = 0;
            *fmousz = 0;
            *bstatus = 0;
            return;
        }


        dwItems = MOUSBUFFERSIZE;
        hr = gpMouse->lpVtbl->GetDeviceData(gpMouse, sizeof(DIDEVICEOBJECTDATA),MousBuffer,&dwItems,0);
        if(hr == DI_BUFFEROVERFLOW) moult[0] = -1;
        if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
        {
            gpMouse->lpVtbl->Acquire(gpMouse); gbstatus = 0;
            hr = gpMouse->lpVtbl->GetDeviceData(gpMouse, sizeof(DIDEVICEOBJECTDATA),MousBuffer,&dwItems,0);
            if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) 
            { 
                *fmousx = 0; 
                *fmousy = 0; 
                *fmousz = 0; 
                *bstatus = 0; 
                return; 
            }
        }

        //Estimate mouse period (mousper) in units of CPU cycles:
        mousx = mousy = mousz = 0; 
        got = 0;
        i = 0; lpdidod = &MousBuffer[i];
        
        while(i < (long)dwItems)
        {
            moult[3] = moult[2]; moult[2] = moult[1]; moult[1] = moult[0];
            moult[0] = lpdidod->dwTimeStamp;
            do
            {
                switch(lpdidod->dwOfs)
                {
                    case DIMOFS_X: 
                        mousx += lpdidod->dwData; 
                    break;
                    case DIMOFS_Y: 
                        mousy += lpdidod->dwData; 
                    break;
                    case DIMOFS_Z: 
                        mousz += lpdidod->dwData; 
                    break;
                    case DIMOFS_BUTTON0: 
                        if(lpdidod->dwData&128) 
                        {
                            ext_mbstatus[0] = 1|2; 
                        }
                        else 
                        {
                            ext_mbstatus[0] &= 2;
                        }
                        gbstatus = ((gbstatus&~1)|((lpdidod->dwData>>7)&1)); 
                        moult[0] = -1; 
                        break;
                    /*case DIMOFS_BUTTON1: if (lpdidod->dwData&128) ext_mbstatus[1] = 1|2; else ext_mbstatus[1] &= 2;
                                            gbstatus = ((gbstatus&~2)|((lpdidod->dwData>>6)&2)); moult[0] = -1; break;
                    */
                    case DIMOFS_BUTTON1:
                                if(lpdidod->dwData&128) 
                                {
                                    ext_mbstatus[1] = 1 | 2;
                                } 
                                else 
                                {
                                    ext_mbstatus[1] &= ~2;
                                }
                                gbstatus = (gbstatus & ~2) | ((lpdidod->dwData >> 6) & 2);
                                moult[0] = -1;
                    break;
                    case DIMOFS_BUTTON2: 
                                if(lpdidod->dwData&128) 
                                {
                                    ext_mbstatus[2] = 1|2; 
                                }
                                else 
                                {
                                    ext_mbstatus[2] &= 2;
                                }           
                                gbstatus = ((gbstatus&~4)|((lpdidod->dwData>>5)&4)); 
                                moult[0] = -1; 
                    break;
                    case DIMOFS_BUTTON3: 
                                if(lpdidod->dwData&128) 
                                {
                                    ext_mbstatus[3] = 1|2; 
                                }
                                else 
                                {
                                    ext_mbstatus[3] &= 2;
                                }    
                                gbstatus = ((gbstatus&~8)|((lpdidod->dwData>>4)&8)); 
                                moult[0] = -1; 
                    break;
                    case DIMOFS_BUTTON4:
                                if (lpdidod->dwData&128) 
                                {
                                    ext_mbstatus[4] = 1 | 2;
                                } 
                                else 
                                {
                                    ext_mbstatus[4] &= ~2;   // Limpa o bit de "pressed"
                                }
                                gbstatus = (gbstatus & ~16) | ((lpdidod->dwData >> 3) & 16); // Atualiza o estado no gbstatus
                                moult[0] = -1;
                    break;
            }
            i++; lpdidod = &MousBuffer[i];
        } 
        while ((i < (long)dwItems) && ((long)lpdidod->dwTimeStamp == moult[0]));

            if (moult[0] != -1)
            {
                got++;
                if ((moult[1] != -1) && (moult[2] != -1) && (moult[3] != -1))
                {
                    nlt0 = moult[0]-moult[1];
                    nlt1 = moult[1]-moult[2];
                    nlt2 = moult[2]-moult[3];
                    ltmin = nlt0; ltmax = nlt0;
                    if (nlt1 < ltmin) ltmin = nlt1;
                    if (nlt2 < ltmin) ltmin = nlt2;
                    if (nlt1 > ltmax) ltmax = nlt1;
                    if (nlt2 > ltmax) ltmax = nlt2;
                    if (ltmin*2 >= ltmax) //WARNING: NT's timer has 10ms resolution!
                    {
                        moultavg += moult[0]-moult[3]; moultavgcnt += 3;
                        mousper = (float)moultavg/(float)moultavgcnt;
                    }
                }
            }
        }
        if(gkillbstatus) 
        { 
            gkillbstatus = 0; 
            gbstatus = 0; 
        } //Flush packets after task switch
    
        (*bstatus) = gbstatus;

            //Calculate and return smoothed mouse data in: (fmousx, fmousy)
        odmoutsc = dmoutsc; 
        readklock(&dmoutsc);
        fmousynctics = (float)((dmoutsc-odmoutsc)*1000.0);

            //At one time, readklock() wasn't always returning increasing values.
            //This made fmousynctics <= 0 possible, causing /0. Fixed now :)
        if((!moultavgcnt) || (!mousmoth)) //|| ((*(long *)&fmousynctics) <= 0))
        { 
            (*fmousx) = (float)mousx; 
            (*fmousy) = (float)mousy; 
            (*fmousz) = (float)mousz; 
            return; 
        }

        mousince = min(mousince+mousper*(float)got,mousper+fmousynctics);
        if(fmousynctics >= mousince) 
        { 
            f = 1; mousince = 0; 
        }
        else 
        { 
            f = fmousynctics / mousince; 
            mousince -= fmousynctics; 
        }
        mougoalx += (float)mousx; (*fmousx) = mougoalx*f; mougoalx -= (*fmousx);
        mougoaly += (float)mousy; (*fmousy) = mougoaly*f; mougoaly -= (*fmousy);
        mougoalz += (float)mousz; (*fmousz) = mougoalz*f; mougoalz -= (*fmousz);
    }

//DirectInput (KEYBOARD) VARIABLES & CODE-------------------------------------------------------


//#define KEYBUFSIZ 256
//static long keybuf[KEYBUFSIZ], keybufr = 0, keybufw = 0, keybufw2 = 0;

void uninitkeyboard();

long initkeyboard(HWND hwnd);

long readkeyboard();

DWORD keyInputDInput(int keyCode, InputMode mode);

/*============================================
/ Joysticks / testando com o dualshock 4 e 5 */

/* Estrutura e variáveis globais para contexto */
typedef struct 
{
    int joystickIndex; /* Índice do joystick a ser configurado */
    LPDIRECTINPUTDEVICE8 *gpJoysticks; /* Array de dispositivos joystick */
} JoystickContext;

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCEA* pdidInstance, VOID* pContext);

long initJoystick(HWND hwnd, int joystickIndex);

long readJoystick(int joystickIndex, DIJOYSTATE2* joyState);

void uninitJoystick(int joystickIndex);

long initJoysticks(HWND hwnd);

void uninitAllJoysticks();


#endif /* INPUT_MANAGER_H */
