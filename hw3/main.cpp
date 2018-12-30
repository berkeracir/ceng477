#include "helper.h"
#include <stdlib.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/ext.hpp>
#include "linmath.h"

using namespace std;

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

GLuint projectionMatrixLoc;
GLuint viewingMatrixLoc;
GLuint modelingMatrixLoc;
GLuint eyeLoc;

int widthTexture, heightTexture;
int prevWinWidth, prevWinHeight, prevWinXpos, prevWinYpos;
bool fullscreen = false;
unsigned int triangle_count;

GLuint vertexBuffer;

glm::vec3 eye, gaze, up;

float *triangle_data;

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    if (!fullscreen) {
      fullscreen = true;
      glfwGetWindowSize(window, &prevWinWidth, &prevWinHeight);
      glfwGetWindowPos(window, &prevWinXpos, &prevWinYpos);
      GLFWmonitor *monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode *mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else {
      fullscreen = false;
      glfwSetWindowMonitor(window, NULL, prevWinXpos, prevWinYpos, prevWinWidth, prevWinHeight, 0);
    }
  }
  else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void initData() {
  eye = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  gaze = glm::vec3(0.0f, 0.0f, 1.0f);
  up = glm::vec3(0.0f, 1.0f, 0.0f);

  /*unsigned int *indexes = new unsigned int[triangle_count*3];
  unsigned int indexes_size = triangle_count*3*sizeof(unsigned int);
  unsigned int i = 0;
  for (int z=0; z < heightTexture; z++) {
    for (int x=0; x < widthTexture; x++) {
      indexes[i++] = x + z*(widthTexture+1);
      indexes[i++] = (x+1) + (z+1)*(widthTexture+1);
      indexes[i++] = x + (z+1)*(widthTexture+1);
      indexes[i++] = x + z*(widthTexture+1);
      indexes[i++] = (x+1) + z*(widthTexture+1);
      indexes[i++] = (x+1) + (z+1)*(widthTexture+1);
    }
  }*/
  unsigned long data_size = triangle_count*3*3*sizeof(float);
  unsigned int data_index = 0;
  for (int z=0; z < heightTexture; z++) {
    for (int x=0; x < widthTexture; x++) {
      triangle_data[data_index++] = (float) x;
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) z;

      triangle_data[data_index++] = (float) (x+1);
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) (z+1);

      triangle_data[data_index++] = (float) x;
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) (z+1);

      triangle_data[data_index++] = (float) x;
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) z;

      triangle_data[data_index++] = (float) (x+1);
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) z;

      triangle_data[data_index++] = (float) (x+1);
      triangle_data[data_index++] = (float) 0;
      triangle_data[data_index++] = (float) (z+1);
    }
  }

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, data_size, triangle_data, GL_STATIC_DRAW);

  delete[] triangle_data;
}

int main(int argc, char * argv[]) {
  GLint vpos_location, vtex_location;

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);
  glfwSetKeyCallback(win, key_callback);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);
  triangle_count = 2*widthTexture*heightTexture;
  triangle_data = new float[triangle_count*9];
  initData();

  projectionMatrixLoc = glGetUniformLocation(idProgramShader, "projectionMatrix");
  viewingMatrixLoc = glGetUniformLocation(idProgramShader, "viewingMatrix");
  modelingMatrixLoc = glGetUniformLocation(idProgramShader, "modelingMatrix");
  eyeLoc = glGetUniformLocation(idProgramShader, "eye");
  //vpos_location = glGetAttribLocation(idProgramShader, "vPos");

  //glEnableVertexAttribArray(vpos_location);
  //glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void *) 0);
  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(win)) {
    float ratio;
    int width, height;
    mat4x4 m, p, v, mvp;

    glfwGetFramebufferSize(win, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 projectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
    GLdouble n = 0.1f;

    glm::vec3 cameraUp = glm::vec3(0, 1, 0);
    glm::vec3 cameraGaze = glm::vec3(0, 0, 1);
    glm::vec3 cameraPosition = glm::vec3(widthTexture / 2, widthTexture / 10, (-1) * (widthTexture / 4));
    glm::vec3 center = glm::vec3(widthTexture / 2, widthTexture / 10, (-1) * (widthTexture / 4) + 0.1);

    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, center, cameraUp);

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count);
    glDisableClientState(GL_VERTEX_ARRAY);

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}