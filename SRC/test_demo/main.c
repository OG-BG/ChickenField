/* TESTS HERE */

#include<stdio.h>
#include<stdlib.h>

#define _OPENGL
#define GL_VERSION_LIMIT
#define GL2_0
//#define LOAD_MATH_LIB
#define LOAD_LODEPNG
#define LOAD_NKGUI
#include<loadEngine.h>

#include<time.h>
#include<math.h>

#include"core/input_manager.h"
//#include"uinput.h"

#include"localmath.h"
#include"util/util.h"
#include"render/gl_command_utils.h"
#include"camera/camera.h"

#include"kenklock.h"
#include"console_utils.h"
#include"render/obj_model.h"
#include"render/pre_geometry_forms.h"

// #include"render/test_lmdl.h" // Lips Of Suna LMDL tests

#include"model_test/all_model.h" // load lips of suna model

#define NK_WIN32_GL2_IMPLEMENTATION
#include"gui_test/nuklear_win32_gl2.h"

#include"lehidjoy/lehidjoy.h"

/* Display Window Size */
int winWidth,
    winHeight;

LE_Display* display; /* Display Window Struct */
int activeLoop = 1; /* Main Program Loop */

RenderInterface* engineRender = NULL;

double fps;

//GLuint program;



clock_t lastTime = 0;
float deltaTime = 0.0f;

RCamera *testCamera;

//Vector3 cameraPos = {0.0f, 0.0f, 3.0f};
//Vector3 cameraFront = {0.0f, 0.0f, -1.0f};
//Vector3 cameraUp = {0.0f, 1.0f, 0.0f};

//float cameraSpeed = 0.05f; // Ajuste de velocidade

//float yaw = -90.0f; // Posição inicial da câmera (em graus)
//float pitch = 0.0f;
//float roll = 0.0f;

//float rollTarget = 0.0f;    // Valor para qual o roll deve retornar
//float rollSpeed = 5.0f;     // Velocidade de retorno em graus por segundo
//float rollMax = 45.0f;      // Limite máximo de roll em qualquer direção

// Sensibilidade do mouse
//float sensitivity = 0.1f;

float lastMouseX, lastMouseY;
float mouseX, mouseY, mouseZ;
long buttonStatus;
int curPosX, curPosY;


//----------------------nk-------------------------
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
//----------------------nk-------------------------

/* init gui state */
struct nk_context *ctx;
struct nk_colorf bg;


//#define MAX_MEMORY (256 * 1024)  // Tamanho da memória para alocar

void initNuklear()
{
    struct nk_font_atlas *atlas;

    ctx = nk_wgl2_init(display->platformHandle);

    nk_wgl2_font_stash_begin(&atlas);

    nk_wgl2_font_stash_end();


    bg.r = 0.10f, 
    bg.g = 0.18f, 
    bg.b = 0.24f, 
    bg.a = 1.0f;

}

void nuklearInput(struct nk_context *ctx, HWND hwnd);

void nuklearUpdate()
{
    nk_input_begin(ctx);
        nuklearInput(ctx, display->platformHandle);
    nk_input_end(ctx);

    /* GUI */
    if(nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;

        nk_layout_row_static(ctx, 30, 80, 1);
        if(nk_button_label(ctx, "button"))
        {
            printf("\n button pressed\n");
        }
        
        nk_layout_row_dynamic(ctx, 30, 2);
        
        if(nk_option_label(ctx, "easy", op == EASY)) 
        {
            op = EASY;
        }
        
        if(nk_option_label(ctx, "hard", op == HARD)) 
        {
            op = HARD;
        }
            
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        if(nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) 
        {
            nk_layout_row_dynamic(ctx, 120, 1);
            bg = nk_color_picker(ctx, bg, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
            bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(ctx);
        }
    }
    nk_end(ctx);

    // render
        /* 
        IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
    nk_wgl2_render(NK_ANTI_ALIASING_ON);
//cleanup:
    //nk_sdl_shutdown();

}




/*
// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,     0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,     2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
GLuint indices[] =
{
    0, 1, 2, // Bottom side
    0, 2, 3, // Bottom side
    4, 6, 5, // Left side
    7, 9, 8, // Non-facing side
    10, 12, 11, // Right side
    13, 15, 14 // Facing side
};

*/

// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS        /    TexCoord    /       NORMALS     //
    -1.0f, 0.0f,  1.0f,     0.0f, 0.0f, 0.0f,       0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
    -1.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,       0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,       1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f,  1.0f,     0.0f, 0.0f, 0.0f,       1.0f, 0.0f,     0.0f, 1.0f, 0.0f
};

// Indices for vertices order
GLuint indices[] =
{
    0, 1, 2,
    0, 2, 3
};

GLfloat lightVertices[] =
{ //     COORDINATES     //
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
};

typedef struct 
{
    GLuint vbo;  // Vertex Buffer Object
    GLuint ebo;  // Element Buffer Object
    size_t indexCount; // indices count
} ModelBuffers;

typedef struct 
{
    float position[3]; // Posição do vértice (x, y, z)
    float texCoords[2]; // Coordenadas de textura (u, v)
} VertexData;

LIMdlModel* MODEL01;
GLuint MODEL01_VBO;
ModelBuffers buffers;
GLuint M01_skin;
GLuint M01_texture_s;
GLuint M01_texAttrib_s;

void print_limdl_model(const LIMdlModel* model)
{
    int i;

    if(!model) 
    {
        printf("Modelo é NULL!\n");
        return;
    }

    printf("==== LIMdlModel ====\n");
    printf("ID: %d\n", model->id);
    printf("Flags: %d\n", model->flags);
    printf("Bounds: (MinX: %f, MinY: %f, MinZ: %f, MaxX: %f, MaxY: %f, MaxZ: %f)\n",
           model->bounds.min.x, model->bounds.min.y, model->bounds.min.z,
           model->bounds.max.x, model->bounds.max.y, model->bounds.max.z);

    printf("\nNodes:\n");
    printf("Hierarquia ou outros dados de LIMdlNodes - Personalize aqui!\n");

    printf("\nHairs: %d\n", model->hairs.count);
    for(i = 0; i < model->hairs.count; i++) 
    {
        printf("  Hair %d: Dados do cabelo - Substitua com membros do LIMdlHairs\n", i);
    }

    printf("\nLevels of Detail (LOD): %d\n", model->lod.count);
    for(i = 0; i < model->lod.count; i++) 
    {
        printf("  LOD %d: Dados do LOD - Substitua com membros do LIMdlLod\n", i);
    }

    printf("\nMaterials: %d\n", model->materials.count);
    for(i = 0; i < model->materials.count; i++) 
    {
        printf("  Material %d: Dados do material - Substitua com membros do LIMdlMaterial\n", i);
    }

    printf("\nParticle Systems: %d\n", model->particle_systems.count);
    for(i = 0; i < model->particle_systems.count; i++) 
    {
        printf("  Particle System %d: Dados do sistema de partículas - Substitua com LIMdlParticleSystem\n", i);
    }

    printf("\nPartitions: %d\n", model->partitions.count);
    for(i = 0; i < model->partitions.count; i++) 
    {
        printf("  Partition %d: Dados da partição - Substitua com LIMdlPartition\n", i);
    }

    printf("\nShapes: %d\n", model->shapes.count);
    for(i = 0; i < model->shapes.count; i++) 
    {
        printf("  Shape %d: Dados da forma - Substitua com LIMdlShape\n", i);
    }

    printf("\nShape Keys: %d\n", model->shape_keys.count);
    for(i = 0; i < model->shape_keys.count; i++) 
    {
        printf("  Shape Key %d: Dados da chave de forma - Substitua com LIMdlShapeKey\n", i);
    }

    printf("\nVertices: %d\n", model->vertices.count);
    for(i = 0; i < model->vertices.count; i++) 
    {
        LIMdlVertex* vertex = &model->vertices.array[i];
        printf("  Vertex %d: Pos(%f, %f, %f), Normal(%f, %f, %f), UV(%f, %f)\n",
           i,
           vertex->coord.x, vertex->coord.y, vertex->coord.z,  // Corrigido de position para coord
           vertex->normal.x, vertex->normal.y, vertex->normal.z,
           vertex->texcoord[0], vertex->texcoord[1]);          // Corrigido de uv para texcoord
    }

    printf("\nWeight Groups: %d\n", model->weight_groups.count);
    for(i = 0; i < model->weight_groups.count; i++) 
    {
        printf("  Weight Group %d: Dados do grupo de peso - Substitua com LIMdlWeightGroup\n", i);
    }
/*
    printf("\n apenas vértices \n ");
    for (i = 0; i < model->vertices.count; i++) 
    {
        LIMdlVertex* vertex = &model->vertices.array[i];
        printf("  Vertex %d: Pos(%f, %f, %f)\n", i,
           vertex->coord.x, vertex->coord.y, vertex->coord.z);
    }
*/
}


void setShaderUniforms(GLuint shaderProgram, RCamera *RCamera)
{
    Matrix4x4 model;
    Matrix4x4 proj;
    GLint modelLoc, viewLoc, projLoc;


    model = identity_matrix();
    proj = identity_matrix();

    modelLoc = glGetUniformLocation(shaderProgram, "model_Matrix");
    viewLoc = glGetUniformLocation(shaderProgram, "view_Matrix");
    projLoc = glGetUniformLocation(shaderProgram, "proj_Matrix");

    glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model.m44[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &RCamera->view.m44[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &RCamera->proj.m44[0][0]);
    glUseProgram(0);

}

ModelBuffers uploadModelData(LIMdlModel* model) 
{
    ModelBuffers buffers;
    LIMdlLod* lod;
    float* vertexData;
    int i;
    GLuint* indexData;

    buffers.vbo = 0;
    buffers.ebo = 0;

    // Verifica se o modelo é válido
    if(!model || model->lod.count == 0 || model->vertices.count == 0) 
    {
        fprintf(stderr, "Erro: Modelo inválido.\n");
        return buffers;
    }

    // Pega o primeiro LOD para simplificação (pode adaptar para múltiplos LODs)
    lod = &model->lod.array[0];

    // Atualiza para acomodar posição (x, y, z) e UV (u, v)
    vertexData = malloc(model->vertices.count * 5 * sizeof(float)); // 3 para posição + 2 para UV
    if(!vertexData) 
    {
        fprintf(stderr, "Erro: Falha ao alocar memória para vértices.\n");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < model->vertices.count; i++) 
    {
        vertexData[i * 5 + 0] = model->vertices.array[i].coord.x;
        vertexData[i * 5 + 1] = model->vertices.array[i].coord.y;
        vertexData[i * 5 + 2] = model->vertices.array[i].coord.z;

        vertexData[i * 5 + 3] = model->vertices.array[i].texcoord[0]; // Coordenada U
        vertexData[i * 5 + 4] = model->vertices.array[i].texcoord[1]; // Coordenada V
    }

    glGenBuffers(1, &buffers.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vertices.count * 5 * sizeof(float), vertexData, GL_STATIC_DRAW);
    free(vertexData);

    // Element Buffer Object
    indexData = malloc(lod->indices.count * sizeof(GLuint));
    if(!indexData) 
    {
        fprintf(stderr, "Erro: Falha ao alocar memória para índices.\n");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < lod->indices.count; i++) 
    {
        indexData[i] = (GLuint)lod->indices.array[i]; // Converte para GLuint
    }

    glGenBuffers(1, &buffers.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lod->indices.count * sizeof(GLuint), indexData, GL_STATIC_DRAW);
    free(indexData);

    // Desvincula os buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    buffers.indexCount = model->lod.array[0].indices.count;

    return buffers;
}


void renderModel(ModelBuffers buffers, 
                 GLuint shaderProgram, 
                 GLuint textureId) 
{
    GLint positionAttrib;
    GLint texCoordAttrib;

    setShaderUniforms(shaderProgram, testCamera); // Enviar as matrizes antes de renderizar

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);

    positionAttrib = glGetAttribLocation(shaderProgram, "vertPosition");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    texCoordAttrib = glGetAttribLocation(shaderProgram, "vertTexCoords");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, buffers.indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(texCoordAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
}

/* map test */
typedef struct 
{
    int id;              // ID do modelo
    char modelName[64];  // Nome do modelo
    float x, y, z;       // Posição no mundo
    float rotX, rotY, rotZ, rotW; // Rotação (quaternion)
} IPL_Instance;

#define MAX_OBJECTS 1024  // Número máximo de objetos carregados

IPL_Instance worldObjects[MAX_OBJECTS]; // Array de objetos carregados
int objectCount = 0;  // Contador de objetos

void loadIPL(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    char line[256];


    if(!file) 
    {
        printf("Erro ao abrir o arquivo IPL: %s\n", filename);
        return;
    }

    while(fgets(line, sizeof(line), file)) 
    {
        IPL_Instance obj;

        if(strncmp(line, "inst", 4) == 0) continue; // Ignorar cabeçalho
        if(strncmp(line, "end", 3) == 0) break; // Fim do arquivo

        sscanf(line, "%d, \"%[^\"]\", %f, %f, %f, %f, %f, %f, %f",
               &obj.id, obj.modelName, &obj.x, &obj.y, &obj.z,
               &obj.rotX, &obj.rotY, &obj.rotZ, &obj.rotW);

        if(objectCount < MAX_OBJECTS) 
        {
            worldObjects[objectCount++] = obj;
        }
    }

    fclose(file);
    printf("Carregado %d objetos do IPL!\n", objectCount);
}

void loadObjectsNear(float playerX, 
                     float playerY, 
                     float playerZ, 
                     float viewDistance) 
{
    int i = 0;

    for(i = 0; i < objectCount; i++) 
    {
        float dx = worldObjects[i].x - playerX;
        float dy = worldObjects[i].y - playerY;
        float dz = worldObjects[i].z - playerZ;
        float distance = dx * dx + dy * dy + dz * dz;

        if(distance <= viewDistance * viewDistance) 
        {
            printf("Carregando objeto %s na posição (%.2f, %.2f, %.2f)\n",
                   worldObjects[i].modelName, 
                   worldObjects[i].x, 
                   worldObjects[i].y, 
                   worldObjects[i].z);
            // Aqui você pode chamar a função para renderizar o objeto
        }
    }
}


GLuint vbo, ebo, vbo2, ebo2, lightVbo, lightEbo;
GLuint defaultShader, lightShader;

GLuint lighting;

//GLuint brickTexture;
//GLint texBrickLoc;

GLuint woodTexture;
GLint texWoodLoc;
GLuint woodSpec;
GLint texWoodSpecLoc;

Vector4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
Vector3 lightPos = {0.5f, 0.5f, 0.5f};  // Posição da luz
Vector3 pyramidPos = {1.5f, 1.5f, 1.5f}; // Posição do objeto



GLint posAttrib, posAttrib2, colAttrib, texAttrib, normalAttrib;

Matrix4x4 lightModel, pyramidModel;

GLuint shader1, shader2, shader3, shader4, shader5;

GLuint Swireframe;

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



//ObjModel modelObj;



void Setup()
{
    int i;

    Matrix4x4 translationObject;
    Matrix4x4 translationLight;

    
    //GLint posAttrib, colAttrib, texAttrib, pyramAttrib;
    //GLint lightPosAttrib;      // Atributo de posição da luz

    //GLuint vbo, ebo, texKitten, texPuppy;
    //GLint posAttrib, colorAttrib, texAttrib, normalAttrib;

    //GLuint vertex_shader = make_shader(GL_VERTEX_SHADER, "vertex_shader.vs");
    //GLuint fragment_shader = make_shader(GL_FRAGMENT_SHADER, "fragment_shader.fs");
    //program = make_program(vertex_shader, fragment_shader);
   // glUseProgram(program);  // Usar o programa uma vez para inicializar

/*
    if(!LoadOBJ("res/models/esun.obj", &modelObj)) 
    {
        LE_print(LE_COLOR_RED, "Erro ao carregar modelo OBJ.\n");
    }
    else
    {
        printf("Modelo carregado com sucesso!\n");
    }
*/




    testCamera = (RCamera *)malloc(sizeof(RCamera)); /* Manual Alloc */
    if (!testCamera) 
    {
        LE_print(LE_COLOR_RED, "\n Erro ao alocar memória para RCamera\n");
    }

    if (cameraSetup(testCamera, winWidth, winHeight) != 0) 
    {
        LE_print(LE_COLOR_RED, "Erro ao configurar a câmera\n");
        free(testCamera);
    }


    #define SPOTS_FRAG_DIR "res/shader/procedural_textures/spots.frag"
    #define SPOTS_VERT_DIR "res/shader/procedural_textures/spots.vert"
    #define FBMNOISE_FRAG_DIR "res/shader/procedural_textures/fbmnoise.frag"
    #define FBMNOISE_VERT_DIR "res/shader/procedural_textures/fbmnoise.vert"
    #define FLOWNOISE_FRAG_DIR "res/shader/procedural_textures/flownoise2.frag"
    #define FLOWNOISE_VERT_DIR "res/shader/procedural_textures/flownoise2.vert"
    #define SNOISE3_FRAG_DIR "res/shader/procedural_textures/snoise3.frag"
    #define SNOISE3_VERT_DIR "res/shader/procedural_textures/snoise3.vert"
    #define TILES_FRAG_DIR "res/shader/procedural_textures/tiles.frag"
    #define TILES_VERT_DIR "res/shader/procedural_textures/tiles.vert"

    createShader(&shader1, SPOTS_VERT_DIR, SPOTS_FRAG_DIR); /* pingos pretos e vermelhos */
    createShader(&shader2, FBMNOISE_VERT_DIR, FBMNOISE_FRAG_DIR); /* fogo ou lava */
    createShader(&shader3, FLOWNOISE_VERT_DIR, FLOWNOISE_FRAG_DIR);
    createShader(&shader4, SNOISE3_VERT_DIR, SNOISE3_FRAG_DIR);
    createShader(&shader5, TILES_VERT_DIR, TILES_FRAG_DIR);

    createShader(&lighting, "res/shader/gl_2_demo/lighting.vert", "res/shader/gl_2_demo/lighting.frag");

    woodTexture = load_texture_png("res/textures/planks.png",
                                    GL_RGBA,
                                    GL_UNSIGNED_BYTE);
    texWoodLoc = glGetUniformLocation(lighting, "0");

    woodSpec = load_texture_png("res/textures/planksSpec.png",
                                GL_RGBA,
                                GL_UNSIGNED_BYTE);
    texWoodSpecLoc = glGetUniformLocation(lighting, "1");
    





    
    //createShader(&defaultShader, "res/shader/default.vs", "res/shader/default.fs");
    createShader(&defaultShader, "res/shader/base_vertex_shader.vs", "res/shader/default.fs");
    createShader(&lightShader, "res/shader/light.vs", "res/shader/light.fs");

    /*
    
    // Criação dos Buffers
    vbo = make_buffer(GL_ARRAY_BUFFER, vertices, sizeof(vertices), GL_STATIC_DRAW);
    ebo = make_buffer(GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices), GL_STATIC_DRAW);

    */
    /*
    // Configuração dos atributos para a pirâmide
    glUseProgram(defaultShader);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        posAttrib = glGetAttribLocation(defaultShader, "vertPosition");
            glEnableVertexAttribArray(posAttrib);
            //glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)0);

        colAttrib = glGetAttribLocation(defaultShader, "vertColor");
            glEnableVertexAttribArray(colAttrib);
            //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        texAttrib = glGetAttribLocation(defaultShader, "vertTexCoords");
            glEnableVertexAttribArray(texAttrib);
            //glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

        normalAttrib = glGetAttribLocation(defaultShader, "vertNormal");
            glEnableVertexAttribArray(normalAttrib);
            //glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
    glUseProgram(0);
    */

    /*
    vbo2 = make_buffer(GL_ARRAY_BUFFER, lightVertices, sizeof(lightVertices), GL_STATIC_DRAW);
    ebo2 = make_buffer(GL_ELEMENT_ARRAY_BUFFER, lightIndices, sizeof(lightIndices), GL_STATIC_DRAW);

    // Configuração para o cubo de luz
    glUseProgram(lightShader);
        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        posAttrib2 = glGetAttribLocation(lightShader, "aPos");
            glEnableVertexAttribArray(posAttrib2);
            //glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glUseProgram(0);
    */


    // Configuração das matrizes de modelo
    //pyramidModel = identity_matrix();
    //lightModel = identity_matrix();



    // Translação para os cubos
    //translationObject = translation_matrix(pyramidPos.x, pyramidPos.y, pyramidPos.z);
    //pyramidModel = multiply_matrices(&pyramidModel, &translationObject);


    //translationLight = translation_matrix(lightPos.x, lightPos.y, lightPos.z);
    //lightModel = multiply_matrices(&lightModel, &translationLight);

    /*
    for(i = 0; i < 4; ++i) 
    {
        printf("pyramidModel row %d: %f %f %f %f\n", i, 
        pyramidModel.m44[i][0], pyramidModel.m44[i][1], 
        pyramidModel.m44[i][2], pyramidModel.m44[i][3]);
    }*/

    // textura da piramide
    //brickTexture = load_texture_png("res/textures/brick.png");
    //texBrickLoc = glGetUniformLocation(defaultShader, "tex0");
    // textura de madeira _ specular map

    woodTexture = load_texture_png("res/textures/planks.png",
                                    GL_RGBA,
                                    GL_UNSIGNED_BYTE);
    texWoodLoc = glGetUniformLocation(defaultShader, "tex0");

    woodSpec = load_texture_png("res/textures/planksSpec.png",
                                GL_RGBA,
                                GL_UNSIGNED_BYTE);
    texWoodSpecLoc = glGetUniformLocation(defaultShader, "tex1");

    /*
    glUseProgram(defaultShader);
    //glUniformMatrix4fv(glGetUniformLocation(defaultShader, "model_Matrix"), 1, GL_FALSE, &pyramidModel.m44[0][0]);
    glUniform4f(glGetUniformLocation(defaultShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(defaultShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUseProgram(0);
    */
    /*
    glUseProgram(lightShader);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, &lightModel.m44[0][0]);
    glUniform4f(glGetUniformLocation(lightShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUseProgram(0);
    */

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glUniform1i(texWoodLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, woodSpec);
    glUniform1i(texWoodSpecLoc, 1); 
    


    testCamera->cameraPos.x = 0.0f;
    testCamera->cameraPos.y = 0.0f;
    testCamera->cameraPos.z = 2.0f;

    //initNuklear();


    MODEL01 = limdl_model_new_from_file("res/models/aerbody1.lmdl", 1); /* 1 carregar malha completa */

    if(MODEL01 == NULL) 
    {
        LE_print(LE_COLOR_RED, " Erro ao carregar o modelo do arquivo\n");
    }
    else
    {
        
        LE_print(LE_COLOR_GREEN, " MODELO LMDL CARREGADO COM SUCESSO ! \n ");
    
        //print_limdl_model(MODEL01);
        buffers = uploadModelData(MODEL01);
        if(buffers.vbo && buffers.ebo) 
        {
            //renderModel(buffers, MODEL01->lod.array[0].indices.count);
        }

            // Libera recursos
        //glDeleteBuffers(1, &buffers.vbo);
        //glDeleteBuffers(1, &buffers.ebo);

    }
    
    /*
    for(i = 0; i < MODEL01->vertices.count; i++) 
    {
        LIMdlVertex* vertex = &MODEL01->vertices.array[i];
        
        printf("  Vertex %d: Pos(%f, %f, %f)\n", i, vertex->coord.x, 
                                                    vertex->coord.y, 
                                                    vertex->coord.z);
    } Demora no carregamento incial */

    //print_triangles_from_model(MODEL01);

    //MODEL01_VBO = uploadVertices(MODEL01);

    M01_skin = load_texture_png("res/textures/aer/skin1.png",
                                    GL_RGBA,
                                    GL_UNSIGNED_BYTE);

    createShader(&M01_texture_s, "res/shader/base_vertex_shader.vs", "res/shader/debug/textureModel.fs");


}


int endProgram()
{

    LE_print(LE_COLOR_BLUE, "\n Program Ends \n");
    uninitmouse();
    uninitkeyboard();
    uninitdirectinput();
    uninitJoystick(0);

    engineRender->cleanup(display);

    activeLoop = 0;
    //exit(0); /* f0rce program ends */

 return 0;
}

/*
JoystickContext *joy1_test;


DIJOYSTATE2 joyState;

#define DEADZONE 8000  // Ajuste o tamanho da zona morta (quanto mais alto, maior a área neutra)
#define SENSITIVITY 0.05f  // Sensibilidade dos eixos analógicos


int applyDeadZone(int value) 
{
    if(value > DEADZONE) 
    {
        return value - DEADZONE;  // Move para a direita
    }
    else if (value < -DEADZONE) 
    {
        return value + DEADZONE;  // Move para a esquerda
    }
    return 0;  // Dentro da zona morta, retorna zero
}

#define ROTATION_SENSITIVITY 0.05f  // Sensibilidade para a rotação




/*
int GetJoystickInput(int joystickIndex, DIJOYSTATE2 js)
{
    //DIJOYSTATE2 js; // Estrutura para armazenar o estado do joystick
    HRESULT hr;

    // Aplicando a centralização (subtraindo a posição central)
    long analogX = js.lX - 24511; // Subtrair a "centralização"
    long analogY = js.lY - 24511;

    // Deadzone (ajuste para não detectar pequenos movimentos)
    int deadZone = 2000; // Defina um valor de deadzone, ajustando conforme necessário

    // Multiplicando pela sensibilidade para ajustar a magnitude dos movimentos
    float sensitivity = 0.05f; // Ajuste a sensibilidade como precisar

    if(joystickIndex >= 8 || !gpJoysticks[joystickIndex]) 
    {
        return 0; // Índice inválido ou dispositivo inexistente
    }

    // Obtendo o estado do joystick
    hr = gpJoysticks[joystickIndex]->lpVtbl->GetDeviceState(gpJoysticks[joystickIndex],
                                                            sizeof(DIJOYSTATE2), 
                                                            &js);
    if(SUCCEEDED(hr))
    {

        // Aplicando deadzone: Se estiver dentro da faixa de deadzone, tratamos como 0
        if(abs(analogX) < deadZone) analogX = 0;
        if(abs(analogY) < deadZone) analogY = 0;

        // Modificando o yaw e pitch com base nos valores ajustados e sensibilidade
        testCamera->yaw += analogX * sensitivity; 
        testCamera->pitch -= analogY * sensitivity; // Inversão do sinal para controle mais usual

        // Restringir o pitch (Para evitar olhar para cima ou para baixo exageradamente)
        if(testCamera->pitch > 89.0f) testCamera->pitch = 89.0f;
        if(testCamera->pitch < -89.0f) testCamera->pitch = -89.0f;
    }

    return(SUCCEEDED(hr));
}*/

/*
int GetJoystickInput(int joystickIndex, DIJOYSTATE2* js, long* joyX, long* joyY, long* joyZ, long* joyW)
{
    HRESULT hr;

    // Deadzone (ajuste para não detectar pequenos movimentos)
    int deadZone = 2000; // Defina um valor de deadzone, ajustando conforme necessário

    // Multiplicando pela sensibilidade para ajustar a magnitude dos movimentos
    float sensitivity = 0.05f; // Ajuste a sensibilidade como precisar

    if (joystickIndex >= 8 || !gpJoysticks[joystickIndex]) 
    {
        return 0; // Índice inválido ou dispositivo inexistente
    }

    // Obtendo o estado do joystick
    hr = gpJoysticks[joystickIndex]->lpVtbl->GetDeviceState(gpJoysticks[joystickIndex],
                                                            sizeof(DIJOYSTATE2),
                                                            js);
    if (SUCCEEDED(hr))
    {
        // Centralização e aplicação de deadzone no eixo X e Y
        *joyX = js->lX - 32511;
        *joyY = js->lY - 32511;

        if(abs(*joyX) < deadZone) *joyX = 0;
        if(abs(*joyY) < deadZone) *joyY = 0;

        // Centralização para Z e W
        *joyZ = js->lZ - 32511; // Ajuste conforme necessário para outros eixos
        *joyW = js->lRz - 32511; // Eixo
    
        if(abs(*joyZ) < deadZone) *joyZ = 0;
        if(abs(*joyW) < deadZone) *joyW = 0;
    
        // Aplicação de sensibilidade (se necessário)
        //*joyX = (long)(*joyX * sensitivity);
        //*joyY = (long)(*joyY * sensitivity);
        //*joyZ = (long)(*joyZ * sensitivity);
        //*joyW = (long)(*joyW * sensitivity);

    }

    return SUCCEEDED(hr); // Retorna sucesso ou falha
}

// Função para mapear a intensidade do joystick para um movimento controlado
float mapJoystickToMovementSpeed(long joyValue, int deadZone, float maxSpeed)
{
    float normalizedValue = 0.0f;
    float mappedSpeed = 0.0f;

    // Verifica se o valor está dentro da dead zone (zona morta)
    if(abs(joyValue) < deadZone) 
    {
        return 0.0f;  // Nenhum movimento se dentro da dead zone
    }

    // Ajuste o valor do joystick para que a movimentação comece após a dead zone
    if (joyValue > 0)
    {
        joyValue -= deadZone;  // Subtrai a deadzone para valores positivos
    }
    
    else
    {
        joyValue += deadZone;  // Soma a deadzone para valores negativos
    }
    
    // Normaliza o valor para o intervalo de -1 a 1, ajustando pela gama total de valores possíveis
    normalizedValue = (float)joyValue / (32767 - deadZone);  // Max valor de entrada do joystick

    // Suaviza a resposta (quadrático para diminuir a resposta em pequenos movimentos)
    mappedSpeed = normalizedValue * normalizedValue * maxSpeed;

    // Aplique uma correção para lidar com os sinais corretamente (para ambos os positivos e negativos)
    if(joyValue < 0) 
    {    
        mappedSpeed = -mappedSpeed;  // Mantém o sinal negativo
    }

    // Limita a velocidade máxima para evitar movimentos muito rápidos
    if(mappedSpeed > maxSpeed)
    {
        mappedSpeed = maxSpeed;
    }
    
    if(mappedSpeed < -maxSpeed)
    {
        mappedSpeed = -maxSpeed;
    }

    return mappedSpeed;
}
*/

void inputEvent() 
{
    int i;
    int inputDetected = 0;
    // Aplica a zona morta aos eixos
    /*int analogLeftX = applyDeadZone(joyState.lX);
    int analogLeftY = applyDeadZone(joyState.lY);
    int analogRightZ = applyDeadZone(joyState.lZ);
    int analogRightW = applyDeadZone(joyState.lRz);*/

    //long joyX, joyY, joyZ, joyW;

    // Usando a função de mapeamento para obter a velocidade de movimento baseada no joystick
    //float adjustedJoyZ = 0;
    //float adjustedJoyW = 0;

    /* Dualshock 5 test
    
        quadrado = 0
        botao x = 1
        circulo = 2
        triangulo = 3

        botao seta cima = POV 0 movido: 0 graus
        botao seta direita = POV 0 movido: 9000 graus
        botao seta baixo = POV 0 movido: 18000 graus
        botao seta esquerda = POV 0 movido: 27000 graus
   
        L1 = botao 4
        L2 = botão 6

        R1 = botao 5 
        R2 = botao 7

        botao playstation = 12

        botao config = 9

        mute = 14

        botao create = 8

        botao do touch = 13

        R3 = 11
        L3 = 10

        gatilho R2 = rotação Y
        gatilho L2 = rotação X
    
        analogico esquerdo = eixo X e Y;

        analogico direito = eixo Z e rotação Z

        eixo X é do analogico esquerdo, que comanda esquerda e direita, sendo o minimo 0 e o maximo 57535.
        eixo Y é do analogico esquerdo, que comanda cima e baixo, sendo o minimo 0 e o maximo 57535.

        eixo Z é do analogico direito, que comanda esquerda e direita, sendo o minimo 0 e o maximo 57535.
        eixo W é do analogico direito, que comanda cima e baixo , sendo o minimo 0 e o maximo 57535.





    */
    /*
    if(GetJoystickInput(0, &joyState, &joyX, &joyY, &joyZ, &joyW))
    {
        //printf("Joystick X: %ld, Y: %ld, Z: %ld, W: %ld\n", joyX, joyY, joyZ, joyW);

        //testCamera->yaw += joyZ * SENSITIVITY;
        //testCamera->pitch -= joyW * SENSITIVITY;

        adjustedJoyZ = mapJoystickToMovementSpeed(joyZ, 2000, 60.0f); // Para Z (Yaw)
        adjustedJoyW = mapJoystickToMovementSpeed(joyW, 2000, 40.0f); // Para W (Pitch)

            // Se houver movimento significativo (não zero)
        if (adjustedJoyZ != 0.0f || adjustedJoyW != 0.0f)
        {
            // Move a câmera
            testCamera->yaw += adjustedJoyZ * SENSITIVITY; // Incremento do Yaw
            testCamera->pitch -= adjustedJoyW * SENSITIVITY; // Incremento do Pitch

            // Limitação do Pitch para evitar que o valor ultrapasse limites extremos
            if (testCamera->pitch > 89.0f) 
                testCamera->pitch = 89.0f;
            if (testCamera->pitch < -89.0f) 
                testCamera->pitch = -89.0f;
        }


 /*       // Limitar o pitch
        if(testCamera->pitch > 89.0f) 
        {
            testCamera->pitch = 89.0f;
        }
        
        if(testCamera->pitch < -89.0f) 
        {
            testCamera->pitch = -89.0f;
        }*

    }
    else 
    {
        //printf("Falha ao ler o estado do Joystick 0.\n");
    }

    if(readJoystick(0, &joyState)) 
    {
        // Verifica movimentos nos eixos principais
        //if(analogLeftX != 0) 
        if(analogLeftX <= 23000) 
        {
            //printf("Eixo X movido: %ld\n", analogLeftX);
            testCamera->camLeft = 1;
            inputDetected = 1;
        }
        else if(analogLeftX >= 26000)
        {
            testCamera->camRight = 1;
            inputDetected = 1;
        }

        //if(analogLeftY != 0) 
        if(analogLeftY <= 23000)
        {
            //printf("Eixo Y movido: %ld\n", analogLeftY);
            //testCamera->camForward = analogLeftY * SENSITIVITY; // Movimentação frente-trás
            testCamera->camForward = 1;
            inputDetected = 1;
        }
        else if(analogLeftY >= 26000)
        {
            testCamera->camBackWard = 1;
            inputDetected = 1;
        }

        //if(analogRightZ != 0)
        if(joyState.lZ != 0)
        {
            //printf("Eixo Z movido: %ld\n", analogRightZ);

            inputDetected = 1;
        }

        // Verifica movimentos nos eixos rotacionais
        //if(analogRightW != 0) 
        if(joyState.lRz != 0)
        {
            //printf("Eixo W movida: %ld\n", analogRightW);
            inputDetected = 1;
        }

        //cameraInputJStick(testCamera, analogRightW, analogRightZ, deltaTime);

        // Verifica os botões pressionados, por exemplo
        if (joyState.rgbButtons[0] & 0x80) 
        {  // Botão quadrado pressionado
            //printf("Botão [] pressionado\n");
            testCamera->camElevate = 1;  // Elevar a câmera
            inputDetected = 1;
        }

        if (joyState.rgbButtons[1] & 0x80) 
        {  // Botão X pressionado
            //printf("Botão X pressionado\n");
            testCamera->camLower = 1;  // Abaixar a câmera
            inputDetected = 1;
        }

        if (joyState.rgbButtons[2] & 0x80) 
        {  // Botão Circulo pressionado
            //printf("Botão O pressionado\n");
            testCamera->camSprint = 1;
            inputDetected = 1;
        }

        if (joyState.rgbButtons[3] & 0x80) 
        {  // Botão Triangulo pressionado
            //printf("Botão /\\ pressionado\n");
            inputDetected = 1;
        }

        // Adicione outras verificações de botões aqui...

        // Se nenhum movimento foi detectado
        if (!inputDetected) {
            printf("Nenhum input detectado.\n");
        }
    } 
    else 
    {
        //printf("Falha ao ler o estado do Joystick 0.\n");
    }
/*
if (readJoystick(0, &joyState)) 
{


    // Verifica botões pressionados
    for(i = 0; i < 128; i++) 
    {
        if (joyState.rgbButtons[i] & 0x80) 
        {
            printf("Botão %d pressionado.\n", i);
            inputDetected = 1;
        }
    }

    // Verifica movimentos nos eixos principais
    if (joyState.lX != 0) 
    {
        printf("Eixo X movido: %ld\n", joyState.lX);
        inputDetected = 1;
    }

    if (joyState.lY != 0) 
    {
        printf("Eixo Y movido: %ld\n", joyState.lY);
        inputDetected = 1;
    }

    if (joyState.lZ != 0) 
    {
        printf("Eixo Z movido: %ld\n", joyState.lZ);
        inputDetected = 1;
    }

    // Verifica movimentos nos eixos rotacionais
    if (joyState.lRx != 0) 
    {
        printf("Rotação X: %ld\n", joyState.lRx);
        inputDetected = 1;
    }

    if (joyState.lRy != 0) 
    {
        printf("Rotação Y: %ld\n", joyState.lRy);
        inputDetected = 1;
    }

    if (joyState.lRz != 0) 
    {
        printf("Rotação Z: %ld\n", joyState.lRz);
        inputDetected = 1;
    }

    // Verifica sliders
    if (joyState.rglSlider[0] != 0) 
    {
        printf("Slider 1: %ld\n", joyState.rglSlider[0]);
        inputDetected = 1;
    }

    if (joyState.rglSlider[1] != 0) 
    {
        printf("Slider 2: %ld\n", joyState.rglSlider[1]);
        inputDetected = 1;
    }

    // Verifica os POVs (Hat Switch)
    for(i = 0; i < 4; i++) 
    {
        if (joyState.rgdwPOV[i] != -1) 
        {
            printf("POV %d movido: %lu graus\n", i, joyState.rgdwPOV[i]);
            inputDetected = 1;
        }
    }

    // Se nenhum input foi detectado
    if(!inputDetected) 
    {
        printf("Nenhum input detectado.\n");
    }
} 
else 
{
    printf("Falha ao ler o estado do Joystick 0.\n");
}
*/

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

    cameraInput(testCamera, mouseX, mouseY, deltaTime);

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
        //cameraSpeed *= sprintMultiplier; // Aumenta a velocidade
        testCamera->camSprint = 1;
    }

    //nuklearInput(ctx, window->platformHandle);
    

}


void Render() 
{
    int i;
    Vector3 vertex;
    Vector3 transformed;
    Vector4 white;
    Vector4 zeros;
    Vector4 m_vLightPos;

    // Configuração de matrizes
   // Matrix4x4 view;
    //Matrix4x4 proj;

    Matrix4x4 model = identity_matrix();
    float time = (float)clock() / CLOCKS_PER_SEC;
    
    //Matrix4x4 rotation = rotate_z(time * PI / 4.0f); // Rotação no tempo
    //Matrix4x4 rotationX = rotate_x(time * PI / 4.0f);
    //Matrix4x4 rotationY = rotate_y(time * PI / 4.0f);
    //Matrix4x4 rotationZ = rotate_z(time * PI / 4.0f);

    float timer = 0.0f;
    GLint timeLocation;


    if(keyInputDInput(KEY_LCTRL, KEEP) && keyInputDInput(KEY_E, KEEP))
    {
        glClearColor(0.0f, 0.19f, 0.06f, 1.0f);
    }
    else
    {
        glClearColor(0.0f, 0.05f, 0.07f, 1.0f);
        //glClearColor(bg.r, bg.g, bg.b, bg.a);
    }

    /* Limpa a tela e o buffer de profundidade */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 
/*
    //model = multiply_matrices(&model, &rotation); // Rotação no tempo
    // Combine as rotações
    //model = multiply_matrices(&rotationX, &model);
    //model = multiply_matrices(&rotationY, &model);
    //model = multiply_matrices(&rotationZ, &model);


*/
    cameraMatrix(testCamera, 75.0f, 0.1f, 100.0f);

     //no shaders
    sendCameraToRender(testCamera, 0, NULL); // no shaders

    glMatrixMode(GL_MODELVIEW);
    glMultMatrixf(&model.m44[0][0]); // Multiplicar pelo modelo
    


    // Envio das matrizes para o shader
    //glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model.m44[0][0]);
    //glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view.m44[0][0]);
    //glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, &proj.m44[0][0]);

    // No Render
    //printf("Matriz de Projeção:\n");
    //debug_matrix(&proj);

    //printf("Matriz de Visão:\n");
    //debug_matrix(&view);

    //printf("Matriz de Modelo:\n");
    //debug_matrix(&model);




    //RenderModel(&modelObj);


    
    glUseProgram(defaultShader); 
    // Send camera view projection to shader
    glUniformMatrix4fv(glGetUniformLocation(defaultShader, "proj_Matrix"), 1, GL_FALSE, &testCamera->proj.m44[0][0]);
    // Exports the camera Position to the Fragment Shader for specular lighting
    glUniform3f(glGetUniformLocation(defaultShader, "camPos"), testCamera->cameraPos.x, 
                                                               testCamera->cameraPos.y, 
                                                               testCamera->cameraPos.z);
    //glUniformMatrix4fv(glGetUniformLocation(defaultShader, "model_Matrix"), 1, GL_FALSE, &pyramidModel.m44[0][0]);
    
    // Export the camMatrix to the Vertex Shader of the pyramid
    glUniformMatrix4fv(glGetUniformLocation(defaultShader, "view_Matrix"), 1, GL_FALSE, &testCamera->view.m44[0][0]);
    glUniform3f(glGetUniformLocation(defaultShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform4f(glGetUniformLocation(defaultShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));

    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
    */
    drawPlane(-5.0f, 1.0f, -5.0f, 2.0f, 1);

    glUseProgram(0); 
    


    // Renderiza o cubo pequeno (luz)
    /*
    glUseProgram(lightShader);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "projection"), 1, GL_FALSE, &testCamera->proj.m44[0][0]);
    //glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, &lightModel.m44[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "camMatrix"), 1, GL_FALSE, &testCamera->view.m44[0][0]);
    //glUniform4f(glGetUniformLocation(lightShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);

    glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

    glUseProgram(0); 
    */
    




    //glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    glUseProgram(shader5);
    timeLocation = glGetUniformLocation(shader5, "time");
    if(timeLocation != -1) 
    {
        timer = (float)getElapsedTimeInSeconds();
        glUniform1f(timeLocation, timer);
        //glUniform1fv(timeLocation, 1, &timer);
    }

    drawCube(-1.0f, 0.0f, 0.0f); // Cubo à esquerda

    glUseProgram(0); 

    // Desenha o segundo cubo
    //glColor3f(0.0f, 0.0f, 1.0f); // Azul
    glUseProgram(shader4);
    timeLocation = glGetUniformLocation(shader4, "time");
    if(timeLocation != -1) 
    {
        timer = (float)getElapsedTimeInSeconds();
        glUniform1f(timeLocation, timer);
        //glUniform1fv(timeLocation, 1, &timer);
    }

    drawCube(1.0f, 0.0f, 0.0f);  // Cubo à direita

    glUseProgram(0);  

    //glColor3f(1.4f, 1.2f, 1.1f);
    glUseProgram(shader2);
    timeLocation = glGetUniformLocation(shader2, "time");
    if(timeLocation != -1) 
    {
        timer = (float)getElapsedTimeInSeconds();
        glUniform1f(timeLocation, timer);
        //glUniform1fv(timeLocation, 1, &timer);
    }
        drawTexturedSphere(1.0f, 36, 4.0f, 5.0f, -4.0f);
    glUseProgram(0);    


    // Desenha um plano em pé
    drawPlane(0.0f, 0.0f, 0.0f, 1.0f, 0);

    glUseProgram(shader3);
    timeLocation = glGetUniformLocation(shader3, "time");
    if(timeLocation != -1) 
    {
        timer = (float)getElapsedTimeInSeconds();
        glUniform1f(timeLocation, timer);
        //glUniform1fv(timeLocation, 1, &timer);
    }

    // Desenha um plano deitado
    drawPlane(3.0f, 0.0f, -5.0f, 2.0f, 1);

    glUseProgram(0);

    // Desenha um plano em pé no YZ em (-3, -1, -5) com escala 0.5
    drawPlane(-3.0f, -1.0f, -5.0f, 0.5f, 2);

    
    //glColor3f(1.5f, 1.5f, 1.5f);
    glColor3f(0.5f, 0.5f, 0.5f);
    drawPlane(-3.0f, 1.0f, -2.0f, 2.0f, 1);

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5f, -0.5f, -1.0f);
        glVertex3f(0.5f, -0.5f, -1.0f);
        glVertex3f(0.0f, 0.5f, -1.0f);
    glEnd();

    glUseProgram(shader1);
    timeLocation = glGetUniformLocation(shader1, "time");
    if(timeLocation != -1) 
    {
        timer = (float)getElapsedTimeInSeconds();
        glUniform1f(timeLocation, timer);
        //glUniform1fv(timeLocation, 1, &timer);
    }

    drawTexturedSphere(1.0f, 36, 2.0f, 8.5f, -4.0f);

    glUseProgram(0);


    // nuklearUpdate(); //imediate


    renderModel(buffers, M01_texture_s, M01_skin);



    glFlush();
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
    }

    /*
    if(!initJoystick(window->platformHandle, 0)) 
    {
        LE_print(LE_COLOR_RED, "Erro ao inicializar o sistema de JoyStick.\n");
    }
    else
    {
        LE_print(LE_COLOR_BLUE, "JOY INICIADO .\n");
    }*/

    /*
    for(i = 0; i < 8; i++) 
    {
        if(!initJoystick(window->platformHandle, i)) 
        {
            LE_print(LE_COLOR_YELLOW, "\n Joystick n: %d faltando " , i);
            //printf("Falha ao inicializar Joystick %d.\n", i);
            LE_print(LE_COLOR_RED, " Erro ao inicializar o sistema de JoyStick.\n");
        }
    }*/

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
