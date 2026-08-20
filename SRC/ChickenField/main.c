#include<stdio.h> 
#include<stdlib.h> 
#include<time.h> 
#include<math.h> 
#include<stdbool.h> 
#include<stdint.h> 


#include"core/input_manager.h" 
#include<3rdparty/pthread/pthread.h> 

#undef APIENTRY 

#define M3D_IMPLEMENTATION 
#include<3rdparty/model3d/m3d.h> 

#define _OPENGL 
#define GL_VERSION_LIMIT 
#define GL2_0 
#define LOAD_MATH_LIB 
#define LOAD_LODEPNG 
//#define LOAD_NKGUI 
#include<loadEngine.h> 


#include"util/util.h" 

#include"console_utils.h" 


#define NANOSVG_IMPLEMENTATION 
#include<3rdparty/nanosvg/nanosvg.h> 
#define NANOSVGRAST_IMPLEMENTATION 
#include<3rdparty/nanosvg/nanosvgrast.h> 


#include"contents/le_camera/camera_component.h" 

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

// tests 
RCamera *testCamera; // main render camera 
float mouseX, mouseY, mouseZ; 
long buttonStatus; 
// ==================================================== 
// =================== DISPLAY ======================== 
// ==================================================== 
int programLoop = 1; 
LE_Display* display; 
/* Display Window Struct */ 
int displayWidth, displayHeight; 
// ==================================================== 
// ================ Render Config ===================== 
// ==================================================== 
RenderInterface* engineRender = NULL; 


double fps; 

float deltaTimeA = 0.0f; 
float deltaTimeB = 0.0f; 
float deltaTimeC = 0.0f; 


double timeA = 0.0, lastTimeKlockA = 0.0; 
double timeB = 0.0, lastTimeKlockB = 0.0; 
double timeC = 0.0, lastTimeKlockC = 0.0; 
// ==================================================== 
// ================ THREAD LOGIC ====================== 
// ==================================================== 
pthread_t threadA; 
pthread_t threadB; 
pthread_t threadC; 

int F_threadA_Setup(); 
void F_threadA_Loop(); 
void* F_threadA(void* arg) // A - API Draw Render , no sleep functions, need speed 
{ 
	F_threadA_Setup(); 
	for(;;) 
	{ 
		//printf("\n Thread A \n"); 
		F_threadA_Loop(); 
		if(programLoop == 0) 
		{ 
			break; 
		} 
	} 
	return NULL; 
} 

int F_threadB_Setup(); 
void F_threadB_Loop(); 
void* F_threadB(void* arg) // B 
{ 
	F_threadB_Setup(); 
	for(;;) 
	{ 
		//printf("\n Thread B \n"); 
		F_threadB_Loop(); 
		if(programLoop == 0) 
		{ 
			break; 
		} 
	} 
	return NULL; 
} 

void* F_threadC(void* arg) // C 
{ 
	for(;;) 
	{ 
		//printf("\n Thread C \n"); 
		if(programLoop == 0) 
		{ 
			break; 
		} 
	} 
	return NULL; 
} 

int A_Setup(); 
void A_Render(); 
int F_threadA_Setup() 
{ 
	unsigned char renderAPI = 0; 
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
	if(renderAPI == 1) 
	{ 
		if(!init_gl()) 
		{ 
			return false; 
		} 
	} 
	else if(renderAPI == 2) 
	{ 
	}; 

	engineRender->setVSync(0); // vsync disabled 
	A_Setup(); 
	return 0; 
} 

void F_threadA_Loop() // render Thread 
{ 
	lastTimeKlockA = timeA; 
	readklock(&timeA); 
	deltaTimeA = (float)(timeA - lastTimeKlockB); 
	
	if(deltaTimeA > 0.1f) deltaTimeA = 0.1f; 

	if(deltaTimeA < 0.0001f) deltaTimeA = 0.0001f; 

	engineRender->update(display); 
	computeFPS(&fps); 
	engineSetWindowTitle(display, "PUBG Redux - FPS: %.2f", fps); 
	A_Render(); 

	if(fps < 35) 
	{ 
		LE_print(LE_COLOR_YELLOW, "\n [ ALERT! ] FPS: %.2f", fps); 
	} 
} 

int A_Setup() 
{ 
	initklock(); 
	testCamera = (RCamera *)malloc(sizeof(RCamera)); /* Manual Alloc */ 

	if(!testCamera) 
	{ 
		LE_print(LE_COLOR_RED, "\n Erro ao alocar memória para RCamera\n"); 
	} 

	if(cameraSetup(testCamera, displayWidth, displayHeight) != 0) 
	{ 
		LE_print(LE_COLOR_RED, "Erro ao configurar a câmera\n"); 
		free(testCamera); 
	} 

	testCamera->cameraPos.x = 0.0f; 
	testCamera->cameraPos.y = 0.0f; 
	testCamera->cameraPos.z = 5.0f; 
	return 0; 
} 

void A_Render() // Thread A - Render 
{ 
	float aspectRatio; 
	getWindowSize(display, &displayWidth, &displayHeight); 
	glViewport(0, 0, displayWidth, displayHeight); 
	if(displayHeight > 0) 
	{ 
		aspectRatio = (float)displayWidth / (float)displayHeight; 
	} 
	else 
	{ 
		aspectRatio = 1.0f; 
	} 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	cameraMatrix(testCamera, // camera pointer 
				 75.0f, // fov 
				 0.03f, // near value 
				 20000.0f, // far meters 
				 aspectRatio); // aspect ratio 

	//no shaders 
	sendCameraToRender(testCamera, 0, NULL); 

	// no shaders 
	glBegin(GL_TRIANGLES); 
	glColor3f(1.0f, 0.0f, 0.0f); 
	glVertex3f(0.0f, 1.0f, 0.0f); 

	/* Topo */ 
	glColor3f(0.0f, 1.0f, 0.0f); 
	glVertex3f(-1.0f, -1.0f, 0.0f); 

	/* Esquerda */ 
	glColor3f(0.0f, 0.0f, 1.0f); 
	glVertex3f(1.0f, -1.0f, 0.0f); 

	/* Direita */ 
	glEnd(); 
} 

int F_threadB_Setup() // B 
{ 
	initklock(); // need this to input functions work, dont remove or will get a crash 

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
	} 
	return 0; 
} 

void inputEvent(); 
void F_threadB_Loop() // B 
{ 
	lastTimeKlockB = timeB; 
	readklock(&timeB); 
	deltaTimeB = (float)(timeB - lastTimeKlockB); 
	
	if(deltaTimeB > 0.1f) deltaTimeB = 0.1f; 

	if(deltaTimeB < 0.0001f) deltaTimeB = 0.0001f; 
	inputEvent(); 
	Sleep(5.0f); 
} 

void inputEvent() 
{ 
	int i; 
	int inputDetected = 0; 
	float yawRad; 
	float pitchRad; 
	vec3 targetFocus; 
	if(testCamera == NULL) 
	return; 

	if(keyInputDInput(KEY_ESC, SINGLE)) 
	{ 
		//endProgram(); 
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

	cameraInput(testCamera, mouseX, mouseY, deltaTimeB); 
	testCamera->camForward = 0; 
	testCamera->camBackWard = 0; 
	testCamera->camLeft = 0; 
	testCamera->camRight = 0; 
	testCamera->camElevate = 0; 
	testCamera->camLower = 0; 
	testCamera->rollLeft = 0; 
	testCamera->rollRight = 0; 
	testCamera->camSprint = 0; 

	// Controle de ações 
	if(keyInputDInput(KEY_W, KEEP)) 
	{ 
		testCamera->camForward = 1; 
	} 

	if(keyInputDInput(KEY_S, KEEP)) 
	{ 
		testCamera->camBackWard = 1; 
	} 

	if(keyInputDInput(KEY_A, KEEP)) 
	{ 
		testCamera->camLeft = 1; 
	} 

	if(keyInputDInput(KEY_D, KEEP)) 
	{ 
		testCamera->camRight = 1; 
	} 

	if(keyInputDInput(KEY_Q, KEEP)) 
	{ 
		testCamera->rollLeft = 1; 
	} 

	if(keyInputDInput(KEY_E, KEEP)) 
	{ 
		testCamera->rollRight = 1; 
	} 

	if(keyInputDInput(KEY_Z, KEEP)) 
	{ 
		testCamera->camElevate = 1; 
	} 

	if(keyInputDInput(KEY_X, KEEP)) 
	{ 
		testCamera->camLower = 1; 
	} 

	if(keyInputDInput(KEY_LSHIFT, KEEP)) 
	{ 
		//cameraSpeed *= sprintMultiplier; 
		// Aumenta a velocidade 
		testCamera->camSprint = 1; 
	} 
} 

// =================================================== 
// =================== SETUP ========================= 
// =================================================== 
// main thread 

int code_setup() // initialization of custom components 
{ 
	return 0; 
} 

// =================================================== 
// =================== MAIN LOOP ===================== 
// =================================================== 

void code_loop() // quick update 
{ 
	//printf("\n Hello World! \n"); 
} 

int main(void) 
{// engine initialization 

	displayWidth = 900; 
	displayHeight = 600; 

	if(programLoop == 0) 
	{ 
		programLoop = 1; 
	} 

	display = engineCreateDisplay("PUBG Redux", displayWidth, displayHeight); 

	if(!display) 
	{ 
		printf("\n problem in MAIN no window created. \n "); 
	} 
	// custom code initialization 
	code_setup(); 
	if(pthread_create(&threadA, NULL, F_threadA, NULL)) 
	{ 
		fprintf(stderr, "Error creating thread. Thread A\n"); 
		return 1; 
	} 

	if(pthread_create(&threadB, NULL, F_threadB, NULL)) 
	{ 
		fprintf(stderr, "Error creating thread. Thread B\n"); 
		return 1; 
	} 

	if(pthread_create(&threadB, NULL, F_threadC, NULL)) 
	{ 
		fprintf(stderr, "Error creating thread. Thread C\n"); 
		return 1; 
	} 

	engineSetLoop(display, &programLoop, code_loop); // main thread 
	pthread_join(threadA, NULL); 
	pthread_join(threadB, NULL); 
	pthread_join(threadC, NULL); 

	printf("Threads finalizadas com sucesso.\n"); 
	system("pause"); 
	return 0; 
}




