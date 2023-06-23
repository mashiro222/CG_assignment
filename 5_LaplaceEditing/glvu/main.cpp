#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define FREEGLUT_STATIC
#include "gl_core_3_3.h"
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#define TW_STATIC
#include <AntTweakBar.h>

#include <iostream>
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>

#include "objloader.h"
#include "glprogram.h"
#include "MyImage.h"
#include "VAOImage.h"
#include "VAOMesh.h"
#include "trackball.h"

#include "laplacian.h"


GLProgram MyMesh::prog, MyMesh::pickProg, MyMesh::pointSetProg;
GLTexture MyMesh::colormapTex;

MyMesh M;
int viewport[4] = { 0, 0, 1280, 960 };
int actPrimType = MyMesh::PE_VERTEX;

bool showATB = true;

using MatX3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using MatX3i = Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>;
MatX3f meshX;
MatX3i meshF;

Eigen::Matrix<float, 3, 3, Eigen::RowMajor>R; //��Ԫ��ת������ת����
float myRotation[4] = { 1, 0, 0, 0 }; //BONUS: interactively specify the rotation for the Laplacian coordinates at the handles
void rotation() {//�õ���ת����
    float w = myRotation[0], x = myRotation[1], y = myRotation[2], z = myRotation[3];
    //ppt12
    R(0, 0) = 1 - 2 * y * y - 2 * z * z; R(0, 1) = 2 * x * y - 2 * w * z; R(0, 2) = 2 * x * z + 2 * w * y;
    R(1, 0) = 2 * x * y + 2 * w * z; R(1, 1) = 1 - 2 * x * x - 2 * z * z; R(1, 2) = 2 * y * z - 2 * w * x;
    R(2, 0) = 2 * x * z - 2 * w * y; R(2, 1) = 2 * y * z - 2 * w * x; R(2, 2) = 1 - 2 * y * y - 2 * x * x;

}

Eigen::SparseMatrix<float> L;//L��laplace����
void deform_preprocess()
{
    L = Laplacian_cotweight(meshX, meshF);
}


void meshDeform()
{
    using namespace Eigen;
    // Ids�Ǹĵĵ��ţ�Dsts�ǸĶ����λ��
    std::vector<int> P2PVtxIds = M.getConstrainVertexIds();
    std::vector<float> p2pDsts = M.getConstrainVertexCoords();//ע��ʹ��vector�洢�ģ��ӱ�
    
    deform_preprocess();//�õ�laplace����
    MatX3f b = L * meshX;
    b = (R * b.transpose()).transpose();

    //��ָ�����Ȩ��ʵ�֣�L+lambda*I��y=Lx+lambda*z�����Ȩ����Ҫȡ�ıȽϴ�
    for (int i = 0; i < P2PVtxIds.size(); i++) {
        float l = 100000;//10��100��1000��100000���Թ���10000Ч�����
        L.coeffRef(P2PVtxIds[i], P2PVtxIds[i]) += l;//coeffRefΪEigen�Դ��ĺ������������ã�ֱ�Ӷ�λ��Ԫ���޸�
        b(P2PVtxIds[i], 0) += p2pDsts[3 * i + 0] * l;
        b(P2PVtxIds[i], 1) += p2pDsts[3 * i + 1] * l;
        b(P2PVtxIds[i], 2) += p2pDsts[3 * i + 2] * l;
    }

    //����ʹ�õ���Eigen��ϡ���������ʡȥ����֤�ֽ��Ƿ�ɹ��Ĳ���
    SimplicialLDLT<SparseMatrix<float, ColMajor>> solver;
    solver.compute(L);
    Matrix<float, Dynamic, Dynamic, RowMajor> y;   
    y = solver.solve(b);
    
    if (y.cols() > 3)  y = y.leftCols(3); 
    if (y.rows() == 0 || y.cols() != 3) return;

    M.upload(y.data(), y.rows(), nullptr, 0, nullptr);
}

int mousePressButton;
int mouseButtonDown;
int mousePos[2];

bool msaa = true;



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (msaa) glEnable(GL_MULTISAMPLE);
    else glDisable(GL_MULTISAMPLE);

    glViewport(0, 0, viewport[2], viewport[3]);
    M.draw(viewport);

    if (showATB) TwDraw();
    glutSwapBuffers();

    //glFinish();
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

        if (updown == GLUT_DOWN) {
            if (button == GLUT_LEFT_BUTTON) {
                if (glutGetModifiers()&GLUT_ACTIVE_CTRL) {
                }
                else {
                    int r = M.pick(x, y, viewport, M.PE_VERTEX, M.PO_ADD);
                }
            }
            else if (button == GLUT_RIGHT_BUTTON) {
                M.pick(x, y, viewport, M.PE_VERTEX, M.PO_REMOVE);
            }
        }
        else { // updown == GLUT_UP
            if (button == GLUT_LEFT_BUTTON);
        }

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
            else if (mousePressButton == GLUT_LEFT_BUTTON) {
                if (!M.moveCurrentVertex(x, y, viewport)) {
                    meshDeform();
                }
                else {
                    const float s[2] = { 2.f / viewport[2], 2.f / viewport[3] };
                    auto r = Quat<float>(M.mQRotate)*Quat<float>::trackball(x*s[0] - 1, 1 - y*s[1], s[0]*mousePos[0] - 1, 1 - s[1]*mousePos[1]);
                    std::copy_n(r.q, 4, M.mQRotate);
                }
            }
        }
    }

    mousePos[0] = x; mousePos[1] = y;

    glutPostRedisplay();
}


void onMouseWheel(int wheel_number, int direction, int x, int y)
{
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

//AntTweakBar��
void TW_CALL bonus(void*)
{
    rotation();
    meshDeform();
}
void createTweakbar()
{
    TwBar *bar = TwGetBarByName("MeshViewer");
    if (bar)    TwDeleteBar(bar);

    //Create a tweak bar
    bar = TwNewBar("MeshViewer");
    TwDefine(" MeshViewer size='220 100' color='0 128 255' text=dark alpha=128 position='5 5'"); // change default tweak bar size and color

    TwAddVarRO(bar, "#Vertex", TW_TYPE_INT32, &M.nVertex, " group='Mesh View'");
    TwAddVarRO(bar, "#Face", TW_TYPE_INT32, &M.nFace, " group='Mesh View'");

    TwAddVarRW(bar, "Point Size", TW_TYPE_FLOAT, &M.pointSize, " group='Mesh View' ");
    TwAddVarRW(bar, "Vertex Color", TW_TYPE_COLOR4F, M.vertexColor.data(), " group='Mesh View' help='mesh vertex color' ");


    TwAddVarRW(bar, "Edge Width", TW_TYPE_FLOAT, &M.edgeWidth, " group='Mesh View' ");
    TwAddVarRW(bar, "Edge Color", TW_TYPE_COLOR4F, M.edgeColor.data(), " group='Mesh View' help='mesh edge color' ");

    TwAddVarRW(bar, "Face Color", TW_TYPE_COLOR4F, M.faceColor.data(), " group='Mesh View' help='mesh face color' ");

    TwDefine(" MeshViewer/'Mesh View' opened=false ");

    TwAddVarRW(bar, "Rotation", TW_TYPE_QUAT4F, myRotation, " group='Modeling' open help='can be used to specify the rotation for current handle' ");
    TwAddButton(bar, "Bonus bar", bonus, NULL, "");//bonus
    
}

int main(int argc, char *argv[])
{
    if (initGL(argc, argv)) {
        fprintf(stderr, "!Failed to initialize OpenGL!Exit...");
        exit(-1);
    }

    MyMesh::buildShaders();

    std::vector<float> x;
    std::vector<int> f;

    const char* meshpath = argc > 1 ? argv[1] : "balls.obj";
    readObj(meshpath, x, f);

    meshX = Eigen::Map<MatX3f>(x.data(), x.size() / 3, 3);
    meshF = Eigen::Map<MatX3i>(f.data(), f.size() / 3, 3);


    M.upload(x.data(), x.size() / 3, f.data(), f.size() / 3, nullptr);

    //////////////////////////////////////////////////////////////////////////
    TwInit(TW_OPENGL_CORE, NULL);
    //Send 'glutGetModifers' function pointer to AntTweakBar;
    //required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);
    glutSpecialFunc([](int key, int x, int y) { TwEventSpecialGLUT(key, x, y); glutPostRedisplay(); }); // important for special keys like UP/DOWN/LEFT/RIGHT ...
    TwCopyStdStringToClientFunc([](std::string& dst, const std::string& src) {dst = src; });

    createTweakbar();

    //////////////////////////////////////////////////////////////////////////
    atexit([] { TwDeleteAllBars();  TwTerminate(); }); 

    glutTimerFunc(1, [](int) {
        deform_preprocess();
    }, 
        0);


    //////////////////////////////////////////////////////////////////////////
    glutMainLoop();

    return 0;
}