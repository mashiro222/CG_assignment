//   Copyright © 2021, Renjie Chen @ USTC

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define FREEGLUT_STATIC
#include "gl_core_3_3.h"
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#define TW_STATIC
#include <AntTweakBar.h>

#include <vector>
#include <string>
#include <iostream>
#include "glprogram.h"
#include "MyImage.h"
#include "VAOImage.h"
#include "VAOMesh.h"
using namespace std;

GLProgram MyMesh::prog;

MyMesh M;
int viewport[4] = { 0, 0, 1280, 960 };

bool showATB = true;

std::string imagefile = "boy.png";

MyImage img;
int resize_width, resize_height;

int mousePressButton;
int mouseButtonDown;
int mousePos[2];

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, viewport[2], viewport[3]);
    M.draw(viewport);

    if (showATB) TwDraw();
    glutSwapBuffers();
}

void onKeyboard(unsigned char code, int x, int y)
{
    if (!TwEventKeyboardGLUT(code, x, y)) {
        switch (code) {
        case 17:
            exit(0);
        case 'f':
            glutFullScreenToggle();
            break;
        case ' ':
            showATB = !showATB;
            break;
        }
    }

    glutPostRedisplay();
}

void onMouseButton(int button, int updown, int x, int y)
{
    if (!showATB || !TwEventMouseButtonGLUT(button, updown, x, y)) {
        mousePressButton = button;
        mouseButtonDown = updown;

        mousePos[0] = x;
        mousePos[1] = y;
    }

    glutPostRedisplay();
}


void onMouseMove(int x, int y)
{
    if (!showATB || !TwEventMouseMotionGLUT(x, y)) {
        if (mouseButtonDown == GLUT_DOWN) {
            if (mousePressButton == GLUT_MIDDLE_BUTTON) {
                M.moveInScreen(mousePos[0], mousePos[1], x, y, viewport);
            }
        }
    }

    mousePos[0] = x; mousePos[1] = y;

    glutPostRedisplay();
}


void onMouseWheel(int wheel_number, int direction, int x, int y)
{
    if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    }
    else
        M.mMeshScale *= direction > 0 ? 1.1f : 0.9f;

    glutPostRedisplay();
}

int initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(960, 960);
    glutInitWindowPosition(200, 50);
    glutCreateWindow(argv[0]);

    // !Load the OpenGL functions. after the opengl context has been created
    if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
        return -1;

    glClearColor(1.f, 1.f, 1.f, 0.f);

    glutReshapeFunc([](int w, int h) { viewport[2] = w; viewport[3] = h; TwWindowSize(w, h); });
    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMouseMove);
    glutMouseWheelFunc(onMouseWheel);
    glutCloseFunc([]() {exit(0); });
    return 0;
}

void uploadImage(const MyImage& img)
{
    int w = img.width();
    int h = img.height();
    float x[] = { 0, 0, 0, w, 0, 0, w, h, 0, 0, h, 0 };
    M.upload(x, 4, nullptr, 0, nullptr);

    M.tex.setImage(img);
    M.tex.setClamping(GL_CLAMP_TO_EDGE);
}


void readImage(const std::string& file)
{
    int w0 = img.width(), h0 = img.height();
    img = MyImage(file);
    uploadImage(img);
    resize_width = img.width();
    resize_height = img.height();

    if (w0 != img.width() || h0 != img.height()) M.updateBBox();
}


int Min(int a, int b, int c) {
    int min = a;
    if (min > b) min = b;
    if (min > c) min = c;
    return min;
}
int Min(int a, int b) {
    int min = a;
    if (min > b) min = b;
    return min;
}
int Num(int a, int b, int c) {
    int min = Min(a, b, c);
    if (min == a) return 3;
    else if (min == b) return 2;
    else return 1;
}
int Num(int a, int b) {
    int min = Min(a, b);
    if (min == a) return 2;
    else if (min == b) return 1;
}


MyImage seamCarving(const MyImage& img, int w, int h)//img是男孩图，hw是制定图像尺寸
{
    MyImage im("boy_saliency.png");//加个断点判断一下维度是不是1
    MyImage img2 = img; MyImage img3 = img;
    //hh,ww,dim为原图的长宽高
    int hh = img.height();
    int ww = img.width();
    int dim = img.dim();
    int i, j, k;    
    int num = 0; int min = 0;

    
    //计算列要扩大/缩小到多少
    int wgoal = w * hh / h + 1;//int型赋值直接向下取整，这样取缩小时就一定可以找到原像
    int type = ((wgoal - ww) <= 0) ? -1 : 1;//扩大还是缩小
    vector<int>p(hh, 0);//存储seam


    //拉伸情况
    if (type == 1) {
        if ((wgoal - ww) > 50) {
            wgoal = 50 + ww;
        }
        vector<vector<int>> E(hh, vector<int>(ww, 0));//存储每个点的代价
        int wtemp = ww;
        for (i = 0; i < hh; i++) {
            for (j = 0; j < ww; j++) {
                E[i][j] = int(im(i, j, 0));//代价随便取一个im的维度
            }
        }
        while (wtemp != wgoal) {
            vector<vector<int>> M(hh, vector<int>(ww, 0));//存储路径的代价
            vector<vector<int>> path(hh, vector<int>(ww, 0));//存储
            //找到seam，并给出path   
            for (i = 0; i < ww; i++) {
                M[0][i] = E[0][i];
            }
            for (i = 1; i < hh; i++) {
                for (j = 0; j < ww; j++) {
                    if (j == 0) {
                        M[i][j] = E[i][j] + Min(int(M[i - 1][j + 1]), int(M[i - 1][j]));
                        num = Num(int(M[i - 1][j + 1]), int(M[i - 1][j]));
                        path[i - 1][j] = j - 1 + num;
                    }
                    else if (j == ww - 1) {
                        M[i][j] = E[i][j] + Min(int(M[i - 1][j]), int(M[i - 1][j - 1]));
                        num = Num(int(M[i - 1][j]), int(M[i - 1][j - 1]));
                        path[i - 1][j] = j - 2 + num;

                    }
                    else {
                        M[i][j] = E[i][j] + Min(M[i - 1][j + 1], M[i - 1][j], M[i - 1][j - 1]);
                        num = Num(M[i - 1][j + 1], M[i - 1][j], M[i - 1][j - 1]);
                        path[i - 1][j] = j - 2 + num;//path[i-1][j]存储ij点的上一个点的纵坐标                    
                    }
                }
            }
            //把最后一行最小的那个取出来,num就是他的列数
            min = 1e10;
            for (i = 0; i < ww; i++) {
                if (min > M[hh - 1][i]) {
                    min = M[hh - 1][i];
                    num = i;
                }
            }
            //把这个seam上的所有点都给很大的代价，这样可以让seam不相交
            p[hh - 1] = num;
            E[hh - 1][num] = 10000;
            for (i = hh - 2; i >= 0; i--) {
                p[i] = path[i][p[i + 1]];
                E[i][p[i]] = 10000;
            }
            wtemp++;
        }//这样循环所有seam上的点都是1e6了

        vector<BYTE> data;
        int it = 0;//这步精妙

        for (i = 0; i < hh; i++) {
            for (j = 0; j < ww; j++) {
                if (E[i][j] == 10000) {
                    for (k = 0; k < dim; k++) {
                        data.push_back(img2(i, j, k));
                        //data[dim * (i * wgoal + j) + k] = img2(i, j, k);
                    }
                    for (k = 0; k < dim; k++) {
                        data.push_back(img2(i, j, k));
                        //data[dim * (i * wgoal + j) + k] = (img2(i, j, k) + img2(i, j + 1, k)) / 2;
                    }
                }
                else {
                    for (k = 0; k < dim; k++) {
                        data.push_back(img2(i, j, k));
                        //data[dim * (i * wgoal + j) + k] = img2(i, j, k);

                    }
                }
            }
        }
        BYTE* data2 = &data[0]; 
        img2 = MyImage(data2, wgoal, hh, 4 * wgoal, 4);
        return img2.rescale(w, h);
    }


    //压缩情况
    while (ww != wgoal) {
        
        vector<vector<int>> E(hh, vector<int>(ww, 0));//存储每个点的代价
        vector<vector<int>> M(hh, vector<int>(ww, 0));//存储路径的代价
        vector<vector<int>> path(hh, vector<int>(ww, 0));//存储
        for (i = 0; i < hh; i++) {
            for (j = 0; j < ww; j++) {
                E[i][j] = int(im(i, j, 0));//代价随便取一个im的维度
            }
        }
        //找到seam，并给出path   
        for (i = 0; i < ww; i++) {
            M[0][i] = E[0][i];
        }
        for (i = 1; i < hh; i++) {
            for (j = 0; j < ww; j++) {
                if (j == 0) {
                    M[i][j] = E[i][j] + Min(int(M[i - 1][j + 1]), int(M[i - 1][j]));
                    num = Num(int(M[i - 1][j + 1]), int(M[i - 1][j]));
                    path[i - 1][j] = j - 1 + num;
                }
                else if (j == ww - 1) { 
                    M[i][j] = E[i][j] + Min(int(M[i - 1][j]), int(M[i - 1][j - 1]));
                    num = Num(int(M[i - 1][j]), int(M[i - 1][j - 1]));
                    path[i - 1][j] = j - 2 + num;
                    
                }
                else {                    
                    M[i][j] = E[i][j] + Min(M[i - 1][j + 1], M[i - 1][j], M[i - 1][j - 1]);                    
                    num = Num(M[i - 1][j + 1], M[i - 1][j], M[i - 1][j - 1]);
                    path[i - 1][j] = j - 2 + num;//path[i-1][j]存储ij点的上一个点的纵坐标                    
                }                
            }
        }
        //把最后一行最大的那个取出来,num就是他的列数
        min = 1e10;
        for (i = 0; i < ww; i++) {
            if (min > M[hh - 1][i]) {
                min = M[hh - 1][i];
                num = i;
            }
        }
        //把这个seam的路径复现出来
        p[hh - 1] = num;
        for (i = hh - 2; i >= 0; i--) {
            p[i] = path[i][p[i + 1]];
        }
        //seamcarv函数用于去掉这条线
        vector<BYTE> data(dim * hh * (ww + type));//插入没有被删掉的点
        vector<BYTE> data1(dim * hh * (ww + type));//插入没有被删掉的点
        if (type == -1)
        {
            for (int i = 0; i < hh; i++) {
                for (int j = 0; j < p[i]; j++) {
                    for (int k = 0; k < dim; k++) {
                        //data.push_back(im(i, j, k));
                        data[dim * (i * (ww + type) + j) + k] = img2(i, j, k);
                        data1[dim * (i * (ww + type) + j) + k] = im(i, j, k);
                    }
                }
                for (int j = p[i]; j < ww - 1; j++) {
                    for (int k = 0; k < dim; k++) {
                        //data.push_back(im(i, j + 1, k));
                        data[dim * (i * (ww + type) + j) + k] = img2(i, j + 1, k);
                        data1[dim * (i * (ww + type) + j) + k] = im(i, j + 1, k);
                    }
                }
            }
        }
        BYTE* data2 = &data[0]; BYTE* data3 = &data1[0];
        img2 = MyImage(data2, ww + type, hh, 4 * (ww + type), 4);
        im = MyImage(data3, ww + type, hh, 4 * (ww + type), 4);
        ww += type;
    }
    //最后再直接缩放
    return img2.rescale(w,h);
}

void createTweakbar()
{
    //Create a tweak bar
    TwBar *bar = TwNewBar("Image Viewer");
    TwDefine(" 'Image Viewer' size='220 150' color='0 128 255' text=dark alpha=128 position='5 5'"); // change default tweak bar size and color

    TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &M.mMeshScale, " min=0 step=0.1");

    TwAddVarRW(bar, "Image filename", TW_TYPE_STDSTRING, &imagefile, " ");
    TwAddButton(bar, "Read Image", [](void*) { readImage(imagefile); }, nullptr, "");

    TwAddVarRW(bar, "Resize Width", TW_TYPE_INT32, &resize_width, "group='Seam Carving' min=1 ");
    TwAddVarRW(bar, "Resize Height", TW_TYPE_INT32, &resize_height, "group='Seam Carving' min=1 ");
    TwAddButton(bar, "Run Seam Carving", [](void* img) {
        MyImage newimg = seamCarving(*(const MyImage*)img, resize_width, resize_height);
        uploadImage(newimg);
        }, 
        &img, "");
}


int main(int argc, char *argv[])
{
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), { 100, 5000 });

    if (initGL(argc, argv)) {
        fprintf(stderr, "!Failed to initialize OpenGL!Exit...");
        exit(-1);
    }

    MyMesh::buildShaders();


    float x[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0 };
    float uv[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
    int t[] = { 0, 1, 2, 2, 3, 0 };

    M.upload(x, 4, t, 2, uv);

    //////////////////////////////////////////////////////////////////////////
    TwInit(TW_OPENGL_CORE, NULL);
    //Send 'glutGetModifers' function pointer to AntTweakBar;
    //required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);
    glutSpecialFunc([](int key, int x, int y) { TwEventSpecialGLUT(key, x, y); glutPostRedisplay(); }); // important for special keys like UP/DOWN/LEFT/RIGHT ...
    TwCopyStdStringToClientFunc([](std::string& dst, const std::string& src) {dst = src; });


    createTweakbar();

    //////////////////////////////////////////////////////////////////////////
    atexit([] { TwDeleteAllBars();  TwTerminate(); });  // Called after glutMainLoop ends

    glutTimerFunc(1, [](int) { readImage(imagefile); },  0);


    //////////////////////////////////////////////////////////////////////////
    glutMainLoop();

    return 0;
}

