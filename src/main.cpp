//SDL Libraries           //En OpenGL, 2 processeurs distint (CPU et GPU) doivent comuniquer entre eux
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

#include <stack>
#include <vector>

#include "Sphere.h"
#include <SDL2/SDL_image.h>

#define WIDTH     600
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))
/////

struct Material {
    float k1;
	float k2;
	float k3;
    GLfloat vColor[3];
     
};
struct Light {
	GLfloat vColor[3];
	GLfloat vPosition[3];
};
	


	

struct GameObject {
    GLuint vboID = 0;
    Geometry* geometry = nullptr;
    glm::mat4 propagatedMatrix = glm::mat4(1.0f);
    glm::mat4 localMatrix = glm::mat4(1.0f);
    std::vector<GameObject*> children;
};

void draw(GameObject& go, Shader* shader, std::stack<glm::mat4>& matrices) {
    matrices.push(matrices.top() * go.propagatedMatrix);
    glm::mat4 mvp = matrices.top() * go.localMatrix;

    glUseProgram(shader->getProgramID());
    {
        glBindBuffer(GL_ARRAY_BUFFER, go.vboID);

        //VBO
        GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vPosition);

        GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");
        glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(go.geometry->getNbVertices() * 3 * sizeof(float)));
        glEnableVertexAttribArray(vColor);

        GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
        glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLES, 0, go.geometry->getNbVertices());
    }
    glUseProgram(0);

    for (int i = 0; i < go.children.size(); i++)
        draw(*(go.children[i]), shader, matrices);

    matrices.pop();
}

int main(int argc, char* argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////

    //Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("VR Camera",                           //Titre
        SDL_WINDOWPOS_UNDEFINED,               //X Position
        SDL_WINDOWPOS_UNDEFINED,               //Y Position
        WIDTH, HEIGHT,                         //Resolution
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

//Initialize OpenGL Version (version 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
    SDL_GLContext context = SDL_GL_CreateContext(window);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test

    ///////////////////TODO_BEGINING///////////////////////////////////////
    /////////////////////////TD_2//////////////////////////////////////////
    /*
    GLuint vboID;
    glGenBuffers(1, &vboID);  //generate buffer

    GLfloat vPosition[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };
    GLfloat vColor[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 1.0f,
    };

    const unsigned int nbVertices = 4;

    glBindBuffer(GL_ARRAY_BUFFER, vboID); //engage un buffer
        glBufferData(GL_ARRAY_BUFFER, nbVertices*(3+3)*sizeof(float), nullptr, GL_DYNAMIC_DRAW); //3+3=6 données par points, crée buffer vide de la bonne taille
        glBufferSubData(GL_ARRAY_BUFFER, 0, nbVertices * 3 * sizeof(float), vPosition); //on gere les sommets, pt depart, taille
        glBufferSubData(GL_ARRAY_BUFFER, nbVertices * 3 * sizeof(float), nbVertices * 3 * sizeof(float), vColor); //on gere les couleurs, pt depart, taille
    glBindBuffer(GL_ARRAY_BUFFER, 0); //close buffer, pas de buffer courrant donc protège des erreurs...
    */
    /////////////////////////TD_2//////////////////////////////////////////

    /////////////////////////TD_3//////////////////////////////////////////
    /*Sphere sphere(32, 32);

    GLuint vboShereID;
    glGenBuffers(1, &vboShereID);
    glBindBuffer(GL_ARRAY_BUFFER, vboShereID);
    glBufferData(GL_ARRAY_BUFFER, sphere.getNbVertices() * (3 + 3) * sizeof(float), nullptr, GL_DYNAMIC_DRAW); //3+3=6 données par points, crée buffer vide de la bonne taille
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere.getNbVertices() * 3 * sizeof(float), sphere.getVertices());
    glBufferSubData(GL_ARRAY_BUFFER, sphere.getNbVertices() * 3 * sizeof(float), sphere.getNbVertices() * 3 * sizeof(float), sphere.getNormals());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GameObject sunGO;
    sunGO.vboID = vboShereID;
    sunGO.geometry = &sphere;

    GameObject earthGO;
    earthGO.vboID = vboShereID;
    earthGO.geometry = &sphere;

    sunGO.children.push_back(&earthGO);

    const char* vertexPath = "Shaders/color.vert";
    const char* fragPath = "Shaders/color.frag";

    FILE* vertexFile = fopen(vertexPath, "r"); //"r" to read
    FILE* fragFile = fopen(fragPath, "r");

    Shader* shader = Shader::loadFromFiles(vertexFile, fragFile); //create shader
    fclose(vertexFile);
    fclose(fragFile);

    if (!shader) {
        std::cerr << "The shader is broken... from loading vertxFile and fragFile" << std::endl;
        return EXIT_FAILURE;
    }

    float t = 0;
    */
    ///////////////////////////////////////TD4//////////////////////////////////////////////////////////
	
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        ERROR("Could not load SDL2_image with PNG files\n");
        return EXIT_FAILURE;
    }
	SDL_Surface* img= IMG_Load("Images/chat.png");
    SDL_Surface* rgbImg = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(img);
	
    GLuint textureID;
    glGenTextures(1, &textureID);
	
    glBindTexture(GL_TEXTURE_2D, textureID);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbImg->w, rgbImg->h, 0,GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbImg->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
		
	}
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
		
	
	
    ////////////////////////////////////TD4////////////////////////////////////////////////////////////
	
    ///////////////////TODO_END///////////////////////////////////////

    bool isOpened = true;

    //Main application loop
    while (isOpened) //affichage
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        //Fetch the SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    isOpened = false;
                    break;
                default:
                    break;
                }
                break;

            case SDL_KEYUP:
                isOpened = false;
                break;
                break;
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ///////////////////TODO_BEGINING_AFFICHAGE///////////////////////////////////////
        //     /////////////////////////TD_2//////////////////////////////////////////

        /*
        //3)
        glm::mat4 camera(1.0f); //create identity matrix
        glm::mat4 projection(1.0f);
        glm::mat4 model(1.0f);
        //model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 mvp = projection * camera * model;


        glUseProgram(shader->getProgramID()); //getProgramID() returns the Shader program ID in the GPU.
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
                //2.3) parameterize VBO
                GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
                glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); //2eme0 car position au début
                glEnableVertexAttribArray(vPosition);

                GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");
                glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(nbVertices*3*sizeof(float))); //car color a la fin
                glEnableVertexAttribArray(vColor);

                //2.4) parameterize Uniforms
                //GLint uScale = glGetUniformLocation(shader->getProgramID(), "uScale");
                //glUniform1f(uScale, 0.5f); //resize
                //3.4)
                GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
                glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp)); //do all the transformation in one time

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        */
        /////////////////////////TD_3//////////////////////////////////////////
        /*
        glm::mat4 projection(1.0f);
        glm::mat4 view(1.0f);

        glm::mat4 vp = projection * view;

        sunGO.localMatrix = sunGO.propagatedMatrix = glm::mat4(1.0f);
        sunGO.localMatrix = glm::scale(sunGO.localMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        sunGO.propagatedMatrix = glm::rotate(sunGO.propagatedMatrix, t, glm::vec3(0.0f, 1.0f, 0.0f));

        earthGO.localMatrix = earthGO.propagatedMatrix = glm::mat4(1.0f);
        earthGO.propagatedMatrix = glm::translate(earthGO.propagatedMatrix, glm::vec3(-0.75f, 0.0f, 0.0f));
        earthGO.localMatrix = glm::scale(earthGO.localMatrix, glm::vec3(0.15f, 0.15f, 0.15f));

        t += 0.1f;

        std::stack<glm::mat4> matrices;
        matrices.push(vp);

        draw(sunGO, shader, matrices);*/
        /////////////////////////TD_4//////////////////////////////////////////
		
       /* glUseProgram(shader->getProgramID());
        {
            glBindBuffer(GL_ARRAY_BUFFER, go.vboID);

            //VBO
            GLint vPosition = glGetAttribLocation(shader->getProgramID(), "MaterialCst");
            glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(vPosition);

            GLint vColor = glGetAttribLocation(shader->getProgramID(), "MaterialColor");
            glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(go.geometry->getNbVertices() * 3 * sizeof(float)));
            glEnableVertexAttribArray(vColor);

            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uLightPosition");
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
			
            nt uMVP = glGetUniformLocation(shader->getProgramID(), "uLightColor");
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
			
            nt uMVP = glGetUniformLocation(shader->getProgramID(), "uCameraPos");
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));

            glDrawArrays(GL_TRIANGLES, 0, go.geometry->getNbVertices());
        }
        glUseProgram(0);
        */
        ///////////////////TODO_END_AFFICHAGE///////////////////////////////////////


        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if (timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay((uint32_t)(TIME_PER_FRAME_MS)-(timeEnd - timeBegin));
    }

    //glDeleteBuffers(1, &vboShereID);
    /*delete shader;*/
    SDL_FreeSurface(rgbImg);

    //Free everything
    if (context != NULL)
        SDL_GL_DeleteContext(context);
    if (window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
