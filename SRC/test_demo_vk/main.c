/* TESTS HERE */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>  // Para sleep()


#define _VULKAN
//#define LOAD_MATH_LIB
#define LOAD_LODEPNG
#include<loadEngine.h>


#include"core/input_manager.h"

#include"localmath.h"

#include"kenklock.h"
#include"console_utils.h"


#include"lehidjoy/lehidjoy.h"

#include<3rdparty/pthread/pthread.h>

//#include"gui_test/custom_ui.h"

/* Display Window Size */
int winWidth,
    winHeight;

LE_Display* display; /* Display Window Struct */
int activeLoop = 1; /* Main Program Loop */
volatile int activeLoop_thd = 1;

RenderInterface* engineRender = NULL;

double fps;

clock_t lastTime = 0;
float deltaTime = 0.0f;


float lastMouseX, lastMouseY;
float mouseX, mouseY, mouseZ;
long buttonStatus;
int curPosX, curPosY;



void computeFPS(double* fpsCount) /* contador de fps a cada atualização. */
{
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER t0;
    static int frames = 0;
    static double fps = 0.0;
    static char titlestring[200];
    double elapsedTime;

    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    
    // Initialize frequency and t0 on the first call
    if (frames == 0)
    {
        QueryPerformanceFrequency(&frequency);
        t0 = t;
    }

    // If one second has passed, or if this is the very first frame
    elapsedTime = (double)(t.QuadPart - t0.QuadPart) / frequency.QuadPart;
    if (elapsedTime > 0.1) /* fps count time */
    {
        fps = (double)frames / elapsedTime;
        //sprintf(titlestring, "GLSL procedural shaders (%.1f FPS)", fps);
        //SetWindowText(hwnd, titlestring);
        
        t0 = t;
        frames = 0;
    }
    frames++;
    //return fps;

    *fpsCount = fps;
}

//
double getElapsedTimeInSeconds() 
{
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER t0;
    static BOOL initialized = FALSE;
    LARGE_INTEGER t;

    if (!initialized) 
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&t0);
        initialized = TRUE;
    }

    QueryPerformanceCounter(&t);

    return (double)(t.QuadPart - t0.QuadPart) / frequency.QuadPart;
}



int Setup()
{



    return 0;
}


int endProgram()
{

    LE_print(LE_COLOR_BLUE, "\n Program Ends \n");
    
    uninitmouse();
    //uninitkeyboard(); /* !FIX ME */
    //uninitJoystick(0);
    uninitdirectinput();


    engineRender->cleanup(display);

    activeLoop = 0;
    //exit(0); /* f0rce program ends */

 return 0;
}



void inputEvent() 
{
    int i;

    if(keyInputDInput(KEY_ESC, SINGLE))
    {
        endProgram();
    }



    readmouse(&mouseX, &mouseY, &mouseZ, &buttonStatus);
    //printf("Mouse: X=%.2f, Y=%.2f, Z=%.2f\n", mouseX, mouseY, mouseZ);
    // Verificar botões do mouse
    if(buttonStatus & 0x01) 
    {
        printf(" <- Botao esquerdo pressionado.\n");
    }
    if(buttonStatus & 0x02) 
    {
        printf(" -> Botao direito pressionado.\n");
    }

    if(buttonStatus & 0x04) 
    {
        LE_print(LE_COLOR_GREEN, "\n HELLO! ");
        printf("Botao do meio (Mouse 3) pressionado\n");
    }

    if(buttonStatus & 0x08) 
    {
        printf("\xE2\x86\x93 ֍ Botao Lower Side (Mouse 4) pressionado\n");
    }

    if(buttonStatus & 0x10) 
    {
        
        printf("\xE2\x86\x91 Botao Upper Side (Mouse 5) pressionado\n");
    }


    // Controle de ações
    if(keyInputDInput(KEY_W, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_S, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_A, KEEP)) 
    {
       
    }
    if(keyInputDInput(KEY_D, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_Q, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_E, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_Z, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_X, KEEP)) 
    {
        
    }
    if(keyInputDInput(KEY_LSHIFT, KEEP)) 
    {
        
        
    }

    

}


void Render() 
{


}


void programLoop()
{

    clock_t currentTime = clock();

    deltaTime = (float)(currentTime - lastTime) / CLOCKS_PER_SEC;

    engineRender->update(display);
    inputEvent();

    computeFPS(&fps);

    engineSetWindowTitle(display, "LEFA - Demo tests - FPS: %.2f", fps);


	Render();


    if(fps < 35)
    {
        LE_print(LE_COLOR_YELLOW, "\n [ ALERT! ] FPS: %.2f", fps);
    }

	getWindowSize(display, &winWidth, &winHeight);

    lastTime = currentTime;
}

int main(int argc, char **argv, char* envp[])
{
    int i;
    char renderAPI = 0;
    winWidth = 800;
    winHeight = 600;

    //enableANSI();
    LE_initConsole();
	system("chcp 65001 > nul");

	printf("!\n Hello World ! \n");


	display = engineCreateDisplay("Lefa Test DEMO !", winWidth, winHeight);
    if(!display)
    {
        printf("\n problem in MAIN no window created. \n ");
    }

    //printf("\n Handle da janela criado: %p\n", window->platformHandle);


    initklock();

    // Inicializar DirectInput
    if(!initdirectinput(display->platformHandle)) 
    {
      printf("Erro ao inicializar DirectInput.\n");
      //return -1;
    }

    // Inicializar o dispositivo de mouse
    if(!initmouse(display->platformHandle)) 
    {
        printf("Erro ao inicializar o dispositivo de mouse.\n");
        uninitdirectinput();
        //return -1;
    }

    if(!initkeyboard(display->platformHandle)) 
    {
        printf("Erro ao inicializar o teclado.\n");
        uninitdirectinput();
    }


    renderAPI = 2;

    if(renderAPI == 1)
    {
        //engineRender = &openglInterface;
    }
    else if(renderAPI == 2)
    {
        engineRender = &vulkanInterface;
    };

    engineRender->init(display);

    Setup();

    //engineRender->setVSync(0);

	engineSetLoop(display, &activeLoop, programLoop);

return 0;
}
