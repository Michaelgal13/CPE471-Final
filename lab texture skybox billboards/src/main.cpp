/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include <algorithm>
#include <vector>  

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> cube, sphere, f18, balloon, sidewinder, AIM7;

glm::vec3 cubeVects[] = {
      glm::vec3(-1, -1, -1),
      glm::vec3(-1, -1, 1),
      glm::vec3(-1, 1, 1),
      glm::vec3(-1, 1, -1),
      glm::vec3(1, 1, -1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, -1, 1),
      glm::vec3(1, 1, -1)
};

void initGameObj();

#define AccelRate 0.5
#define MaxAccel 1.0
#define MaxSpeed 1.0
#define PI 3.1415926

enum cameraType {
   firstP = 0,
   thirdP = 1,
   testP = 2
};
cameraType camType = firstP;

double get_last_elapsed_time()
{
   static double lasttime = glfwGetTime();
   double actualtime = glfwGetTime();
   double difference = actualtime - lasttime;
   lasttime = actualtime;
   return difference;
}

//float c = 1.0;
//float accelVal = 0.5;
//float deccelVal = 0.5;
float walkSpeed = 1.5;
float runSpeed = 3;
float gamma;
glm::mat4 missView;
glm::mat4 cockpitView;
glm::mat4 planeView;

class gameObject {
   glm::mat4 update(float frametime);
   glm::mat4 getLoc(void);
};

class enemy {
   virtual void drawEnem() = 0;
};
class enemBox : public enemy {
public:
   glm::vec4 boxCol = glm::vec4(1, 0, 0, 0.5);
   glm::vec3 boxPos = glm::vec3(0, 0, 0);
   glm::vec3 boxScale = glm::vec3(1, 1, 1);
   glm::vec3 boxRot = glm::vec3(0, 0, 0);
   glm::vec3 AABBMin = glm::vec3(-1, -1, -1);
   glm::vec3 AABBMax = glm::vec3(1, 1, 1);
   glm::vec3 OOBBPoshitbox[8] = {
      glm::vec3(-1, -1, -1),
      glm::vec3(-1, -1, 1),
      glm::vec3(-1, 1, 1),
      glm::vec3(-1, 1, -1),
      glm::vec3(1, 1, -1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, -1, 1),
      glm::vec3(1, -1, -1)
   };

   enemBox(glm::vec3 boxPos) {
      this->boxPos = boxPos;
      AABBMin = glm::vec3(this->boxPos.x - 1, this->boxPos.y - 1, this->boxPos.z - 1);
      AABBMax = glm::vec3(this->boxPos.x + 1, this->boxPos.y + 1, this->boxPos.z + 1);
   }
   enemBox(glm::vec3 boxPos, glm::vec4 boxCol) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      AABBMin = glm::vec3(this->boxPos.x - 1, this->boxPos.y - 1, this->boxPos.z - 1);
      AABBMax = glm::vec3(this->boxPos.x + 1, this->boxPos.y + 1, this->boxPos.z + 1);
   }
   enemBox(glm::vec3 boxPos, glm::vec4 boxCol, glm::vec3 boxScale) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      this->boxScale = boxScale;
      AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
      AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   }
   enemBox(glm::vec3 boxPos, glm::vec4 boxCol, glm::vec3 boxScale, glm::vec3 boxRot) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      this->boxScale = boxScale;
      this->boxRot = boxRot;
      AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
      AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   }
   void drawEnem();
   void updateCol();
};
vector <enemBox> enemBoxList;

class enemBall : public enemy {
public:
   glm::vec4 boxCol = glm::vec4(1, 0, 0, 0.5);
   glm::vec3 boxPos = glm::vec3(0, 0, 0);
   glm::vec3 boxScale = glm::vec3(1, 1, 1);
   glm::vec3 boxRot = glm::vec3(0, 0, 0);
   glm::vec3 AABBMin = glm::vec3(-1, -1, -1);
   glm::vec3 AABBMax = glm::vec3(1, 1, 1);
   glm::vec3 OOBBPoshitbox[8] = {
      glm::vec3(-1, -1, -1),
      glm::vec3(-1, -1, 1),
      glm::vec3(-1, 1, 1),
      glm::vec3(-1, 1, -1),
      glm::vec3(1, 1, -1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, -1, 1),
      glm::vec3(1, -1, -1)
   };
   glm::vec3 OOBBMishitbox[8] = {
      glm::vec3(-20, -20, -20),
      glm::vec3(-20, -20, 20),
      glm::vec3(-20, 20, 20),
      glm::vec3(-20, 20, -20),
      glm::vec3(20, 20, -20),
      glm::vec3(20, 20, 20),
      glm::vec3(20, -20, 20),
      glm::vec3(20, -20, -20)
   };


   glm::vec2 expCount = glm::vec2(0, 0);
   int countExp = 0;
   int ballState = 0;
   enemBall() {
      this->boxPos = glm::vec3(0, 0, 0);
      AABBMin = glm::vec3(this->boxPos.x - 1, this->boxPos.y - 1, this->boxPos.z - 1);
      AABBMax = glm::vec3(this->boxPos.x + 1, this->boxPos.y + 1, this->boxPos.z + 1);
   }
   enemBall(glm::vec3 boxPos) {
      this->boxPos = boxPos;
      AABBMin = glm::vec3(this->boxPos.x - 1, this->boxPos.y - 1, this->boxPos.z - 1);
      AABBMax = glm::vec3(this->boxPos.x + 1, this->boxPos.y + 1, this->boxPos.z + 1);
   }
   enemBall(glm::vec3 boxPos, glm::vec4 boxCol) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      AABBMin = glm::vec3(this->boxPos.x - 1, this->boxPos.y - 1, this->boxPos.z - 1);
      AABBMax = glm::vec3(this->boxPos.x + 1, this->boxPos.y + 1, this->boxPos.z + 1);
   }
   enemBall(glm::vec3 boxPos, glm::vec4 boxCol, glm::vec3 boxScale) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      this->boxScale = boxScale;
      AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
      AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   }
   enemBall(glm::vec3 boxPos, glm::vec4 boxCol, glm::vec3 boxScale, glm::vec3 boxRot) {
      this->boxCol = boxCol;
      this->boxPos = boxPos;
      this->boxScale = boxScale;
      this->boxRot = boxRot;
      AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
      AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   }
   void drawEnem();
   void drawEnemE(glm::mat4 iVR);
   void drawEnemS();
   void updateCol();
};
vector <enemBall> enemBallList;

class missile {
public:
   glm::vec3 pos = glm::vec3(0, 0, 0);
   glm::mat4 rotMat = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 1, 0));
   glm::vec3 missRot = glm::vec3(0, 0, 0);
   glm::vec3 norm1 = glm::vec3(1, 0, 0);
   glm::vec3 norm2 = glm::vec3(0, 1, 0);
   glm::vec3 norm3 = glm::vec3(0, 0, 1);
   float speed = 0;
   int missState = 0;
   int locked = 0;
   enemBall ball;
   float fuel = 100;
   glm::vec2 expCount = glm::vec2(0, 0);
   int countExp = 0;

   glm::vec3 AABBMin = glm::vec3(-2, -2, -2);
   glm::vec3 AABBMax = glm::vec3(2, 2, 2);

   glm::vec3 OOBBPoshitboxdef[8] = {
      glm::vec3(-15, -7.5, -40),
      glm::vec3(-15, -7.5, 40),
      glm::vec3(-15, 7.5, 40),
      glm::vec3(-15, 7.5, -40),
      glm::vec3(15, 7.5, -40),
      glm::vec3(15, 7.5, 40),
      glm::vec3(15, -7.5, 40),
      glm::vec3(15, -7.5, -40)
   };

   glm::vec3 OOBBPoshitbox[8] = {
      glm::vec3(-15, -7.5, -40),
      glm::vec3(-15, -7.5, 40),
      glm::vec3(-15, 7.5, 40),
      glm::vec3(-15, 7.5, -40),
      glm::vec3(15, 7.5, -40),
      glm::vec3(15, 7.5, 40),
      glm::vec3(15, -7.5, 40),
      glm::vec3(15, -7.5, -40)
   };
   missile() {
      pos = glm::vec3(0, 0, 0);
   }
   void update(float frametime);
   void drawMiss();
   void drawMissE(glm::mat4 iVR);
   void drawMissS();
   bool OOBBColCheck(enemBall enemy);

};
vector <missile> missileList;


class player : public gameObject {
public:
   //plane metric
   float maxSpeed = 1.0f;
   glm::vec3 OOBBPoshitboxdef[8] = {
      glm::vec3(-15, -5, -40),
      glm::vec3(-15, -5, 40),
      glm::vec3(-15, 5, 40),
      glm::vec3(-15, 5, -40),
      glm::vec3(15, 5, -40),
      glm::vec3(15, 5, 40),
      glm::vec3(15, -5, 40),
      glm::vec3(15, -5, -40)
   };


   glm::vec3 OOBBPoshitbox[8] = {
      glm::vec3(-15, -7.5, -40),
      glm::vec3(-15, -7.5, 40),
      glm::vec3(-15, 7.5, 40),
      glm::vec3(-15, 7.5, -40),
      glm::vec3(15, 7.5, -40),
      glm::vec3(15, 7.5, 40),
      glm::vec3(15, -7.5, 40),
      glm::vec3(15, -7.5, -40)
   };
   glm::vec3 AABBMax = glm::vec3(40, 40, 40);
   glm::vec3 AABBMin = glm::vec3(-40, -40, -40);
   glm::vec3 norm1 = glm::vec3(1, 0, 0);
   glm::vec3 norm2 = glm::vec3(0, 1, 0);
   glm::vec3 norm3 = glm::vec3(0, 0, 1);
   int planeState = 0;
   glm::vec2 expCount = glm::vec2(0, 0);
   int countExp = 0;
   //buttons
   int buttonX;
   int buttonA;
   int buttonB;
   int buttonY;
   int buttonLB;
   int buttonRB;
   int buttonLT;
   int buttonRT;
   int buttonBack;
   int buttonStart;
   int buttonGuide;
   int buttonL3;
   int buttonR3;
   int buttonUp;
   int buttonRight;
   int buttonDown;
   int buttonLeft;
   int buttonPs;
   float rollVal;
   float pitchVal;
   float camHori;
   float camVert;
   float zDec;
   float zAcc;
   //plane state
   float zAcceleration = 0;
   float zSpe = -0.5;
   float ySpe = 0;
   glm::vec3 pos = glm::vec3(0, 0, 0);
   glm::vec3 posPure = glm::vec3(0, 0, 0);
   glm::vec3 rot = glm::vec3(0, 0, 0);
   glm::vec3 dir = glm::vec3(0, 0, 0);
   glm::mat4 rotMat = glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 1, 0));
   //cam state
   int camChange = 0;
   //hitbox state
   int hitChange = 0;
   int hitState = 0;
   //missile state
   int missileLast = 0;
   float missileRTime = 1;
   float missileLTime = 1;
   int missileState = 0;
   //player();
   glm::mat4 planeLoc;
   void update(float frametime);
   glm::mat4 getLoc(void);
   void checkCollision(void);
   bool OOBBColCheck(enemBox enemy);
   bool OOBBColCheck(enemBall enemy);
};
player Cipher;



bool enemBoxDist(enemBox i, enemBox j) { return (pow(Cipher.pos.x - i.boxPos.x, 2) + pow(Cipher.pos.y - i.boxPos.y, 2) + pow(Cipher.pos.z - i.boxPos.z, 2) > pow(Cipher.pos.x - j.boxPos.x, 2) + pow(Cipher.pos.y - j.boxPos.y, 2) + pow(Cipher.pos.z - j.boxPos.z, 2)); }


class camera
{
public:
   glm::vec3 pos, rot, prevpos, setpos;
   glm::vec4 dir2;
   float yAcc, xAcc;
   float ySpe, xSpe, zSpe;
   int w, a, s, d, shift, ctrl, space;
   bool jump = false;

   glm::vec2 mousePrev = glm::vec2(0, 0), mouseNew = glm::vec2(0, 0);
   camera()
   {
      w = a = s = d = shift = ctrl = 0;
      pos = rot = glm::vec3(0, 0, 0);
   }

   glm::mat4 process(double ftime)
   {
      float yangle = 0;
      glm::mat4 T, R, R2;
      float ftime2 = 500 * ftime;
      float contCamY = Cipher.camVert;
      float contCamX = Cipher.camHori;
      glm::mat4 planeLoc;
      glm::mat4 T2;
      switch (camType) {
      case testP:
         pos = setpos;
         if (ftime2 > 1) {
            ftime2 = 1;
         }
         if (w == 1) {
            if (shift == 1) {
               zSpe = ((1 - ftime2) * zSpe + runSpeed * ftime2);
            }
            else {
               zSpe = ((1 - ftime2) * zSpe + walkSpeed * ftime2);
            }
         }
         else if (s == 1) {
            if (shift == 1) {
               zSpe = ((1 - ftime2) * zSpe - runSpeed * ftime2) * 0.8;
            }
            else {
               zSpe = ((1 - ftime2) * zSpe - walkSpeed * ftime2) * 0.8;
            }
         }
         else {
            zSpe *= 0.8;
         }
         if (a == 1) {
            xSpe = 0.5;
         }
         else if (d == 1) {
            xSpe = -0.5;
         }
         else {
            xSpe = 0;
         }

         yangle = 0;
         rot.y += mouseNew.x;
         rot.x += mouseNew.y;
         if (rot.x < -1.25) {
            rot.x = -1.25;
         }
         if (rot.x > 1) {
            rot.x = 1;
         }
         contCamX = (PI - 0.1) * contCamX;
         contCamY = (PI - 0.5) * contCamY;
         contCamY = glm::clamp(contCamY, (float)(-PI + 0.5), (float)(PI / 2));
         R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
         R2 = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
         glm::vec4 dir = glm::vec4(xSpe, 0, zSpe, 1);
         glm::mat4 R3 = glm::rotate(glm::mat4(1), contCamX, glm::vec3(0, 1, 0));
         glm::mat4 R4 = glm::rotate(glm::mat4(1), contCamY, glm::vec3(1, 0, 0));
         dir = dir * R;
         pos += glm::vec3(dir.x, dir.y, dir.z);
         if (ctrl) {
            pos.y = 1.5;
         }
         else {
            pos.y = 0;
         }
         setpos = pos;
         T = glm::translate(glm::mat4(1), pos);
         return  R4 * R3 * R2 * R * T;
      case(thirdP):
         pos = Cipher.pos;
         planeLoc = Cipher.planeLoc;
         contCamX = (PI - 0.1) * contCamX;
         contCamY = (PI - 0.5) * contCamY;
         contCamY = glm::clamp(contCamY, (float)(-PI + 0.5), (float)(PI / 2));
         R = glm::rotate(glm::mat4(1), contCamY, glm::vec3(1, 0, 0));
         R2 = glm::rotate(glm::mat4(1), contCamX, glm::vec3(0, 1, 0));
         //glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), float(-PI / 2), glm::vec3(1.0f, 0.0f, 0.0f));
         //glm::mat4 Rp = glm::rotate(glm::mat4(1), Cipher.rot.y, glm::vec3(0, 1, 0));
         //glm::mat4 R2p = glm::rotate(glm::mat4(1), Cipher.rot.x, glm::vec3(1, 0, 0));
         //glm::mat4 R3p = glm::rotate(glm::mat4(1), Cipher.rot.z, glm::vec3(0, 0, 1));
         T2 = glm::translate(glm::mat4(1), pos);
         T = glm::translate(glm::mat4(1), glm::vec3(0, 15, 100));
         //glm::vec4 posPrime = glm::vec4(pos, 1);
         //posPrime = posPrime * glm::inverse(T2 * Rp * R2p * R3p * T)
         //pos = glm::vec3(posPrime.x, posPrime.y, posPrime.z);
         //return R * R2 * glm::inverse(Rp * R2p * R3p * T * T2);
         return glm::inverse(T2 * Cipher.rotMat * R * R2 * T);

      case(firstP):
      default:
         pos = Cipher.pos;
         planeLoc = Cipher.planeLoc;
         contCamX = (PI - 0.1) * contCamX;
         contCamY = (PI - 0.5) * contCamY;
         contCamY = glm::clamp(contCamY, (float)(-PI + 0.5), (float)(PI / 2));
         R = glm::rotate(glm::mat4(1), contCamY, glm::vec3(1, 0, 0));
         R2 = glm::rotate(glm::mat4(1), contCamX, glm::vec3(0, 1, 0));
         //glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), float(-PI / 2), glm::vec3(1.0f, 0.0f, 0.0f));
         //glm::mat4 Rp = glm::rotate(glm::mat4(1), Cipher.rot.y, glm::vec3(0, 1, 0));
         //glm::mat4 R2p = glm::rotate(glm::mat4(1), Cipher.rot.x, glm::vec3(1, 0, 0));
         //glm::mat4 R3p = glm::rotate(glm::mat4(1), Cipher.rot.z, glm::vec3(0, 0, 1));
         T2 = glm::translate(glm::mat4(1), pos);
         T = glm::translate(glm::mat4(1), glm::vec3(0, 2, -20));
         //glm::vec4 posPrime = glm::vec4(pos, 1);
         //posPrime = posPrime * glm::inverse(T2 * Rp * R2p * R3p * T)
         //pos = glm::vec3(posPrime.x, posPrime.y, posPrime.z);
         //return R * R2 * glm::inverse(Rp * R2p * R3p * T * T2);
         return R * R2 * glm::inverse(T2 * Cipher.rotMat * T);
      }
   }
};

camera mycam;

class Application : public EventCallbacks
{

public:

   WindowManager* windowManager = nullptr;

   // Our shader program
   std::shared_ptr<Program> prog, psky, shad, depth, plane, boundBox, water, explode;

   // Contains vertex information for OpenGL
   GLuint VertexArrayID;

   // Data necessary to give our box to OpenGL
   GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

   // Contains vertex information for OpenGL
   GLuint VertexArrayID2;

   // Data necessary to give our box to OpenGL
   GLuint VertexBufferID2, VertexNormDBox2, VertexTexBox2, IndexBufferIDBox2;

   GLuint FBOtex, depthMapFBO, depth_rb;

   //texture data
   GLuint Texture, TextureN, TextureExp;
   GLuint Texture2, TextureBall, TextureSide, TextureWat1, TextureWat2;
   void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
   {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      {
         glfwSetWindowShouldClose(window, GL_TRUE);
      }

      if (key == GLFW_KEY_W && action == GLFW_PRESS)
      {
         mycam.w = 1;
      }
      if (key == GLFW_KEY_W && action == GLFW_RELEASE)
      {
         mycam.w = 0;
      }
      if (key == GLFW_KEY_S && action == GLFW_PRESS)
      {
         mycam.s = 1;
      }
      if (key == GLFW_KEY_S && action == GLFW_RELEASE)
      {
         mycam.s = 0;
      }
      if (key == GLFW_KEY_A && action == GLFW_PRESS)
      {
         mycam.a = 1;
      }
      if (key == GLFW_KEY_A && action == GLFW_RELEASE)
      {
         mycam.a = 0;
      }
      if (key == GLFW_KEY_D && action == GLFW_PRESS)
      {
         mycam.d = 1;
      }
      if (key == GLFW_KEY_D && action == GLFW_RELEASE)
      {
         mycam.d = 0;
      }
      if ((key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)) {
         mycam.shift = 1;
      }
      if ((key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)) {
         mycam.shift = 0;
      }
      if ((key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)) {
         mycam.ctrl = 1;
      }
      if ((key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)) {
         mycam.ctrl = 0;
      }
      if ((key == GLFW_KEY_SPACE && action == GLFW_PRESS)) {
         mycam.space = 1;
      }
      if ((key == GLFW_KEY_SPACE && action == GLFW_RELEASE)) {
         mycam.space = 0;
      }
   }

   // callback for the mouse when clicked move the triangle when helper functions
   // written
   void mouseCallback(GLFWwindow* window, int button, int action, int mods)
   {
      double posX, posY;
      float newPt[2];
      if (action == GLFW_PRESS)
      {
         /*glfwGetCursorPos(window, &posX, &posY);
         std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

         //change this to be the points converted to WORLD
         //THIS IS BROKEN< YOU GET TO FIX IT - yay!
         newPt[0] = 0;
         newPt[1] = 0;

         std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
         glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
         //update the vertex array with the updated points
         glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
         glBindBuffer(GL_ARRAY_BUFFER, 0);*/
      }
   }

   void joystickUpdate(void)
   {
      if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
         int axesCount;
         const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
         Cipher.rollVal = axes[0];
         Cipher.pitchVal = axes[1];
         Cipher.camHori = axes[2];
         Cipher.zDec = axes[3] + 1;
         Cipher.zAcc = axes[4] + 1;
         Cipher.camVert = axes[5];
         if (abs(Cipher.rollVal) < 0.1) {
            Cipher.rollVal = 0;
         }
         if (abs(Cipher.pitchVal) < 0.1) {
            Cipher.pitchVal = 0;
         }
         if (abs(Cipher.camHori) < 0.1) {
            Cipher.camHori = 0;
         }
         if (abs(Cipher.camVert) < 0.1) {
            Cipher.camVert = 0;
         }

         int buttonCount;
         const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
         if (buttonCount >= 15) {
            if (GLFW_PRESS == buttons[0]) {
               Cipher.buttonX = 1;
            }
            else if (GLFW_RELEASE == buttons[0]) {
               Cipher.buttonX = 0;
            }
            if (GLFW_PRESS == buttons[1]) {
               Cipher.buttonA = 1;
            }
            else if (GLFW_RELEASE == buttons[1]) {
               Cipher.buttonA = 0;
            }
            if (GLFW_PRESS == buttons[2]) {
               Cipher.buttonB = 1;
            }
            else if (GLFW_RELEASE == buttons[2]) {
               Cipher.buttonB = 0;
            }
            if (GLFW_PRESS == buttons[3]) {
               Cipher.buttonY = 1;
            }
            else if (GLFW_RELEASE == buttons[3]) {
               Cipher.buttonY = 0;
            }
            if (GLFW_PRESS == buttons[4]) {
               Cipher.buttonLB = 1;
            }
            else if (GLFW_RELEASE == buttons[4]) {
               Cipher.buttonLB = 0;
            }
            if (GLFW_PRESS == buttons[5]) {
               Cipher.buttonRB = 1;
            }
            else if (GLFW_RELEASE == buttons[5]) {
               Cipher.buttonRB = 0;
            }
            if (GLFW_PRESS == buttons[6]) {
               Cipher.buttonLT = 1;

            }
            else if (GLFW_RELEASE == buttons[6]) {
               Cipher.buttonLT = 0;
            }
            if (GLFW_PRESS == buttons[7]) {
               Cipher.buttonRT = 1;
            }
            else if (GLFW_RELEASE == buttons[7]) {
               Cipher.buttonRT = 0;
            }
            if (GLFW_PRESS == buttons[8]) {
               Cipher.buttonBack = 1;
            }
            else if (GLFW_RELEASE == buttons[8]) {
               Cipher.buttonBack = 0;
            }
            if (GLFW_PRESS == buttons[9]) {
               Cipher.buttonStart = 1;
            }
            else if (GLFW_RELEASE == buttons[9]) {
               Cipher.buttonStart = 0;
            }
            if (GLFW_PRESS == buttons[10]) {
               Cipher.buttonL3 = 1;
            }
            else if (GLFW_RELEASE == buttons[10]) {
               Cipher.buttonL3 = 0;
            }
            if (GLFW_PRESS == buttons[11]) {
               Cipher.buttonR3 = 1;
            }
            else if (GLFW_RELEASE == buttons[11]) {
               Cipher.buttonR3 = 0;
            }
            if (GLFW_PRESS == buttons[12]) {
               Cipher.buttonPs = 1;
            }
            else if (GLFW_RELEASE == buttons[12]) {
               Cipher.buttonPs = 0;
            }
            if (GLFW_PRESS == buttons[13]) {
               Cipher.buttonGuide = 1;
            }
            else if (GLFW_RELEASE == buttons[13]) {
               Cipher.buttonGuide = 0;
            }
            if (GLFW_PRESS == buttons[14]) {
               Cipher.buttonUp = 1;
            }
            else if (GLFW_RELEASE == buttons[14]) {
               Cipher.buttonUp = 0;
            }
            if (GLFW_PRESS == buttons[15]) {
               Cipher.buttonRight = 1;
            }
            else if (GLFW_RELEASE == buttons[15]) {
               Cipher.buttonRight = 0;
            }
            if (GLFW_PRESS == buttons[16]) {
               Cipher.buttonDown = 1;
            }
            else if (GLFW_RELEASE == buttons[16]) {
               Cipher.buttonDown = 0;
            }
            if (GLFW_PRESS == buttons[17]) {
               Cipher.buttonLeft = 1;
            }
            else if (GLFW_RELEASE == buttons[17]) {
               Cipher.buttonLeft = 0;
            }
         }
      }
   }

   //if the window is resized, capture the new size and reset the viewport
   void resizeCallback(GLFWwindow* window, int in_width, int in_height)
   {
      //get the window size - may be different then pixels for retina
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0, width, height);
   }
   unsigned int depthMap;
   /*Note that any gl calls must always happen after a GL state is initialized */
   void initGeom()
   {
      //generate the VAO
      glGenVertexArrays(1, &VertexArrayID);
      glBindVertexArray(VertexArrayID);

      //generate vertex buffer to hand off to OGL
      glGenBuffers(1, &VertexBufferID);
      //set the current state to focus on our vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

      GLfloat rect_vertices[] = {
         // front
         -1.0, -1.0,  1.0,//LD
         1.0, -1.0,  1.0,//RD
         1.0,  1.0,  1.0,//RU
         -1.0,  1.0,  1.0,//LU
      };
      //make it a bit smaller
      for (int i = 0; i < 12; i++)
         rect_vertices[i] *= 0.5;
      //actually memcopy the data - only do this once
      glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_DYNAMIC_DRAW);

      //we need to set up the vertex array
      glEnableVertexAttribArray(0);
      //key function to get up how many elements to pull out at a time (3)
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //color
      GLfloat rect_norm[] = {
         // front colors
         0.0, 0.0, 1.0,
         0.0, 0.0, 1.0,
         0.0, 0.0, 1.0,
         0.0, 0.0, 1.0,

      };
      glGenBuffers(1, &VertexNormDBox);
      //set the current state to focus on our vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
      glBufferData(GL_ARRAY_BUFFER, sizeof(rect_norm), rect_norm, GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //color
      glm::vec2 rect_tex[] = {
         // front colors
         glm::vec2(0.0, 1.0),
         glm::vec2(1.0, 1.0),
         glm::vec2(1.0, 0.0),
         glm::vec2(0.0, 0.0),

      };
      glGenBuffers(1, &VertexTexBox);
      //set the current state to focus on our vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
      glBufferData(GL_ARRAY_BUFFER, sizeof(rect_tex), rect_tex, GL_STATIC_DRAW);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

      glGenBuffers(1, &IndexBufferIDBox);
      //set the current state to focus on our vertex buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      GLushort rect_elements[] = {

         // front
         0, 1, 2,
         2, 3, 0,
      };
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_elements), rect_elements, GL_STATIC_DRAW);



      glBindVertexArray(0);


      string resourceDirectory = "../resources";
      // Initialize mesh.
      cube = make_shared<Shape>();
      //shape->loadMesh(resourceDirectory + "/t800.obj");
      cube->loadMesh(resourceDirectory + "/cube2.obj");
      cube->resize();
      cube->init();

      balloon = make_shared<Shape>();
      //shape->loadMesh(resourceDirectory + "/t800.obj");
      balloon->loadMesh(resourceDirectory + "/balloon.obj");
      balloon->resize();
      balloon->init();



      sphere = make_shared<Shape>();
      //shape->loadMesh(resourceDirectory + "/t800.obj");
      sphere->loadMesh(resourceDirectory + "/sphere.obj");
      sphere->resize();
      sphere->init();

      f18 = make_shared<Shape>();
      string mtldir = resourceDirectory + "/FA-18E_SuperHornet/";
      f18->loadMesh(resourceDirectory + "/FA-18E_SuperHornet/FA-18E_SuperHornet.obj", &mtldir, stbi_load);
      //f18->loadMesh(resourceDirectory + "/sphere.obj");
      f18->resize();
      f18->init();

      AIM7 = make_shared<Shape>();
      mtldir = resourceDirectory + "/AIM7F-Wavefront/";
      AIM7->loadMesh(resourceDirectory + "/AIM7F-Wavefront/AIM7F2.obj", &mtldir, stbi_load);
      //f18->loadMesh(resourceDirectory + "/sphere.obj");
      AIM7->resize();
      AIM7->init();


      sidewinder = make_shared<Shape>();
      //shape->loadMesh(resourceDirectory + "/t800.obj");
      sidewinder->loadMesh(resourceDirectory + "/AIM-9 SIDEWINDER.obj");
      sidewinder->resize();
      sidewinder->init();

      int width, height, channels;
      char filepath[1000];

      //texture 1
      string str = resourceDirectory + "/box.png";
      strcpy(filepath, str.c_str());
      unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &Texture);
      //glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      //texture Night
      str = resourceDirectory + "/night.jpg";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureN);
      //glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureN);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);


      //texture explode
      str = resourceDirectory + "/P6LFNRLPISUT.png";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureExp);
      //glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureExp);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);


      //texture 2
      str = resourceDirectory + "/sky.jpg";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &Texture2);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, Texture2);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      //texture 2
      str = resourceDirectory + "/target.png";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureBall);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureBall);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      //texture 2
      str = resourceDirectory + "/sidewinder.png";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureSide);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureSide);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      //texture 2
      str = resourceDirectory + "/IKT4l.jpg";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureWat1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureWat1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      //texture 2
      str = resourceDirectory + "/water_new_height.png";
      strcpy(filepath, str.c_str());
      data = stbi_load(filepath, &width, &height, &channels, 4);
      glGenTextures(1, &TextureWat2);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureWat2);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);




      //[TWOTEXTURES]
      //set the 2 textures to the correct samplers in the fragment shader:
      GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
      GLuint Tex2Location = glGetUniformLocation(prog->pid, "depthMap");
      // Then bind the uniform samplers to texture units:
      glUseProgram(prog->pid);
      glUniform1i(Tex1Location, 0);
      glUniform1i(Tex2Location, 1);

      Tex1Location = glGetUniformLocation(psky->pid, "tex");//tex, tex2... sampler in the fragment shader
      Tex2Location = glGetUniformLocation(psky->pid, "tex2");
      // Then bind the uniform samplers to texture units:
      glUseProgram(psky->pid);
      glUniform1i(Tex1Location, 0);
      glUniform1i(Tex2Location, 1);

      Tex1Location = glGetUniformLocation(plane->pid, "tex");//tex, tex2... sampler in the fragment shader
      Tex2Location = glGetUniformLocation(plane->pid, "depthMap");
      // Then bind the uniform samplers to texture units:
      glUseProgram(plane->pid);
      glUniform1i(Tex1Location, 0);
      glUniform1i(Tex2Location, 1);

      Tex1Location = glGetUniformLocation(water->pid, "tex");//tex, tex2... sampler in the fragment shader
      Tex2Location = glGetUniformLocation(water->pid, "tex2");
      GLuint Tex3Location = glGetUniformLocation(water->pid, "depthMap");
      // Then bind the uniform samplers to texture units:
      glUseProgram(water->pid);
      glUniform1i(Tex1Location, 0);
      glUniform1i(Tex2Location, 1);
      glUniform1i(Tex3Location, 2);

      /*Tex1Location = glGetUniformLocation(explode->pid, "tex");//tex, tex2... sampler in the fragment shader
      // Then bind the uniform samplers to texture units:
      glUseProgram(explode->pid);
      glUniform1i(Tex1Location, 0);*/


      //FrameBuffer Objext
      glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
      glGenFramebuffers(1, &depthMapFBO);
      glGenTextures(1, &depthMap);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      // attach depth texture as FBO's depth buffer
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      GLenum status;
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      switch (status) {
      case GL_FRAMEBUFFER_COMPLETE:
         cout << "framebuffer status: good" << endl;
         break;
      default:
         cout << "FRAMEBUFFER ERROR!!!!!!!!!" << endl;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      Tex1Location = glGetUniformLocation(depth->pid, "tex");//tex, tex2... sampler in the fragment shader
      // Then bind the uniform samplers to texture units:
      glUseProgram(depth->pid);
      glUniform1i(Tex1Location, 0);
   }

   //General OGL initialization - set OGL state here
   void init(const std::string& resourceDirectory)
   {
      GLSL::checkVersion();

      // Set background color.
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      // Enable z-buffer test.
      glEnable(GL_DEPTH_TEST);
      //glDisable(GL_DEPTH_TEST);
      // Initialize the GLSL program.
      prog = std::make_shared<Program>();
      prog->setVerbose(true);
      prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
      if (!prog->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      prog->addUniform("ligP");
      prog->addUniform("ligV");
      prog->addUniform("P");
      prog->addUniform("V");
      prog->addUniform("M");
      prog->addUniform("campos");
      prog->addUniform("ligpos");
      prog->addAttribute("vertPos");
      prog->addAttribute("vertNor");
      prog->addAttribute("vertTex");

      plane = std::make_shared<Program>();
      plane->setVerbose(true);
      plane->setShaderNames(resourceDirectory + "/plane_vertex.glsl", resourceDirectory + "/plane_fragment.glsl");
      if (!plane->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      plane->addUniform("ligP");
      plane->addUniform("ligV");
      plane->addUniform("P");
      plane->addUniform("V");
      plane->addUniform("M");
      plane->addUniform("campos");
      plane->addUniform("ligpos");
      plane->addAttribute("vertPos");
      plane->addAttribute("vertNor");
      plane->addAttribute("vertTex");


      psky = std::make_shared<Program>();
      psky->setVerbose(true);
      psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
      if (!psky->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      psky->addUniform("dn");
      psky->addUniform("P");
      psky->addUniform("V");
      psky->addUniform("M");
      psky->addUniform("campos");
      psky->addAttribute("vertPos");
      psky->addAttribute("vertNor");
      psky->addAttribute("vertTex");

      boundBox = std::make_shared<Program>();
      boundBox->setVerbose(true);
      boundBox->setShaderNames(resourceDirectory + "/boundvertex.glsl", resourceDirectory + "/boundfrag.glsl");
      if (!boundBox->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      boundBox->addUniform("incol");
      boundBox->addUniform("P");
      boundBox->addUniform("V");
      boundBox->addUniform("M");
      boundBox->addUniform("campos");
      boundBox->addAttribute("vertPos");
      boundBox->addAttribute("vertNor");
      boundBox->addAttribute("vertTex");

      shad = std::make_shared<Program>();
      shad->setVerbose(true);
      shad->setShaderNames(resourceDirectory + "/shadow_vertex.glsl", resourceDirectory + "/shadow_fragment.glsl");
      if (!shad->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      shad->addUniform("P");
      shad->addUniform("V");
      shad->addUniform("M");
      shad->addUniform("campos");
      shad->addAttribute("vertPos");
      shad->addAttribute("vertNor");
      shad->addAttribute("vertTex");


      depth = std::make_shared<Program>();
      depth->setVerbose(true);
      depth->setShaderNames(resourceDirectory + "/depth_vertex.glsl", resourceDirectory + "/depth_fragment.glsl");
      if (!depth->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      depth->addUniform("P");
      depth->addUniform("V");
      depth->addUniform("M");
      //depth->addUniform("lightSpaceMatrix");
      depth->addUniform("campos");
      depth->addAttribute("vertPos");

      water = std::make_shared<Program>();
      water->setVerbose(true);
      water->setShaderNames(resourceDirectory + "/watervertex.glsl", resourceDirectory + "/waterfrag.glsl");
      if (!water->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      water->addUniform("ligP");
      water->addUniform("ligV");
      water->addUniform("P");
      water->addUniform("V");
      water->addUniform("M");
      water->addUniform("campos");
      water->addUniform("texoff");
      water->addUniform("ligpos");
      water->addUniform("vertoff");
      water->addAttribute("vertPos");
      water->addAttribute("vertNor");
      water->addAttribute("vertTex");


      explode = std::make_shared<Program>();
      explode->setVerbose(true);
      explode->setShaderNames(resourceDirectory + "/explode_vertex.glsl", resourceDirectory + "/explode_fragment.glsl");
      if (!explode->init())
      {
         std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
         exit(1);
      }
      explode->addUniform("P");
      explode->addUniform("V");
      explode->addUniform("M");
      explode->addUniform("texoff");
      explode->addUniform("campos");
      explode->addAttribute("vertPos");


   }


   /****DRAW
   This is the most important function in your program - this is where you
   will actually issue the commands to draw any geometry you have set up to
   draw
   ********/
   glm::vec3 ligpos = glm::vec3(0, 0, 1000);
   glm::mat4 lightView, lightPer;


   void render()
   {

      static float texoff = 0;
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      double frametime = get_last_elapsed_time();
      GLFWwindow* window = windowManager->getHandle();
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      // Get current frame buffer size.
      int width, height;
      glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
      float aspect = width / (float)height;
      glViewport(0, 0, width, height);
      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);
      glfwSetCursorPos(window, 0.0, 0.0);
      mycam.mouseNew = glm::vec2(mouseX / width, mouseY / height);
      // Clear framebuffer.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Create the matrix stacks - please leave these alone for now

      glm::mat4 V, M, P; //View, Model and Perspective matrix
      V = mycam.process(frametime);
      M = glm::mat4(1);
      P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

      float sangle = 3.1415926 / 2.;
      glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::vec3 camp = -mycam.pos;
      if (camType != testP) {
         camp = Cipher.pos;
      }
      glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
      glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 200.0f));
      M = TransSky * RotateXSky * SSky;
      glm::vec3 offset = Cipher.pos;
      offset.x = (int)offset.x;
      offset.z = (int)offset.z;
      offset.y = 0;

      texoff += frametime;

      psky->bind();
      glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture2);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureN);
      static float ttime = 0;
      ttime += frametime;
      float dn = sin(ttime) * 0.5 + 0.5;
      glUniform1f(psky->getUniform("dn"), dn);
      glDisable(GL_DEPTH_TEST);
      sphere->draw(psky);
      glEnable(GL_DEPTH_TEST);
      psky->unbind();


      glm::mat4 RotateX;
      glm::mat4 TransZ;
      glm::mat4 S;




      // Draw the box using GLSL.
      prog->bind();


      //send the matrices to the shaders
      glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glUniformMatrix4fv(prog->getUniform("ligV"), 1, GL_FALSE, &lightView[0][0]);
      glUniformMatrix4fv(prog->getUniform("ligP"), 1, GL_FALSE, &lightPer[0][0]);
      glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);
      glUniform3fv(prog->getUniform("ligpos"), 1, &ligpos[0]);

      glBindVertexArray(VertexArrayID);
      //actually draw from vertex 0, 3 vertices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, depthMap);

      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -20.0f, -17));
      S = glm::scale(glm::mat4(1.0f), glm::vec3(100.f, 100.f, 0.f));
      float angle = 3.1415926 / 2.0f;
      RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

      M = TransZ * RotateX * S;
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
      glBindVertexArray(0);
      S = glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 10.f));
      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.7f, -17));
      M = TransZ * RotateX * S;
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //shape->draw(prog);
      prog->unbind();
      plane->bind();
      glUniformMatrix4fv(plane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(plane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(plane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glUniformMatrix4fv(plane->getUniform("ligV"), 1, GL_FALSE, &lightView[0][0]);
      glUniformMatrix4fv(plane->getUniform("ligP"), 1, GL_FALSE, &lightPer[0][0]);
      glUniform3fv(plane->getUniform("campos"), 1, &mycam.pos[0]);
      glUniform3fv(plane->getUniform("ligpos"), 1, &ligpos[0]);
      M = Cipher.getLoc();
      glUniformMatrix4fv(plane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      if (Cipher.planeState == 0) {
         f18->draw(plane);
      }
      S = glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 10.f));
      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, -50));
      M = TransZ * S;
      glUniformMatrix4fv(plane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureSide);
      for (int i = 0; i < missileList.size(); i++) {
         missileList[i].drawMiss();
      }
      //sidewinder->draw(plane);



      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureBall);

      for (int i = 0; i < enemBallList.size(); i++) {
         enemBallList[i].drawEnem();
      }

      plane->unbind();
      water->bind();
      glBindVertexArray(VertexArrayID);
      //actually draw from vertex 0, 3 vertices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureWat1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, TextureWat2);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, depthMap);

      glUniformMatrix4fv(water->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(water->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(water->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glUniformMatrix4fv(water->getUniform("ligV"), 1, GL_FALSE, &lightView[0][0]);
      glUniformMatrix4fv(water->getUniform("ligP"), 1, GL_FALSE, &lightPer[0][0]);
      glUniform3fv(water->getUniform("campos"), 1, &mycam.pos[0]);
      glUniform3fv(water->getUniform("ligpos"), 1, &ligpos[0]);
      glUniform1f(water->getUniform("texoff"), texoff);
      glUniform3fv(water->getUniform("vertoff"), 1, &offset[0]);
      S = glm::scale(glm::mat4(1.0f), glm::vec3(1000.f, 1000.f, 0.f));
      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -500.0f, 0));
      M = TransZ * RotateX * S;
      glUniformMatrix4fv(water->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
      glBindVertexArray(0);
      water->unbind();

      explode->bind();
      //send the matrices to the shaders
      glUniformMatrix4fv(explode->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(explode->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(explode->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //glUniform3fv(depth->getUniform("campos"), 1, &mycam.pos[0]);
      //glUniformMatrix4fv(depth->getUniform("lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix);

      glBindVertexArray(VertexArrayID);
      //actually draw from vertex 0, 3 vertices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TextureExp);
      mat4 iVR = V;
      iVR[3][0] = 0;
      iVR[3][1] = 0;
      iVR[3][2] = 0;

      iVR = inverse(iVR);
      if (Cipher.planeState == 1) {
         TransZ = glm::translate(glm::mat4(1.0f), Cipher.pos);
         S = glm::scale(glm::mat4(1.0f), glm::vec3(40.f, 40.f, 0.f));
         //float angle = 3.1415926 / 2.0f;
         //RotateX = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(1.0f, 0.0f, 0.0f));

         M = TransZ * iVR * S;
         glUniform2fv(explode->getUniform("texoff"), 1, &Cipher.expCount[0]);
         glUniformMatrix4fv(explode->getUniform("M"), 1, GL_FALSE, &M[0][0]);
         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
         //glBindVertexArray(0);
      }
      for (int i = 0; i < enemBallList.size(); i++) {
         enemBallList[i].drawEnemE(iVR);
      }
      for (int i = 0; i < missileList.size(); i++) {
         missileList[i].drawMissE(iVR);
      }
      glBindVertexArray(0);
      explode->unbind();

      //LAST FOR TRANSPARENCY
      std::sort(enemBoxList.begin(), enemBoxList.end(), enemBoxDist);
      boundBox->bind();
      glUniformMatrix4fv(boundBox->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(boundBox->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      //for_each(enemBoxList.begin(), enemBoxList.end(), enemBox.drawBBox);
      for (int i = 0; i < enemBoxList.size(); i++) {
         enemBoxList[i].drawEnem();
      }
      /*if (Cipher.hitState == 0) {
         S = glm::scale(glm::mat4(1.0f), glm::vec3(15.f, 7.5f, 40.f));
         TransZ = glm::translate(glm::mat4(1.0f), Cipher.pos);
         M = TransZ * Cipher.rotMat * S;
      }
      else {
         S = glm::scale(glm::mat4(1.0f), glm::vec3(40.f, 40.f, 40.f));
         TransZ = glm::translate(glm::mat4(1.0f), Cipher.pos);
         M = TransZ * S;
      }
      glUniformMatrix4fv(boundBox->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glUniform4fv(boundBox->getUniform("incol"), 1, &glm::vec4(0, 0, 1, 0.25)[0]);
      cube->draw(boundBox);
      S = glm::scale(glm::mat4(1.0f), glm::vec3(20.f, 20.f, 20.f));
      for (int i = 0; i < enemBallList.size(); i++) {
         TransZ = glm::translate(glm::mat4(1.0f), enemBallList[i].boxPos);
         M = TransZ * S;
         glUniformMatrix4fv(boundBox->getUniform("M"), 1, GL_FALSE, &M[0][0]);
         glUniform4fv(boundBox->getUniform("incol"), 1, &glm::vec4(0, 1, 1, 0.25)[0]);
         cube->draw(boundBox);
      }*/
      boundBox->unbind();
      


      if (Cipher.buttonGuide == 1) {
         cout << "PAUSE" << endl;
      }

   }

   void render2()
   {
      glm::mat4 RotateX;
      glm::mat4 TransZ;
      glm::mat4 S;
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      double frametime = get_last_elapsed_time();
      GLFWwindow* window = windowManager->getHandle();
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      // Get current frame buffer size.
      int width, height;
      glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
      float aspect = width / (float)height;
      glViewport(0, 0, width, height);
      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);
      glfwSetCursorPos(window, 0.0, 0.0);
      mycam.mouseNew = glm::vec2(mouseX / width, mouseY / height);
      // Clear framebuffer.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Create the matrix stacks - please leave these alone for now

      glm::mat4 V, M, P; //View, Model and Perspective matrix
      V = mycam.process(frametime);
      M = glm::mat4(1);
      P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 100.0f); //so much type casting... GLM metods are quite funny ones

      float sangle = 3.1415926 / 2.;
      glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::vec3 camp = -mycam.pos;
      glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
      glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

      M = TransSky * RotateXSky * SSky;
      // Draw the box using GLSL.
      depth->bind();


      //send the matrices to the shaders
      glUniformMatrix4fv(depth->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(depth->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(depth->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //glUniform3fv(depth->getUniform("campos"), 1, &mycam.pos[0]);
      //glUniformMatrix4fv(depth->getUniform("lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix);

      glBindVertexArray(VertexArrayID);
      //actually draw from vertex 0, 3 vertices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);

      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, -17));
      S = glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 0.f));
      float angle = 3.1415926 / 2.0f;
      RotateX = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(1.0f, 0.0f, 0.0f));

      M = TransZ * S;
      glUniformMatrix4fv(depth->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
      glBindVertexArray(0);

      depth->unbind();
   }

   void renderFramebuffer()
   {
      /*for (int i = 0; i < enemBallList.size(); i++) {
         enemBallList[i].ballState = 0;
      }*/



      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      double frametime = get_last_elapsed_time();
      joystickUpdate();
      GLFWwindow* window = windowManager->getHandle();
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      // Get current frame buffer size.
      int width, height;
      glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
      float aspect = width / (float)height;
      glViewport(0, 0, width, height);
      //update frames
      Cipher.update(frametime);
      for (int i = 0; i < missileList.size(); i++) {
         missileList[i].update(frametime);
         if ((missileList[i].fuel <= 0.0f) || (missileList[i].missState == 2)) {
            missileList.erase(missileList.begin() + i);
         }
      }
      for (int i = 0; i < enemBallList.size(); i++) {
        //enemBallList[i].update(frametime);
         if (enemBallList[i].ballState == 2) {
            enemBallList.erase(enemBallList.begin() + i);
         }
      }
      Cipher.checkCollision();


      //double mouseX, mouseY;
      //glfwGetCursorPos(window, &mouseX, &mouseY);
      //glfwSetCursorPos(window, 0.0, 0.0);
      //mycam.mouseNew = glm::vec2(mouseX / width, mouseY / height);
      // Clear framebuffer.


      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //ligpos.x = -Cipher.pos.x;
      //ligpos.z = -Cipher.pos.z;
      //ligpos.y = 1000;




      // Create the matrix stacks - please leave these alone for now

      glm::mat4 V, M, P; //View, Model and Perspective matrix
      //V = mycam.process(frametime);
      M = glm::mat4(1);
      //P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
      float near_plane = 50.0f, far_plane = 1500.0f;
      P = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
      float sangle = 3.1415926 / 2.;
      //glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
      //glm::vec3 camp = -ligpos;
      //glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
      //glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
      V = glm::lookAt(glm::vec3(Cipher.pos.x, Cipher.pos.y + 1000, Cipher.pos.z),
         glm::vec3(Cipher.pos.x, Cipher.pos.y, Cipher.pos.z),
         glm::vec3(0.0f, 0.0f, 1.0f));
      lightView = V;
      lightPer = P;
      //float sangle = 3.1415926 / 2.;
      glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::vec3 camp = -mycam.pos;
      glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
      glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

      glm::mat4 RotateX;
      glm::mat4 TransZ;
      glm::mat4 S;

      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      // Draw the box using GLSL.
      shad->bind();


      //send the matrices to the shaders
      glUniformMatrix4fv(shad->getUniform("P"), 1, GL_FALSE, &P[0][0]);
      glUniformMatrix4fv(shad->getUniform("V"), 1, GL_FALSE, &V[0][0]);
      glUniformMatrix4fv(shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);

      glBindVertexArray(VertexArrayID);
      //actually draw from vertex 0, 3 vertices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
      //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture);

      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -20.0f, -17));
      S = glm::scale(glm::mat4(1.0f), glm::vec3(100.f, 100.f, 0.f));
      float angle = 3.1415926 / 2.0f;
      RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

      M = TransZ * RotateX * S;
      glUniformMatrix4fv(shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
      glBindVertexArray(0);
      S = glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 10.f));
      TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.7f, -17));
      M = TransZ * RotateX * S;
      glUniformMatrix4fv(shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      //shape->draw(shad);

      M = Cipher.getLoc();

      glUniformMatrix4fv(shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      if (Cipher.planeState == 0) {
         f18->draw(shad);
      }

      for (int i = 0; i < enemBallList.size(); i++) {
         enemBallList[i].drawEnemS();
      }
      for (int i = 0; i < missileList.size(); i++) {
         missileList[i].drawMissS();
      }

      shad->unbind();
      glDisable(GL_CULL_FACE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, depthMap);

   }


};
//******************************************************************************************
Application* appGlob;
int main(int argc, char** argv)
{
   std::string resourceDir = "../resources"; // Where the resources are loaded from
   if (argc >= 2)
   {
      resourceDir = argv[1];
   }

   Application* application = new Application();
   appGlob = application;
   /* your main will always include a similar set up to establish your window
      and GL context, etc. */
   WindowManager* windowManager = new WindowManager();
   windowManager->init(1920, 1080);
   windowManager->setEventCallbacks(application);
   application->windowManager = windowManager;


   /* This is the code that will likely change program to program as you
      may need to initialize or set up different data and state */
      // Initialize scene.
   application->init(resourceDir);
   application->initGeom();
   initGameObj();

   // Loop until the user closes the window.
   while (!glfwWindowShouldClose(windowManager->getHandle()))
   {
      //int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
      //cout << "JOYSTICK STATUS: " << present << endl;
      // Render scene.
      application->renderFramebuffer();
      application->render();
      //application->render2();

      // Swap front and back buffers.
      glfwSwapBuffers(windowManager->getHandle());
      // Poll for and process events.
      glfwPollEvents();
   }

   // Quit program.
   windowManager->shutdown();
   return 0;
}

void player::update(float frametime) {
   glm::mat4 rotX, rotY, rotz;
   glm::mat4 tranX, tranY, tranZ, T;
   if (camChange == 0 && buttonBack == 1) {
      camChange = 1;
      switch (camType) {
      case(firstP):
         camType = thirdP;
         break;
      case(thirdP):
         camType = testP;
         break;
      default:
         camType = firstP;
      }
   }
   else if (camChange != 0 && buttonBack == 0) {
      camChange = 0;
   }
   float rotXAngle = -PI / 2;
   if (planeState == 0) {
      zAcceleration = -50 * (zAcc + 1) + 50 * (zDec + 1);
      zSpe = zSpe + zAcceleration * frametime;
      if (zSpe > maxSpeed) {
         zSpe = maxSpeed;
      }
      else if (zSpe < -maxSpeed) {
         zSpe = -maxSpeed;
      }
      if (zSpe > 0){ 
         zSpe = 0;
      }
      if (hitChange == 0 && buttonUp == 1) {
         hitChange = 1;
         switch (hitState) {
         case(0):
            hitState = 1;
            break;
         case(1):
            hitState = 0;
            break;
         default:
            hitState = 0;
         }
      }
      else if (hitChange != 0 && buttonUp == 0) {
         hitChange = 0;
      }
      float newRotz = 2.5 * rollVal * frametime;
      newRotz = glm::clamp(newRotz, -5.0f, 5.0f);
      //rot.z += newRot;
      float newRotx = 0.25 * pitchVal * frametime;
      newRotx += glm::clamp(newRotx, -1.0f, 1.0f);
      float newRoty = (0.5 - 0.25 * (-zSpe / maxSpeed)) * buttonLB * frametime;
      newRoty -= (0.5 - 0.25 * (-zSpe / maxSpeed)) * buttonRB * frametime;
      newRoty += glm::clamp(newRoty, -0.5f, 0.5f);
      glm::mat4 R = glm::rotate(glm::mat4(1), newRoty, glm::vec3(0, 1, 0));
      glm::mat4 R2 = glm::rotate(glm::mat4(1), newRotx, glm::vec3(1, 0, 0));
      glm::mat4 R3 = glm::rotate(glm::mat4(1), newRotz, glm::vec3(0, 0, 1));
      glm::vec4 dir = glm::vec4(0, 0, zSpe, 1);
      rotMat = rotMat * R * R2 * R3;
      dir = rotMat * dir;
      /*for (int i = 0; i < 8; i++) {
         glm::vec4 transMat = glm::vec4(AABBhitbox[i], 1);
      }*/
      if (zSpe > -0.2) {
         ySpe = ySpe + ((1.2 + zSpe)/-0.2) * frametime;
      }
      else {
         ySpe = 0;
      }
      dir.y += ySpe;
      pos += glm::vec3(dir.x, dir.y, dir.z);


      AABBMin += glm::vec3(dir.x, dir.y, dir.z);
      AABBMax += glm::vec3(dir.x, dir.y, dir.z);
      T = glm::translate(glm::mat4(1), pos);
      /*for (int i = 0; i < 8; i++) {
         glm::vec4 transMat = glm::vec4(AABBhitbox[i], 1);
         transMat = T * transMat;
         AABBhitbox[i] = glm::vec3(transMat.x, transMat.y, transMat.z);
      }*/
      rotX = glm::rotate(glm::mat4(1.0f), rotXAngle, glm::vec3(1.0f, 0.0f, 0.0f));
      planeLoc = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 40.0f, 40.0f));
      planeLoc = T * rotMat * rotX * planeLoc;
      //CALCULATE NEW NORMALS
      for (int i = 0; i < 8; i++) {
         glm::vec4 transMat = glm::vec4(OOBBPoshitboxdef[i], 1);
         transMat = T * rotMat * transMat;
         OOBBPoshitbox[i] = glm::vec3(transMat.x, transMat.y, transMat.z);
         /*if (i < 2 || i>5) {
            OOBBPoshitbox[i] = glm::vec3(transMat.x, transMat.y, transMat.z);
         }
         else {
            OOBBPoshitbox[i] = glm::vec3(transMat.x, transMat.y - 10.0f, transMat.z);
         }*/
      }
      
      glm::vec3 QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[1];
      glm::vec3 QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[3];
      this->norm1 = glm::normalize(glm::cross(QS, QR));
      //cout << endl << endl << endl << endl << endl << endl;
      //cout << "NORM1: (" << this->norm1.x << ", " << this->norm1.y << ", " << this->norm1.z << ")" << endl;
      QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[1];
      QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[7];
      this->norm2 = -glm::normalize(glm::cross(QS, QR));

      //cout << "NORM2: (" << this->norm2.x << ", " << this->norm2.y << ", " << this->norm2.z << ")" << endl;
      QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[3];
      QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[7];
      this->norm3 = glm::normalize(glm::cross(QS, QR));

      //cout << "NORM3: (" << this->norm3.x << ", " << this->norm3.y << ", " << this->norm3.z << ")" << endl;
      if (missileLTime < 1) {
         missileLTime += frametime * 0.5;
      }
      if (missileRTime < 1) {
         missileRTime += frametime * 0.5;
      }
      glm::vec4 missOff = glm::vec4(1,1,1,1);
      glm::vec3 missOff3;
      if (buttonB && this->missileState == 0) {
         missileState = 1;
         if (missileLast == 0) {
            if (missileLTime >= 0.95) {
               missile newMiss;
               //T = glm::translate(glm::mat4(1), glm::vec3(-5, -2, 0));
               missOff = glm::vec4(-10, -2, 0, 1);
               missOff = rotMat * missOff;
               missOff3 = glm::vec3(missOff.x, missOff.y, missOff.z);
               newMiss.pos = (this->pos + missOff3);
               newMiss.missRot = this->norm3;
               newMiss.speed = 4;
               newMiss.rotMat = this->rotMat;
               missileList.push_back(newMiss);
               missileLast = 1;
               missileLTime = 0;
            }
            else if (missileRTime >= 0.95) {
               missile newMiss;
               missOff = glm::vec4(10, -2, 0, 1);
               missOff = rotMat * missOff;
               missOff3 = glm::vec3(missOff.x, missOff.y, missOff.z);
               newMiss.pos = (this->pos + missOff3);
               newMiss.missRot = this->norm3;
               newMiss.speed = 4;
               newMiss.rotMat = this->rotMat;
               missileList.push_back(newMiss);
               missileLast = 0;
               missileRTime = 0;
            }

         }
         else {
            if (missileRTime >= 0.95) {
               missile newMiss;
               missOff = glm::vec4(10, -2, 0, 1);
               missOff = rotMat * missOff;
               missOff3 = glm::vec3(missOff.x, missOff.y, missOff.z);
               newMiss.pos = (this->pos + missOff3);
               newMiss.missRot = this->norm3;
               newMiss.speed = 4;
               newMiss.rotMat = this->rotMat;
               missileList.push_back(newMiss);
               missileLast = 0;
               missileRTime = 0;
            }
            else if (missileLTime >= 0.95) {
               missile newMiss;
               missOff = glm::vec4(-10, -2, 0, 1);
               missOff = rotMat * missOff;
               missOff3 = glm::vec3(missOff.x, missOff.y, missOff.z);
               newMiss.pos = (this->pos + missOff3);
               newMiss.missRot = this->norm3;
               newMiss.speed = 4;
               newMiss.rotMat = this->rotMat;
               missileList.push_back(newMiss);
               missileLast = 1;
               missileLTime = 0;
            }
         }
      }
      else {
         if (this->buttonB == 0) {
            this->missileState = 0;
         }
      }
   }
   else if (planeState == 1) {
      if (this->countExp < 4) {
         this->countExp++;
      }
      else {
         if (this->expCount.x < 4) {
            expCount.x++;
         }
         else {
            if (this->expCount.y < 3) {
               expCount.x = 0;
               expCount.y++;
            }
            else {
               expCount.x = 0;
               expCount.y = 1;
            }
         }
         countExp = 0;
      }
   }
   
}

glm::mat4 player::getLoc() {
   return planeLoc;
}

void player::checkCollision() {
   //cout << endl << endl << endl << endl << endl << endl;
   //cout << "PLAYER MIN: (" << Cipher.AABBMin.x << ", " << Cipher.AABBMin.y << ", " << Cipher.AABBMin.z << ")" << endl;
   //cout << "PLAYER MAX: (" << Cipher.AABBMax.x << ", " << Cipher.AABBMax.y << ", " << Cipher.AABBMax.z << ")" << endl;
   for (int i = 0; i < enemBoxList.size(); i++) {
      //cout << "ENEM MIN: (" << enemBoxList[i].AABBMin.x << ", " << enemBoxList[i].AABBMin.y << ", " << enemBoxList[i].AABBMin.z << ")" << endl;
      //cout << "ENEM MAX: (" << enemBoxList[i].AABBMax.x << ", " << enemBoxList[i].AABBMax.y << ", " << enemBoxList[i].AABBMax.z << ")" << endl;

      //enemBoxList[i].boxCol = glm::vec4(1, 0, 0, 0.25);
      if (this->AABBMin.x <= enemBoxList[i].AABBMax.x && this->AABBMax.x >= enemBoxList[i].AABBMin.x) {
         if (this->AABBMin.y <= enemBoxList[i].AABBMax.y && this->AABBMax.y >= enemBoxList[i].AABBMin.y) {
            if (this->AABBMin.z <= enemBoxList[i].AABBMax.z && this->AABBMax.z >= enemBoxList[i].AABBMin.z) {
               if (this->OOBBColCheck(enemBoxList[i])) {
                  enemBoxList[i].boxCol = glm::vec4(0, 1, 0, 0.25);
               }
            }
         }
      }
   }
   for (int i = 0; i < enemBallList.size(); i++) {
      //cout << "ENEM MIN: (" << enemBoxList[i].AABBMin.x << ", " << enemBoxList[i].AABBMin.y << ", " << enemBoxList[i].AABBMin.z << ")" << endl;
      //cout << "ENEM MAX: (" << enemBoxList[i].AABBMax.x << ", " << enemBoxList[i].AABBMax.y << ", " << enemBoxList[i].AABBMax.z << ")" << endl;

      //enemBallList[i].boxCol = glm::vec4(1, 0, 0, 0.25);
      if (this->AABBMin.x <= enemBallList[i].AABBMax.x && this->AABBMax.x >= enemBallList[i].AABBMin.x) {
         if (this->AABBMin.y <= enemBallList[i].AABBMax.y && this->AABBMax.y >= enemBallList[i].AABBMin.y) {
            if (this->AABBMin.z <= enemBallList[i].AABBMax.z && this->AABBMax.z >= enemBallList[i].AABBMin.z) {
               if (this->OOBBColCheck(enemBallList[i])) {
                  enemBallList[i].ballState = 1;
                  this->planeState = 1;
               }
            }
         }
      }
   }
   if (this->pos.y < -490) {
      this->planeState = 1;
   }
}




void enemBox::drawEnem() {
   //requires boundBox to be boud
   glm::mat4 Tmat = glm::translate(glm::mat4(1), this->boxPos);
   glm::mat4 Smat = glm::scale(glm::mat4(1.0f), this->boxScale);
   glm::mat4 R = glm::rotate(glm::mat4(1), this->boxRot.y, glm::vec3(0, 1, 0));
   glm::mat4 R2 = glm::rotate(glm::mat4(1), this->boxRot.x, glm::vec3(1, 0, 0));
   glm::mat4 R3 = glm::rotate(glm::mat4(1), this->boxRot.z, glm::vec3(0, 0, 1));
   glUniform4fv(appGlob->boundBox->getUniform("incol"), 1, &this->boxCol[0]);
   glm::mat4 M = Tmat * R * R2 * R3 * Smat;
   glUniformMatrix4fv(appGlob->boundBox->getUniform("M"), 1, GL_FALSE, &M[0][0]);

   cube->draw(appGlob->boundBox);
}

void enemBall::drawEnem() {
   //requires boundBox to be boud
   if (this->ballState == 0) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->boxPos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), this->boxScale);
      glm::mat4 R = glm::rotate(glm::mat4(1), this->boxRot.y, glm::vec3(0, 1, 0));
      glm::mat4 R2 = glm::rotate(glm::mat4(1), this->boxRot.x, glm::vec3(1, 0, 0));
      glm::mat4 R3 = glm::rotate(glm::mat4(1), this->boxRot.z, glm::vec3(0, 0, 1));
      glm::mat4 M = Tmat * R * R2 * R3 * Smat;
      glUniformMatrix4fv(appGlob->plane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      balloon->draw(appGlob->plane);
   }
}

void enemBall::drawEnemE(glm::mat4 iVR) {
   //requires boundBox to be boud
   if (this->ballState == 1) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->boxPos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), this->boxScale * 10.0f);
      //glm::mat4 R = glm::rotate(glm::mat4(1), this->boxRot.y, glm::vec3(0, 1, 0));
      //glm::mat4 R2 = glm::rotate(glm::mat4(1), this->boxRot.x, glm::vec3(1, 0, 0));
      //glm::mat4 R3 = glm::rotate(glm::mat4(1), this->boxRot.z, glm::vec3(0, 0, 1));
      glm::mat4 M = Tmat * iVR* Smat;
      glUniformMatrix4fv(appGlob->explode->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      if (this->countExp < 4) {
         this->countExp++;
      }
      else {
         if (this->expCount.x < 4) {
            expCount.x++;
         }
         else {
            if (this->expCount.y < 4) {
               expCount.x = 0;
               expCount.y++;
            }
            else {
               this->ballState = 2;
            }
         }
         countExp = 0;
      }
   glUniform2fv(appGlob->explode->getUniform("texoff"), 1, &expCount[0]);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
   //glBindVertexArray(0);
   }
}

void enemBall::drawEnemS() {
   //requires boundBox to be boud
   if (this->ballState == 0) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->boxPos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), this->boxScale);
      glm::mat4 R = glm::rotate(glm::mat4(1), this->boxRot.y, glm::vec3(0, 1, 0));
      glm::mat4 R2 = glm::rotate(glm::mat4(1), this->boxRot.x, glm::vec3(1, 0, 0));
      glm::mat4 R3 = glm::rotate(glm::mat4(1), this->boxRot.z, glm::vec3(0, 0, 1));
      glm::mat4 M = Tmat * R * R2 * R3 * Smat;
      glUniformMatrix4fv(appGlob->shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      balloon->draw(appGlob->shad);
   }
}

void missile::drawMissE(glm::mat4 iVR) {
   //requires boundBox to be boud
   if (this->missState == 1) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->pos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
      //glm::mat4 R = glm::rotate(glm::mat4(1), this->boxRot.y, glm::vec3(0, 1, 0));
      //glm::mat4 R2 = glm::rotate(glm::mat4(1), this->boxRot.x, glm::vec3(1, 0, 0));
      //glm::mat4 R3 = glm::rotate(glm::mat4(1), this->boxRot.z, glm::vec3(0, 0, 1));
      glm::mat4 M = Tmat * iVR * Smat;
      glUniformMatrix4fv(appGlob->explode->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      if (this->countExp < 4) {
         this->countExp++;
      }
      else {
         if (this->expCount.x < 4) {
            expCount.x++;
         }
         else {
            if (this->expCount.y < 4) {
               expCount.x = 0;
               expCount.y++;
            }
            else {
               this->missState = 2;
            }
         }
         countExp = 0;
      }
      glUniform2fv(appGlob->explode->getUniform("texoff"), 1, &expCount[0]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
      //glBindVertexArray(0);*/
   }
}

void missile::drawMissS() {
   //requires boundBox to be boud
   if (this->missState == 0) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->pos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
      glm::mat4 M = Tmat * this->rotMat * Smat;


      glUniformMatrix4fv(appGlob->shad->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      sidewinder->draw(appGlob->shad);
   }
}

void initGameObj() {
   //enemyBoxes
   enemBox enemBox1(glm::vec3(0, 0, 0), glm::vec4(1, 0, 0, 0.25), glm::vec3(20, 20, 20), glm::vec3(0, 0, 0));
   //enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(0, 0, -100);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(0, 0, -100);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(0, 0, -200);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(0, -50, -300);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(0, 50, -400);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(50, 50, -500);
   enemBoxList.push_back(enemBox1);
   enemBox1.boxPos = glm::vec3(-50, -50, -600);
   enemBoxList.push_back(enemBox1);
   for (int i = 0; i < enemBoxList.size(); i++) {
      enemBoxList[i].updateCol();
   }
   //enemyBalloons
   enemBall enemBall1(glm::vec3(10, -100, -800), glm::vec4(1, 0, 0, 0.25), glm::vec3(20, 20, 20), glm::vec3(0, 0, 0));
   enemBallList.push_back(enemBall1);
   enemBall1.boxPos = glm::vec3(-40, 100, -1200);
   enemBallList.push_back(enemBall1);
   enemBall1.boxPos = glm::vec3(30, -200, -1400);
   enemBallList.push_back(enemBall1);
   enemBall1.boxPos = glm::vec3(-100, 300, -1600);
   enemBallList.push_back(enemBall1);
   enemBall1.boxPos = glm::vec3(0, 500, -1700);
   enemBallList.push_back(enemBall1);


   for (int i = 0; i < enemBallList.size(); i++) {
      enemBallList[i].updateCol();
   }

}
void enemBox::updateCol() {
   AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
   AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);

   OOBBPoshitbox[0] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBPoshitbox[1] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBPoshitbox[2] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBPoshitbox[3] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBPoshitbox[4] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBPoshitbox[5] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBPoshitbox[6] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBPoshitbox[7] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
}

void enemBall::updateCol() {
   AABBMin = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
   AABBMax = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);

   OOBBPoshitbox[0] = glm::vec3(this->boxPos.x - (this->boxScale.x / 2), this->boxPos.y - (this->boxScale.y / 2), this->boxPos.z - (this->boxScale.z / 2));
   OOBBPoshitbox[1] = glm::vec3(this->boxPos.x - (this->boxScale.x / 2), this->boxPos.y - (this->boxScale.y / 2), this->boxPos.z + (this->boxScale.z / 2));
   OOBBPoshitbox[2] = glm::vec3(this->boxPos.x - (this->boxScale.x / 2), this->boxPos.y + (this->boxScale.y / 2), this->boxPos.z + (this->boxScale.z / 2));
   OOBBPoshitbox[3] = glm::vec3(this->boxPos.x - (this->boxScale.x / 2), this->boxPos.y + (this->boxScale.y / 2), this->boxPos.z - (this->boxScale.z / 2));
   OOBBPoshitbox[4] = glm::vec3(this->boxPos.x + (this->boxScale.x / 2), this->boxPos.y + (this->boxScale.y / 2), this->boxPos.z - (this->boxScale.z / 2));
   OOBBPoshitbox[5] = glm::vec3(this->boxPos.x + (this->boxScale.x / 2), this->boxPos.y + (this->boxScale.y / 2), this->boxPos.z + (this->boxScale.z / 2));
   OOBBPoshitbox[6] = glm::vec3(this->boxPos.x + (this->boxScale.x / 2), this->boxPos.y - (this->boxScale.y / 2), this->boxPos.z + (this->boxScale.z / 2));
   OOBBPoshitbox[7] = glm::vec3(this->boxPos.x + (this->boxScale.x / 2), this->boxPos.y - (this->boxScale.y / 2), this->boxPos.z - (this->boxScale.z / 2));


   OOBBMishitbox[0] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBMishitbox[1] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBMishitbox[2] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBMishitbox[3] = glm::vec3(this->boxPos.x - this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBMishitbox[4] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z - this->boxScale.z);
   OOBBMishitbox[5] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y + this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBMishitbox[6] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z + this->boxScale.z);
   OOBBMishitbox[7] = glm::vec3(this->boxPos.x + this->boxScale.x, this->boxPos.y - this->boxScale.y, this->boxPos.z - this->boxScale.z);

}


bool player::OOBBColCheck(enemBox enemy) {
   glm::vec3 QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[1];
   glm::vec3 QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[3];
   glm::vec3 norm4 = glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[1];
   QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[7];
   glm::vec3 norm5 = -glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[3];
   QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[7];
   glm::vec3 norm6 = glm::normalize(glm::cross(QS, QR));
   float playerProjMax, playerProjMin;
   float enemyProjMax, enemyProjMin;
   float playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm1);
   float enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm1);
   playerProjMax = playerProj;
   playerProjMin = playerProj;
   enemyProjMax = enemyProj;
   enemyProjMin = enemyProj;
   for (int i = 1; i < 8; i++) {
      playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm1);
      enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm1);
      if (playerProj < playerProjMin) {
         playerProjMin = playerProj;
      }
      else if (playerProj > playerProjMax)
      {
         playerProjMax = playerProj;
      }
      if (enemyProj < enemyProjMin) {
         enemyProjMin = enemyProj;
      }
      else if (enemyProj > enemyProjMax)
      {
         enemyProjMax = enemyProj;
      }
   }
   if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
      playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm2);
      enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm2);
      playerProjMax = playerProj;
      playerProjMin = playerProj;
      enemyProjMax = enemyProj;
      enemyProjMin = enemyProj;
      for (int i = 1; i < 8; i++) {
         playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm2);
         enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm2);
         if (playerProj < playerProjMin) {
            playerProjMin = playerProj;
         }
         else if (playerProj > playerProjMax)
         {
            playerProjMax = playerProj;
         }
         if (enemyProj < enemyProjMin) {
            enemyProjMin = enemyProj;
         }
         else if (enemyProj > enemyProjMax)
         {
            enemyProjMax = enemyProj;
         }
      }
      if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
         playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm3);
         enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm3);
         playerProjMax = playerProj;
         playerProjMin = playerProj;
         enemyProjMax = enemyProj;
         enemyProjMin = enemyProj;
         for (int i = 1; i < 8; i++) {
            playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm3);
            enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm3);
            if (playerProj < playerProjMin) {
               playerProjMin = playerProj;
            }
            else if (playerProj > playerProjMax)
            {
               playerProjMax = playerProj;
            }
            if (enemyProj < enemyProjMin) {
               enemyProjMin = enemyProj;
            }
            else if (enemyProj > enemyProjMax)
            {
               enemyProjMax = enemyProj;
            }
         }
         if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
            playerProj = glm::dot(this->OOBBPoshitbox[0], norm4);
            enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm4);
            playerProjMax = playerProj;
            playerProjMin = playerProj;
            enemyProjMax = enemyProj;
            enemyProjMin = enemyProj;
            for (int i = 1; i < 8; i++) {
               playerProj = glm::dot(this->OOBBPoshitbox[i], norm4);
               enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm4);
               if (playerProj < playerProjMin) {
                  playerProjMin = playerProj;
               }
               else if (playerProj > playerProjMax)
               {
                  playerProjMax = playerProj;
               }
               if (enemyProj < enemyProjMin) {
                  enemyProjMin = enemyProj;
               }
               else if (enemyProj > enemyProjMax)
               {
                  enemyProjMax = enemyProj;
               }
            }
            if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
               playerProj = glm::dot(this->OOBBPoshitbox[0], norm5);
               enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm5);
               playerProjMax = playerProj;
               playerProjMin = playerProj;
               enemyProjMax = enemyProj;
               enemyProjMin = enemyProj;
               for (int i = 1; i < 8; i++) {
                  playerProj = glm::dot(this->OOBBPoshitbox[i], norm5);
                  enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm5);
                  if (playerProj < playerProjMin) {
                     playerProjMin = playerProj;
                  }
                  else if (playerProj > playerProjMax)
                  {
                     playerProjMax = playerProj;
                  }
                  if (enemyProj < enemyProjMin) {
                     enemyProjMin = enemyProj;
                  }
                  else if (enemyProj > enemyProjMax)
                  {
                     enemyProjMax = enemyProj;
                  }
               }
               if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
                  playerProj = glm::dot(this->OOBBPoshitbox[0], norm6);
                  enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm6);
                  playerProjMax = playerProj;
                  playerProjMin = playerProj;
                  enemyProjMax = enemyProj;
                  enemyProjMin = enemyProj;
                  for (int i = 1; i < 8; i++) {
                     playerProj = glm::dot(this->OOBBPoshitbox[i], norm6);
                     enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm6);
                     if (playerProj < playerProjMin) {
                        playerProjMin = playerProj;
                     }
                     else if (playerProj > playerProjMax)
                     {
                        playerProjMax = playerProj;
                     }
                     if (enemyProj < enemyProjMin) {
                        enemyProjMin = enemyProj;
                     }
                     else if (enemyProj > enemyProjMax)
                     {
                        enemyProjMax = enemyProj;
                     }
                     if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
                        return true;
                     }
                  }
               }
            }
         }
      }
   }
   return false;
}

bool player::OOBBColCheck(enemBall enemy) {
   glm::vec3 QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[1];
   glm::vec3 QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[3];
   glm::vec3 norm4 = glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[1];
   QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[7];
   glm::vec3 norm5 = glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[3];
   QR = enemy.OOBBPoshitbox[0] - enemy.OOBBPoshitbox[7];
   glm::vec3 norm6 = glm::normalize(glm::cross(QS, QR));
   float playerProjMax, playerProjMin;
   float enemyProjMax, enemyProjMin;
   float playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm1);
   float enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm1);
   playerProjMax = playerProj;
   playerProjMin = playerProj;
   enemyProjMax = enemyProj;
   enemyProjMin = enemyProj;
   for (int i = 1; i < 8; i++) {
      playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm1);
      enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm1);
      if (playerProj < playerProjMin) {
         playerProjMin = playerProj;
      }
      else if (playerProj > playerProjMax)
      {
         playerProjMax = playerProj;
      }
      if (enemyProj < enemyProjMin) {
         enemyProjMin = enemyProj;
      }
      else if (enemyProj > enemyProjMax)
      {
         enemyProjMax = enemyProj;
      }
   }
   if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
      playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm2);
      enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm2);
      playerProjMax = playerProj;
      playerProjMin = playerProj;
      enemyProjMax = enemyProj;
      enemyProjMin = enemyProj;
      for (int i = 1; i < 8; i++) {
         playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm2);
         enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm2);
         if (playerProj < playerProjMin) {
            playerProjMin = playerProj;
         }
         else if (playerProj > playerProjMax)
         {
            playerProjMax = playerProj;
         }
         if (enemyProj < enemyProjMin) {
            enemyProjMin = enemyProj;
         }
         else if (enemyProj > enemyProjMax)
         {
            enemyProjMax = enemyProj;
         }
      }
      if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
         playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm3);
         enemyProj = glm::dot(enemy.OOBBPoshitbox[0], this->norm3);
         playerProjMax = playerProj;
         playerProjMin = playerProj;
         enemyProjMax = enemyProj;
         enemyProjMin = enemyProj;
         for (int i = 1; i < 8; i++) {
            playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm3);
            enemyProj = glm::dot(enemy.OOBBPoshitbox[i], this->norm3);
            if (playerProj < playerProjMin) {
               playerProjMin = playerProj;
            }
            else if (playerProj > playerProjMax)
            {
               playerProjMax = playerProj;
            }
            if (enemyProj < enemyProjMin) {
               enemyProjMin = enemyProj;
            }
            else if (enemyProj > enemyProjMax)
            {
               enemyProjMax = enemyProj;
            }
         }
         if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
            playerProj = glm::dot(this->OOBBPoshitbox[0], norm4);
            enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm4);
            playerProjMax = playerProj;
            playerProjMin = playerProj;
            enemyProjMax = enemyProj;
            enemyProjMin = enemyProj;
            for (int i = 1; i < 8; i++) {
               playerProj = glm::dot(this->OOBBPoshitbox[i], norm4);
               enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm4);
               if (playerProj < playerProjMin) {
                  playerProjMin = playerProj;
               }
               else if (playerProj > playerProjMax)
               {
                  playerProjMax = playerProj;
               }
               if (enemyProj < enemyProjMin) {
                  enemyProjMin = enemyProj;
               }
               else if (enemyProj > enemyProjMax)
               {
                  enemyProjMax = enemyProj;
               }
            }
            if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
               playerProj = glm::dot(this->OOBBPoshitbox[0], norm5);
               enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm5);
               playerProjMax = playerProj;
               playerProjMin = playerProj;
               enemyProjMax = enemyProj;
               enemyProjMin = enemyProj;
               for (int i = 1; i < 8; i++) {
                  playerProj = glm::dot(this->OOBBPoshitbox[i], norm5);
                  enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm5);
                  if (playerProj < playerProjMin) {
                     playerProjMin = playerProj;
                  }
                  else if (playerProj > playerProjMax)
                  {
                     playerProjMax = playerProj;
                  }
                  if (enemyProj < enemyProjMin) {
                     enemyProjMin = enemyProj;
                  }
                  else if (enemyProj > enemyProjMax)
                  {
                     enemyProjMax = enemyProj;
                  }
               }
               if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
                  playerProj = glm::dot(this->OOBBPoshitbox[0], norm6);
                  enemyProj = glm::dot(enemy.OOBBPoshitbox[0], norm6);
                  playerProjMax = playerProj;
                  playerProjMin = playerProj;
                  enemyProjMax = enemyProj;
                  enemyProjMin = enemyProj;
                  for (int i = 1; i < 8; i++) {
                     playerProj = glm::dot(this->OOBBPoshitbox[i], norm6);
                     enemyProj = glm::dot(enemy.OOBBPoshitbox[i], norm6);
                     if (playerProj < playerProjMin) {
                        playerProjMin = playerProj;
                     }
                     else if (playerProj > playerProjMax)
                     {
                        playerProjMax = playerProj;
                     }
                     if (enemyProj < enemyProjMin) {
                        enemyProjMin = enemyProj;
                     }
                     else if (enemyProj > enemyProjMax)
                     {
                        enemyProjMax = enemyProj;
                     }
                     if (enemyProjMin > playerProjMax || playerProjMin < enemyProjMax) {
                        return true;
                     }
                  }
               }
            }
         }
      }
   }

   return false;
}

void missile::update(float frametime) {
   glm::vec3 dir;
   if (locked == 0) {
      /*glm::vec4 dir = glm::vec4(0, 0, 40 * frametime, 1);
      glm::mat4 R = glm::rotate(glm::mat4(1), (float)(this->missRot.y * PI), glm::vec3(0, 1, 0));
      glm::mat4 R2 = glm::rotate(glm::mat4(1), (float)(this->missRot.x * PI), glm::vec3(1, 0, 0));
      glm::mat4 R3 = glm::rotate(glm::mat4(1), (float)(this->missRot.z * PI), glm::vec3(0, 0, 1));
      dir = R * R2 * R3 * dir;*/
      dir = this->speed * this->missRot;
      this->pos.x += (dir.x);
      this->pos.y += (dir.y);
      this->pos.z += (dir.z);

   }
   AABBMin = glm::vec3(pos.x - 2.0f, pos.y - 2.0f, pos.z - 2.0f);
   AABBMax = glm::vec3(pos.x + 2.0f, pos.y + 2.0f, pos.z + 2.0f);

   glm::mat4 T = glm::translate(glm::mat4(1), pos);
   for (int i = 0; i < 8; i++) {
      glm::vec4 transMat = glm::vec4(OOBBPoshitboxdef[i], 1);
      transMat = T * rotMat * transMat;
      OOBBPoshitbox[i] = glm::vec3(transMat.x, transMat.y, transMat.z);

   }
   glm::vec3 QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[1];
   glm::vec3 QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[3];
   this->norm1 = glm::normalize(glm::cross(QS, QR));
   cout << endl << endl << endl << endl << endl << endl;
   //cout << "NORM1: (" << this->norm1.x << ", " << this->norm1.y << ", " << this->norm1.z << ")" << endl;
   QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[1];
   QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[7];
   this->norm2 = -glm::normalize(glm::cross(QS, QR));

   //cout << "NORM2: (" << this->norm2.x << ", " << this->norm2.y << ", " << this->norm2.z << ")" << endl;
   QS = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[3];
   QR = this->OOBBPoshitbox[0] - this->OOBBPoshitbox[7];
   this->norm3 = glm::normalize(glm::cross(QS, QR));

   fuel -= frametime;

   for (int i = 0; i < enemBallList.size(); i++) {
      //cout << "ENEM MIN: (" << enemBoxList[i].AABBMin.x << ", " << enemBoxList[i].AABBMin.y << ", " << enemBoxList[i].AABBMin.z << ")" << endl;
      //cout << "ENEM MAX: (" << enemBoxList[i].AABBMax.x << ", " << enemBoxList[i].AABBMax.y << ", " << enemBoxList[i].AABBMax.z << ")" << endl;
      //enemBallList[i].boxCol = glm::vec4(1, 0, 0, 0.25);
      if (this->AABBMin.x <= enemBallList[i].AABBMax.x && this->AABBMax.x >= enemBallList[i].AABBMin.x) {
         if (this->AABBMin.y <= enemBallList[i].AABBMax.y && this->AABBMax.y >= enemBallList[i].AABBMin.y) {
            if (this->AABBMin.z <= enemBallList[i].AABBMax.z && this->AABBMax.z >= enemBallList[i].AABBMin.z) {
               if (this->OOBBColCheck(enemBallList[i])) {
                  enemBallList[i].ballState = 1;
                  this->missState = 1;
               }
            }
         }
      }
   }
   if (this->pos.y < -500) {

   }
}

void missile::drawMiss() {
   //requires boundBox to be boud
   if (this->missState == 0) {
      glm::mat4 Tmat = glm::translate(glm::mat4(1), this->pos);
      glm::mat4 Smat = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
      //glm::mat4 R = glm::rotate(glm::mat4(1), (float)(this->missRot.y * -PI / 12.0f), glm::vec3(0, 1, 0));
      //glm::mat4 R2 = glm::rotate(glm::mat4(1), (float)(this->missRot.x * PI / 12.0f), glm::vec3(1, 0, 0));
      //glm::mat4 R3 = glm::rotate(glm::mat4(1), (float)(this->missRot.z * PI / 12.0f), glm::vec3(0, 0, 1));
     // glm::mat4 M = Tmat * R * R2 * R3 * Smat;
      glm::mat4 M = Tmat * this->rotMat * Smat;


      glUniformMatrix4fv(appGlob->plane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
      sidewinder->draw(appGlob->plane);
   }
}

bool missile::OOBBColCheck(enemBall enemy) {
   glm::vec3 QS = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[1];
   glm::vec3 QR = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[3];
   glm::vec3 norm4 = glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[1];
   QR = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[7];
   glm::vec3 norm5 = glm::normalize(glm::cross(QS, QR));
   QS = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[3];
   QR = enemy.OOBBMishitbox[0] - enemy.OOBBMishitbox[7];
   glm::vec3 norm6 = glm::normalize(glm::cross(QS, QR));
   float playerProjMax, playerProjMin;
   float enemyProjMax, enemyProjMin;
   float playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm1);
   float enemyProj = glm::dot(enemy.OOBBMishitbox[0], this->norm1);
   playerProjMax = playerProj;
   playerProjMin = playerProj;
   enemyProjMax = enemyProj;
   enemyProjMin = enemyProj;
   for (int i = 1; i < 8; i++) {
      playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm1);
      enemyProj = glm::dot(enemy.OOBBMishitbox[i], this->norm1);
      if (playerProj < playerProjMin) {
         playerProjMin = playerProj;
      }
      else if (playerProj > playerProjMax)
      {
         playerProjMax = playerProj;
      }
      if (enemyProj < enemyProjMin) {
         enemyProjMin = enemyProj;
      }
      else if (enemyProj > enemyProjMax)
      {
         enemyProjMax = enemyProj;
      }
   }
   if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
      playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm2);
      enemyProj = glm::dot(enemy.OOBBMishitbox[0], this->norm2);
      playerProjMax = playerProj;
      playerProjMin = playerProj;
      enemyProjMax = enemyProj;
      enemyProjMin = enemyProj;
      for (int i = 1; i < 8; i++) {
         playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm2);
         enemyProj = glm::dot(enemy.OOBBMishitbox[i], this->norm2);
         if (playerProj < playerProjMin) {
            playerProjMin = playerProj;
         }
         else if (playerProj > playerProjMax)
         {
            playerProjMax = playerProj;
         }
         if (enemyProj < enemyProjMin) {
            enemyProjMin = enemyProj;
         }
         else if (enemyProj > enemyProjMax)
         {
            enemyProjMax = enemyProj;
         }
      }
      if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
         playerProj = glm::dot(this->OOBBPoshitbox[0], this->norm3);
         enemyProj = glm::dot(enemy.OOBBMishitbox[0], this->norm3);
         playerProjMax = playerProj;
         playerProjMin = playerProj;
         enemyProjMax = enemyProj;
         enemyProjMin = enemyProj;
         for (int i = 1; i < 8; i++) {
            playerProj = glm::dot(this->OOBBPoshitbox[i], this->norm3);
            enemyProj = glm::dot(enemy.OOBBMishitbox[i], this->norm3);
            if (playerProj < playerProjMin) {
               playerProjMin = playerProj;
            }
            else if (playerProj > playerProjMax)
            {
               playerProjMax = playerProj;
            }
            if (enemyProj < enemyProjMin) {
               enemyProjMin = enemyProj;
            }
            else if (enemyProj > enemyProjMax)
            {
               enemyProjMax = enemyProj;
            }
         }
         if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
            playerProj = glm::dot(this->OOBBPoshitbox[0], norm4);
            enemyProj = glm::dot(enemy.OOBBMishitbox[0], norm4);
            playerProjMax = playerProj;
            playerProjMin = playerProj;
            enemyProjMax = enemyProj;
            enemyProjMin = enemyProj;
            for (int i = 1; i < 8; i++) {
               playerProj = glm::dot(this->OOBBPoshitbox[i], norm4);
               enemyProj = glm::dot(enemy.OOBBMishitbox[i], norm4);
               if (playerProj < playerProjMin) {
                  playerProjMin = playerProj;
               }
               else if (playerProj > playerProjMax)
               {
                  playerProjMax = playerProj;
               }
               if (enemyProj < enemyProjMin) {
                  enemyProjMin = enemyProj;
               }
               else if (enemyProj > enemyProjMax)
               {
                  enemyProjMax = enemyProj;
               }
            }
            if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
               playerProj = glm::dot(this->OOBBPoshitbox[0], norm5);
               enemyProj = glm::dot(enemy.OOBBMishitbox[0], norm5);
               playerProjMax = playerProj;
               playerProjMin = playerProj;
               enemyProjMax = enemyProj;
               enemyProjMin = enemyProj;
               for (int i = 1; i < 8; i++) {
                  playerProj = glm::dot(this->OOBBPoshitbox[i], norm5);
                  enemyProj = glm::dot(enemy.OOBBMishitbox[i], norm5);
                  if (playerProj < playerProjMin) {
                     playerProjMin = playerProj;
                  }
                  else if (playerProj > playerProjMax)
                  {
                     playerProjMax = playerProj;
                  }
                  if (enemyProj < enemyProjMin) {
                     enemyProjMin = enemyProj;
                  }
                  else if (enemyProj > enemyProjMax)
                  {
                     enemyProjMax = enemyProj;
                  }
               }
               if ((enemyProjMin < playerProjMin && enemyProjMax > playerProjMin) || (playerProjMin < enemyProjMin && playerProjMax > enemyProjMin)) {
                  playerProj = glm::dot(this->OOBBPoshitbox[0], norm6);
                  enemyProj = glm::dot(enemy.OOBBMishitbox[0], norm6);
                  playerProjMax = playerProj;
                  playerProjMin = playerProj;
                  enemyProjMax = enemyProj;
                  enemyProjMin = enemyProj;
                  for (int i = 1; i < 8; i++) {
                     playerProj = glm::dot(this->OOBBPoshitbox[i], norm6);
                     enemyProj = glm::dot(enemy.OOBBMishitbox[i], norm6);
                     if (playerProj < playerProjMin) {
                        playerProjMin = playerProj;
                     }
                     else if (playerProj > playerProjMax)
                     {
                        playerProjMax = playerProj;
                     }
                     if (enemyProj < enemyProjMin) {
                        enemyProjMin = enemyProj;
                     }
                     else if (enemyProj > enemyProjMax)
                     {
                        enemyProjMax = enemyProj;
                     }
                     if (enemyProjMin > playerProjMax || playerProjMin < enemyProjMax) {
                        return true;
                     }
                  }
               }
            }
         }
      }
   }

   return false;
}


