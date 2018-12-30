#include "helper.h"
#include <stdlib.h>
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/ext.hpp>

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
float fovy, ratio, near, far;
float heightFactor, delta_heightFactor = 0.25f;
float yaw, pitch, roll, delta_angle = 1.0 * M_PI/180;
float speed, delta_speed = 0.1f;

glm::mat4 yaw_positive_rotation_matrix, yaw_negative_rotation_matrix;
glm::mat4 pitch_positive_rotation_matrix, pitch_negative_rotation_matrix;
glm::mat4 roll_positive_rotation_matrix, roll_negative_rotation_matrix;

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
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else {
      fullscreen = false;
      glfwSetWindowMonitor(window, NULL, prevWinXpos, prevWinYpos, prevWinWidth, prevWinHeight, 0);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
  else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    heightFactor += delta_heightFactor;
  }
  else if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      heightFactor -= delta_heightFactor;
  }
  else if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    speed += delta_speed;
  }
  else if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    speed -= delta_speed;
  }
  else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(yaw_positive_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(yaw_positive_rotation_matrix * glm::vec4(up, 1.0f));
  }
  else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(yaw_negative_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(yaw_negative_rotation_matrix * glm::vec4(up, 1.0f));
  }
  else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(pitch_negative_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(pitch_negative_rotation_matrix * glm::vec4(up, 1.0f));
  }
  else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(pitch_positive_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(pitch_positive_rotation_matrix * glm::vec4(up, 1.0f));
  }
  else if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(roll_positive_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(roll_positive_rotation_matrix * glm::vec4(up, 1.0f));
  }
  else if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    gaze = glm::vec3(roll_negative_rotation_matrix * glm::vec4(gaze, 1.0f));
    up = glm::vec3(roll_negative_rotation_matrix * glm::vec4(up, 1.0f));
  }
}

void initData() {  
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

  eye = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  gaze = glm::vec3(0.0f, 0.0f, 1.0f);
  up = glm::vec3(0.0f, 1.0f, 0.0f);

  fovy = 45.0f;
  ratio = 1.0f;
  near = 0.1f;
  far = 10000.0f;

  heightFactor = 10.0f;
  yaw = 0.0f;
  pitch = 0.0f;
  roll = 0.0f;
  speed = 0.0f;

  glm::mat4 identity_matrix = glm::mat4(1.0f);
  yaw_positive_rotation_matrix = glm::rotate(identity_matrix, delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
  yaw_negative_rotation_matrix = glm::rotate(identity_matrix, -delta_angle, glm::vec3(0.0f, 1.0f, 0.0f));
  pitch_positive_rotation_matrix = glm::rotate(identity_matrix, delta_angle, glm::vec3(1.0f, 0.0f, 0.0f));
  pitch_negative_rotation_matrix = glm::rotate(identity_matrix, -delta_angle, glm::vec3(1.0f, 0.0f, 0.0f));
  roll_positive_rotation_matrix = glm::rotate(identity_matrix, delta_angle, glm::vec3(0.0f, 0.0f, 1.0f));;
  roll_negative_rotation_matrix = glm::rotate(identity_matrix, -delta_angle, glm::vec3(0.0f, 0.0f, 1.0f));;

  projectionMatrixLoc = glGetUniformLocation(idProgramShader, "projectionMatrix");
  viewingMatrixLoc = glGetUniformLocation(idProgramShader, "viewingMatrix");
  modelingMatrixLoc = glGetUniformLocation(idProgramShader, "modelingMatrix");
  eyeLoc = glGetUniformLocation(idProgramShader, "eye");

  GLint samplerLoc = glGetUniformLocation(idProgramShader, "rgbTexture");
  glUniform1i(samplerLoc, 0);

  GLint widthTextureLoc = glGetUniformLocation(idProgramShader, "widthTexture");
  glUniform1i(widthTextureLoc, widthTexture);
  GLint heightTextureLoc = glGetUniformLocation(idProgramShader, "heightTexture");
  glUniform1i(heightTextureLoc, heightTexture);

  GLint heightFactorLoc = glGetUniformLocation(idProgramShader, "heightFactor");

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(win)) {
    int width, height;
    glfwGetFramebufferSize(win, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    eye += speed * gaze;

    glm::mat4 projectionMatrix = glm::perspective(fovy, ratio, near, far);

    glm::vec3 center = eye + gaze * near;
    glm::mat4 viewMatrix = glm::lookAt(eye, center, up);

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform1f(heightFactorLoc, heightFactor);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, triangle_count*9);
    glDisableClientState(GL_VERTEX_ARRAY);

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}