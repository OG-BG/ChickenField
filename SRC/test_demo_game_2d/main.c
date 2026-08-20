/* TESTS HERE */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>  // Para sleep()


#define _OPENGL
#define GL_VERSION_LIMIT
#define GL2_0
//#define LOAD_MATH_LIB
#define LOAD_LODEPNG
#define LOAD_NKGUI
#include<loadEngine.h>


#include"core/input_manager.h"
//#include"uinput.h"

#include"localmath.h"
#include"util/util.h"

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

/* Global Engine Input System */

#define MAX_KEYS 4

typedef struct 
{/* GP = GAMEPAD or Controllers / KB = KeyBoard / MS = Mouse or Joystick*/

    int input_up;       /* GP PAD UP, KB ARROW UP,  KB W, KB I , GP JOYUP.*/
    int input_down;     /* GP PAD DOWN, KB ARROW Down,  KB S, KB K , GP JOYDown.*/
    int input_left;     /* GP PAD LEFT, KB ARROW LEFT,  KB A, KB J , GP JOYLEFT.*/
    int input_right;    /* GP PAD RIGHR, KB ARROW RIGHT,  KB D, KB L , GP JOYRIGHT.*/
    int input_jump;     /* GP PAD PS X, GP PAD XB A, KB SPACE*/
    int input_attack;   /* GP PAD RIGHT TRIGGER, MS LEFT*/
    int input_confirm;  /* GP PAD PS CIRCLE, GP PAD XB B, KB ENTER, KB SPACE*/
    int input_interact; /* GP PAD PS SQUARE, GP PAD XB CROSS X, KB E*/
    int input_decline;  /* GP PAD PS CROSS, GP PAD XB CROSS X, KB ESC, KB BACKSPACE*/
    int input_pause;    /* GP PAD PS OPTIONS, GP PAD XB MENU, KB HOME */
    int input_menu;     /* GP PAD PS TOUCH BUTTON, GP PAD XB GUIDE, KB ESC */

    struct
    {
        int x;
        int y;
    }cur_pos;   /* Cursor Position */

    struct
    {
        int x;
        int y;
        int press;
    }L_joystick;    /* Left Analog Stick */
    
    struct
    {
        int x;
        int y;
        int press;
    }R_joystick;    /* Right Analog Stick */

    struct
    {
        int left;
        int right;

        unsigned char left_fb;
        unsigned char right_fb;
    }trigger_feedback;

    struct
    {
        int left;
        int right;
    }bumber;

    struct
    {
        int up;
        int down;
        int left;
        int right;
    }directional;

} LEGLOBAL_Input;

#define CONTROL_KEYBOARD        0
#define CONTROL_MOUSE           1
#define CONTROL_XBOX            2
#define CONTROL_PLAYSTATION     3
#define CONTROL_GENERIC_GAMEPAD 4


/**/


static bool init_gl(void)
{

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));


    return true;
}

typedef struct 
{
    float x, y;
    float width, height;
} AABB;

int checkCollision(AABB a, AABB b) 
{
    return(a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.y + a.height > b.y);
}

typedef struct 
{
    float x, y;
    float width, height;
    float velocityX, velocityY;
    int onGround;
} Player;

void drawPlayer(Player* player) 
{
    glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
    glBegin(GL_QUADS);
        glVertex2f(player->x, player->y);
        glVertex2f(player->x + player->width, player->y);
        glVertex2f(player->x + player->width, player->y + player->height);
        glVertex2f(player->x, player->y + player->height);
    glEnd();
}

void updatePlayer(Player* player, float deltaTime) 
{
    player->velocityY += 500.0f * deltaTime; // Gravidade

    player->x += player->velocityX * deltaTime;
    player->y += player->velocityY * deltaTime;

    // Verificação de colisão com o chão
    if(player->y + player->height >= 300)
    { // Colisão com o chão
        player->y = 300 - player->height; // Ajusta para a posição exata do chão
        player->velocityY = 0;
        player->onGround = 1; // Está no chão
    }
    else
    {
        player->onGround = 0; // Está no ar
    }
}

void handleInput(Player* player) 
{
    if(keyInputDInput(KEY_A, KEEP)) 
    {
        player->velocityX = -100;
    } 
    else if(keyInputDInput(KEY_D, KEEP)) 
    {
        player->velocityX = 100;
    } 
    else 
    {
        player->velocityX = 0;
    }

    if(keyInputDInput(KEY_SPC, KEEP) && player->onGround) 
    {
        player->velocityY = -250; // Pulo
        player->onGround = 0; // Define como no ar
    }
    if(keyInputDInput(KEY_W, KEEP) && player->onGround) 
    {
        player->velocityY = -250; // Pulo
        player->onGround = 0; // Define como no ar
    }
}


Player player = {100, 300, 32, 32, 0, 0, 1}; // Inicialização do jogador
Player player2 = {600, 300, 32, 32, 0, 0, 1}; // Inicialização do jogador

/* Estruturas globais */

/* Color intensity */
float intensity = 1.0f;
uint16_t lrmbl = 0.0;
uint16_t rrmbl = 0.0;
/* Force */
TriggerEffectType rType = NoResitance;
int btMul;

typedef struct
{
    DeviceContext   con[5];
    DS5InputState   input[5];
    DS5OutputState  output[5];
    unsigned int    requiredLength;

} hidjoy_device;

hidjoy_device joystick;

/* Função para inicializar o controle */
int initialize_controller(hidjoy_device *hj_device) 
{
    unsigned int i = 0;
    DS5W_ReturnValue result;
    DS5W_ReturnValue resultDeviceContext[5];
    DeviceEnumInfo device[5];

    result = enumDevices(device, 5, &hj_device->requiredLength, 1);

    if(hj_device->requiredLength == 0) 
    {
        printf("\n No devices found. <[^~~^]> \n");
        return -1;
    }
    
    /* Check return */
    if(result == DS5W_OK) 
    {
        unsigned int i = 0;

        printf("Dispositivos detectados (%d encontrados):\n", hj_device->requiredLength);
        for(i = 0; i < hj_device->requiredLength; i++) 
        {
            printf(" \n =============== \n");
            printf("Device %d:\n", i + 1);
            printf("  Path: %s\n", device[i]._internal.path);
            if(device[i]._internal.connection == BT)
            {
                printf(" \n Connection Bluetooth \n");
            }
            else if(device[i]._internal.connection == (BT | USB))
            {
                printf(" \n Connection Bluetooth & chargin \n");
            }
            else
            {
                printf(" \n Connection USB \n");
            }
        }
    } 
    else if(result == DS5W_E_INSUFFICIENT_BUFFER)
    {
        printf("\n Insufficient buffer. At least %d entries are required.\n ", hj_device->requiredLength);
    } 
    else 
    {
        printf("Error listing devices (Error Code: %d).\n", result);
    }

    /* Inicializa o contexto para cada dispositivo */
    for(i = 0; i < hj_device->requiredLength; i++) 
    {
        resultDeviceContext[i] = initDeviceContext(&device[i], &hj_device->con[i]);

        if(resultDeviceContext[i] != OK) 
        {
            printf("\n [!ERROR]: Failed to connect to controller %d\n", i + 1);
            return -1;
        }
        else
        {
            btMul = hj_device->con[i]._internal.connection == BT ? 10 : 1;

            memset(&hj_device->input, 0, sizeof(DS5InputState));
            memset(&hj_device->output, 0, sizeof(DS5OutputState));
        }
    }

    return 0;
}

/* Função principal do loop */
void controller_loop(hidjoy_device *hj_device) 
{
    unsigned int i = 0;

    for(i = 0; i < hj_device->requiredLength; i++) 
    {
        DS5W_ReturnValue DeviceInputState;
        DeviceInputState = getDeviceInputState(&hj_device->con[i], &hj_device->input[i]);

        if(DeviceInputState == OK) 
        {
            /*
            // Aqui você pode adicionar o código para processar a entrada de cada controle
            printf("Reading state for controller %d...\n", i + 1);
            
            // Exemplo de como processar os dados de entrada
            printf("Left Stick (Controller %d): X: %d, Y: %d\n", i + 1, 
                        (int)hj_device->input[i].leftStick.x, 
                        (int)hj_device->input[i].leftStick.y);*/
        } 
        else 
        {
            printf("\nDevice %d removed! Reconnecting...\n", i + 1);
            reconnectDevice(&hj_device->con[i]);
        }

        /* Player LEDS */
        hj_device->output[0].playerLeds.playerLedFade = true;
        hj_device->output[0].playerLeds.bitmask = HIDJOY_LED_PLAYER_1;
        hj_device->output[0].playerLeds.brightness = 0x00;

        hj_device->output[1].playerLeds.playerLedFade = true;
        hj_device->output[1].playerLeds.bitmask = HIDJOY_LED_PLAYER_2;
        hj_device->output[1].playerLeds.brightness = 0x00;
        //

        /* Lightbar */
        hj_device->output[0].lightbar = color_R8G8B8_UCHAR_A32_FLOAT(0, 255, 0, 1.0f);
        hj_device->output[1].lightbar = color_R8G8B8_UCHAR_A32_FLOAT(100, 50, 50, 1.0f);


        // Enviar dados de saída para o dispositivo
        setDeviceOutputState(&hj_device->con[i], &hj_device->output[i]);
    }

}

void cleanHidJoys(hidjoy_device *hj_device)
{
    unsigned int i = 0;
    /* Free state */
    for(i = 0; i < hj_device->requiredLength; i++) 
    {
        freeDeviceContext(&hj_device->con[i]);
    }

}

void usleep(unsigned int microseconds) 
{
    Sleep(microseconds / 1000); // Converte microssegundos para milissegundos
}
void uSleep(unsigned int microseconds) 
{
    Sleep(microseconds / 1000); // Converte microssegundos para milissegundos
}


void* loopThread(void* arg) 
{
    while(activeLoop_thd)
    {
        controller_loop(&joystick);
        uSleep(500); // 500ms (usamos Sleep no Windows)
    }
    cleanHidJoys(&joystick);
    return 0;
}

LEGLOBAL_Input user_input01;
LEGLOBAL_Input user_input02;

pthread_t threadID;

int Setup()
{

    initialize_controller(&joystick);

    // Criar a thread
    if(pthread_create(&threadID, NULL, loopThread, NULL) != 0) 
    {
        printf("Erro ao criar a thread!\n");
        return 1;
    }


    //joystick.output.lightbar = color_R8G8B8_UCHAR_A32_FLOAT(0, 255, 0, intensity);
    //setDeviceOutputState(&joystick.con, &joystick.output); /* Send Data to Device */



    return 0;
}


int endProgram()
{

    LE_print(LE_COLOR_BLUE, "\n Program Ends \n");
    
    activeLoop_thd = 0; /* Thread Loop OFF */
    pthread_join(threadID, NULL);

    //cleanHidJoys(&joystick);
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
    int inputDetected = 0;

    if(keyInputDInput(KEY_ESC, SINGLE))
    {
        endProgram();
    }

    user_input01.input_up = 0;
    user_input01.input_down = 0;
    user_input01.input_left = 0;
    user_input01.input_right = 0;

    user_input02.input_up = 0;
    user_input02.input_down = 0;
    user_input02.input_left = 0;
    user_input02.input_right = 0;

    player.velocityX = 0;
    player2.velocityX = 0;

    if(joystick.input[0].buttonsAndDpad & DS5W_ISTATE_DPAD_UP) 
    {
        user_input01.input_up = 1;
    }

    if(joystick.input[0].buttonsAndDpad & DS5W_ISTATE_DPAD_DOWN) 
    {
        user_input01.input_down = 1;
    } 

    if(joystick.input[0].buttonsAndDpad & DS5W_ISTATE_DPAD_LEFT) 
    {
        user_input01.input_left = 1;
    }

    if(joystick.input[0].buttonsAndDpad & DS5W_ISTATE_DPAD_RIGHT) 
    {
        user_input01.input_right = 1;
    }


    if(keyInputDInput(KEY_W, KEEP)) 
    {
        user_input01.input_up = 1;
    }

    if(keyInputDInput(KEY_A, KEEP)) 
    {
        user_input01.input_left = 1;
    }

    if(keyInputDInput(KEY_D, KEEP)) 
    {
        user_input01.input_right = 1;
    } 

    if(keyInputDInput(KEY_UPAR, KEEP)) 
    {
        user_input01.input_up = 1;
    }

    if(keyInputDInput(KEY_LFAR, KEEP)) 
    {
        user_input01.input_left = 1;
    }

    if(keyInputDInput(KEY_RIAR, KEEP)) 
    {
        user_input01.input_right = 1;
    } 


    if(user_input01.input_up && player.onGround)
    {
        player.velocityY = -250; // Pulo
        player.onGround = 0; // Define como no ar
    }

    if(user_input01.input_left)
    {
        player.velocityX = -100;
    }

    if(user_input01.input_right)
    {
        player.velocityX = 100;
    }

    if(joystick.input[1].buttonsAndDpad & DS5W_ISTATE_DPAD_UP) 
    {
        user_input02.input_up = 1;
    }

    if(joystick.input[1].buttonsAndDpad & DS5W_ISTATE_DPAD_DOWN) 
    {
        user_input02.input_down = 1;
    } 

    if(joystick.input[1].buttonsAndDpad & DS5W_ISTATE_DPAD_LEFT) 
    {
        user_input02.input_left = 1;
    }

    if(joystick.input[1].buttonsAndDpad & DS5W_ISTATE_DPAD_RIGHT) 
    {
        user_input02.input_right = 1;
    }

    if(user_input02.input_up && player2.onGround)
    {
        player2.velocityY = -250; // Pulo
        player2.onGround = 0; // Define como no ar
    }

    if(user_input02.input_left)
    {
        player2.velocityX = -100;
    }

    if(user_input02.input_right)
    {
        player2.velocityX = 100;
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


    if(keyInputDInput(KEY_LCTRL, KEEP) && keyInputDInput(KEY_E, KEEP))
    {
        glClearColor(0.0f, 0.19f, 0.06f, 1.0f);
    }
    else
    {
        glClearColor(0.0f, 0.05f, 0.07f, 1.0f);
    }

    /* Limpa a tela e o buffer de profundidade */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glDisable(GL_DEPTH_TEST);

    /* Configuração para 2D */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);  // Modo 2D com (0,0) no topo esquerdo
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Desenha um quadrado vermelho */
    glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    glBegin(GL_QUADS);
        glVertex2f(100, 100);   // Topo esquerdo
        glVertex2f(200, 100);   // Topo direito
        glVertex2f(200, 200);   // Inferior direito
        glVertex2f(100, 200);   // Inferior esquerdo
    glEnd();


    //handleInput(&player);
    updatePlayer(&player, deltaTime);

    drawPlayer(&player); // Renderiza o jogador
    updatePlayer(&player2, deltaTime);

    drawPlayer(&player2); // Renderiza o jogador

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
	glViewport(0, 0, winWidth, winHeight);

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


    renderAPI = 1;

    if(renderAPI == 1)
    {
        engineRender = &openglInterface;
    }
    else if(renderAPI == 2)
    {
        /*engineRender = &vulkanInterface;*/
    };

    engineRender->init(display);

    if(!init_gl())
    {
        return false;
    }

    Setup();

    engineRender->setVSync(0);

	engineSetLoop(display, &activeLoop, programLoop);

return 0;
}
