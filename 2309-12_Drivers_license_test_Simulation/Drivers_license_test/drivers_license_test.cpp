#ifdef __LINUX//����������
#include "GL/freeglut.h"
#else//����������
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <windows.h>
#endif
//����
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simuwork.h"

//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
#include <iostream>
using namespace std;

#define PI 3.141592

int simuwork = 0; //�ùķ��̼� ����(?) ���� ���� Ȯ�κ���
int safetybelt = 0, sidebrake = 0, leftlight = 0, rightlight = 0, emerlight = 0; // ������Ʈ, ���̵�극��ũ, �������õ���, ��, ���� ����
int fnddat; // FND(7segment) ������ ����

//�̰� �Ű� �� �ᵵ ��
const int Wid=1024, Hei=600;
int Width = 1024, Height = 600;
int mouse_x = 0;//���������� Ŭ���� ��ġ
int mouse_y = 0;
int rotationX = 0, rotationY = 0;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

//�̰� ���� �ڵ��� ����
float dspeed, speed;
float dxcar = 0, dycar = 0, xcar = -60, ycar = 50;//�ڵ��� �̵�/��ġ
int rcar=180;//�����̼�
bool acceration = false;

void perspective(GLdouble fovy, GLdouble zfar);
void changeSize(int w, int h);
bool isRectangleOnLines();

char pixel_data[Wid * Hei * 3];


void ScreenCapture(const char* strFilePath)
{
    //��Ʈ�� ���� ó���� ���� ��� ����ü
    BITMAPFILEHEADER	BMFH;
    BITMAPINFOHEADER	BMIH;

    int nWidth = 0;
    int nHeight = 0;
    unsigned long dwQuadrupleWidth = 0;		//LJH �߰�, ���� ����� 4�� ����� �ƴ϶�� 4�� ����� ���� ����

    GLbyte* pPixelData = NULL;				//front buffer�� �ȼ� ������ ��� ���� ���� ������ ������

    nWidth = 1024;		//(���� ���)������������ ��� �ػ� �����̹Ƿ� �� ���� �Է�
    nHeight = 600;

    //4�� ������� �ƴ��� Ȯ���ؼ� 4�� ����� �ƴ϶�� 4�� ����� �����ش�.
    dwQuadrupleWidth = (nWidth % 4) ? ((nWidth)+(4 - (nWidth % 4))) : (nWidth);

    //��Ʈ�� ���� ��� ó��
    BMFH.bfType = 0x4D42;		//B(42)�� M(4D)�� �ش��ϴ� ASCII ���� �־��ش�.
    //����Ʈ ������ ��ü���� ũ��
    BMFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwQuadrupleWidth * 3 * nHeight);
    //���� ������ ��ġ������ �Ÿ�
    BMFH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //��Ʈ�� ���� ��� ó��
    BMIH.biSize = sizeof(BITMAPINFOHEADER);		//�� ����ü�� ũ��
    BMIH.biWidth = nWidth;							//�ȼ� ������ ������ ��
    BMIH.biHeight = nHeight;							//������ ����
    BMIH.biPlanes = 1;								//��Ʈ �÷��� ��(�׻� 1)
    BMIH.biBitCount = 24;								//�ȼ��� ��Ʈ��(�÷�, ��� ����)
    BMIH.biCompression = 0;							//���� ����
    BMIH.biSizeImage = dwQuadrupleWidth * 3 * nHeight;	//������ ũ��
    BMIH.biXPelsPerMeter = 0;								//���� �ػ�
    BMIH.biYPelsPerMeter = 0;								//���� �ػ�
    BMIH.biClrUsed = 0;								//���� ��� �����
    BMIH.biClrImportant = 0;								//�߿��� ���� �ε���

    pPixelData = new GLbyte[dwQuadrupleWidth * 3 * nHeight];	//LJH ����

    //����Ʈ ���۷� ���� �ȼ� �������� ���´�.
    glReadPixels(
        0, 0,					//ĸó�� ������ ������� ��ǥ
        nWidth, nHeight,		//ĸó�� ������ ũ��
        GL_BGR_EXT,					//ĸó�� �̹����� �ȼ� ����
        GL_UNSIGNED_BYTE,		//ĸó�� �̹����� ������ ����
        pPixelData				//ĸó�� �̹����� ������ ��Ƶ� ���� ������
    );

    {//���� �κ�
        FILE* outFile = fopen(strFilePath, "wb");
        if (outFile == NULL)
        {
            //���� ó��
            printf("����");
            //fclose( outFile );
        }

        fwrite(&BMFH, sizeof(char), sizeof(BITMAPFILEHEADER), outFile);			//���� ��� ����
        fwrite(&BMIH, sizeof(char), sizeof(BITMAPINFOHEADER), outFile);			//���� ��� ����
        fwrite(pPixelData, sizeof(unsigned char), BMIH.biSizeImage, outFile);	//glReadPixels�� ���� ������ ����

        fclose(outFile);	//���� �ݱ�
    }

    if (pPixelData != NULL) {
        delete[] pPixelData;
    }
}

void perspective(GLdouble fovy = 75.0, GLdouble zfar = 500.0) {
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (double)Width / (double)Height, 1.0, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void changeSize(int w, int h) {
    if (h == 0)  h = 1;
    Width = w;
    Height = h;
    //float ratio = w * 1.0 / h;
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glViewport(0, 0, w, h);
    //gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    //glMatrixMode(GL_MODELVIEW);
    perspective();
}
void idle() {
    //acceration = false;
    if (isRectangleOnLines()) {
        //cout << "-5" << endl;
    }
    else {
        //cout << "0" << endl;
    }
}
void Timer(int val) {
    if(acceration == true)
    {
        if (speed >= 0)speed -= 0.001f;
        else speed = 0;
        dspeed += speed;
    }
    idle();
    glutPostRedisplay();
    glutTimerFunc(1, Timer, 0);
}
int squareVertices[4][2] = {
    {xcar - 5, ycar - 2},
    {xcar + 5, ycar - 2},
    {xcar + 5, ycar + 2},
    {xcar - 5, ycar + 2}
};

// �Ͼ� ��� ��ǥ(������ ������ -5���)
int whitepath1[][4] = {
    {-97, -61, -97, 55},
    {-97, 55 , 91, 55},
    {91, 55, 91, -61},
    {91, -61,-97, -61},
    {-76, 9 , -76, 36},
    {-76, 36, -9, 36},
    {-9, 36, -9, 9},
    {-76, 9,-9, 9},
    {9, 9, 9, 36} ,
    {9, 36, 73, 36},
    {73, 36, 73, 9},
    {9, 9 , 73, 9},
    {9, -43, 9, -9},
    {9, -9, 73, -9},
    {73, -9, 73, -43},
    {9, -43, 73, -43},
    {-76, -43, -76, -9},
    {-76, -9, -9, -9},
    {-9, -9, -9, -43},
    {-76, -43, -9, -43}
};
//��Ȳ ��� ��ǥ(������ ������ -5���)
int lines[][4] = {
    {-87, 9, -87, 45},   // 1. {-87, 9}�� {-87, 45}�� ���� ��
    {-87, 45, 82, 45},   // 2. {-87, 45}�� {82, 45}�� ���� ��
    {82, 45, 82, 9},    // 3. {82, 45}�� {82, 9}�� ���� ��
    {0, 9, 0, 36},      // 4. {0, 9}�� {0, 36}�� ���� ��
    {-17, 0, -87, 0},   // 5. {0, 9}�� {0, 36}�� ���� ��
    {-87, 0, -87, -52}, // 6. {-87, 0}�� {-87, -52}�� ���� ��
    {-87, -52, 82, -52},// 7. {-87, -52}�� {82, -52}�� ���� ��
    {82, -52, 82, -9},  // 8. {82, -52}�� {82, -9}�� ���� ��
    {0, -52, 0, -12},   // 9. {0, -52}�� {0, -12}�� ���� ��
    {7, 0, 67, 0}       // 10. {7, 0}�� {67, 0}�� ���� ��
};
int rqLines1[][4] = {
    {-97, 26, -73, 26},   // 1�� ��
    {-19, 0, -19, 9},     // 2�� ��
    {67, 0, 67, 9},       // 3�� ��
    {9, 0, 9, -9},        // 4�� ��
    {-9, -17, 0, -17},     // 5�� ��
    {73, 9, 91, 9}
};

////////////////////////////////////////////////////////////////
bool isPointOnLine(float x, float y, float x1, float y1, float x2, float y2) {
    // ���� (x1, y1) - (x2, y2) ���� �� (x, y)�� �ִ��� Ȯ��
    float minX = min(x1, x2);
    float maxX = max(x1, x2);
    float minY = min(y1, y2);
    float maxY = max(y1, y2);

    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
}
bool isPointOnPath(int x, int y, int x1, int y1, int x2, int y2) {
    int minX = min(x1, x2);
    int maxX = max(x1, x2);
    int minY = min(y1, y2);
    int maxY = max(y1, y2);

    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
}
///////////////////////////////////////////////////////
bool isRectangleOnLines() {
    // �簢���� �� �������� �� ���� �ִ��� Ȯ��
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (isPointOnPath(squareVertices[j][0], squareVertices[j][1],
                whitepath1[i][0], whitepath1[i][1], whitepath1[i][2], whitepath1[i][3])) {
                return true; // �簢���� �������� �� ���� �ִٸ� true ��ȯ
            }
        }
    }
    for (int k = 0; k < 10; ++k) {
        for (int t = 0; t < 4; ++t) {
            if (isPointOnLine(squareVertices[t][0], squareVertices[t][1],
                lines[k][0], lines[k][1], lines[k][2], lines[k][3])) {
                return true; // �簢���� �������� �� ���� �ִٸ� true ��ȯ
            }
        }
    }
    return false; // �簢���� �������� �� ���� ���ٸ� false ��ȯ
}
void updateSquareVertices() {
    // Update squareVertices based on the current square position
    squareVertices[0][0] = xcar - 5;
    squareVertices[0][1] = ycar - 2;
    squareVertices[1][0] = xcar + 5;
    squareVertices[1][1] = ycar - 2;
    squareVertices[2][0] = xcar + 5;
    squareVertices[2][1] = ycar + 2;
    squareVertices[3][0] = xcar - 5;
    squareVertices[3][1] = ycar + 2;
}// ���� (x1, y1) - (x2, y2) ���� �� (x, y)�� �ִ��� Ȯ��

void display_lines(int line_width = 5.0) {
    //
    glColor3f(1.0, 1.0, 1.0);  // �������
    glLineWidth(line_width);         // �β� 2 ����
    glBegin(GL_LINES);
    for (int i = 0; i < 20; ++i) {
        glVertex3f(whitepath1[i][0], 0.1, whitepath1[i][1]);
        glVertex3f(whitepath1[i][2], 0.1, whitepath1[i][3]);
    }
    for (int i = 0; i < 6; ++i) {
        glVertex3f(rqLines1[i][0], 0.1, rqLines1[i][1]);
        glVertex3f(rqLines1[i][2], 0.1, rqLines1[i][3]);
    }
    glEnd();
    //
    glColor3f(1.0, 1.0, 0.0);  // ����� ����
    glLineWidth(line_width);         // �β� 2 ����

    glBegin(GL_LINES);
    for (int i = 0; i < 10; ++i) {
        glVertex3f(lines[i][0], 0.1, lines[i][1]);
        glVertex3f(lines[i][2], 0.1, lines[i][3]);
    }
    glEnd();

    //
    glColor3f(0.0, 1.0, 0.0);  // �ʷϻ� ����
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i) {
        glVertex3f(squareVertices[i][0], 0.1, squareVertices[i][1]);
    }
    glEnd();
    //

    glFlush();
}
void drawingCar() {//�ڵ��� �𵨸�, �̵�, ȸ��
    // �� ���� �κ�
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(8, 3, -4);  // B
    glVertex3i(3, 3, -4);  // C
    glVertex3i(3, 3, 4);   // D
    glEnd();

    // �� ���ۺκ� 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(8, 3, -4);  // B
    glVertex3i(8, 0, -4);  // F
    glVertex3i(8, 0, 4);   // E
    glEnd();

    // �� ���� ���� �� �κ� 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(6, 3, 4);   // G
    glVertex3i(6, 0, 4);   // H
    glVertex3i(8, 0, 4);   // E
    glEnd();

    // �� ���� ���� �� �κ� [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, -4);   // A
    glVertex3i(6, 3, -4);   // G
    glVertex3i(6, 0, -4);   // H
    glVertex3i(8, 0, -4);   // E
    glEnd();


    // �� ���� ���� �� �κ� 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 3, 4);   // G
    glVertex3i(3, 3, 4);   // D
    glVertex3i(3, 1, 4);  // J
    glVertex3i(6, 1, 4);   // M
    glEnd();

    // �� ���� ���� ��� �κ� 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(3, 3, 4);   // D
    glVertex3i(3, 0, 4);   // I
    glVertex3i(-3, 0, 4);   // N 
    glVertex3i(-3, 3, 4);   // O
    glEnd();

    // �� ���� ���� �� �κ� [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 3, -4);   // G
    glVertex3i(3, 3, -4);   // D
    glVertex3i(3, 1, -4);  // J
    glVertex3i(6, 1, -4);   // M
    glEnd();

    // �� ���� ���� ��� �κ� [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(3, 3, -4);   // D
    glVertex3i(3, 0, -4);   // I
    glVertex3i(-3, 0, -4);   // N 
    glVertex3i(-3, 3, -4);   // O
    glEnd();

    //�� �� ���� ������1
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 6, 3);   // V
    glVertex3i(3, 3, 3);   // T
    glVertex3i(3, 3, 4);   // D
    glEnd();
    //�� �� ���� ������2
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 5, -4);   // X
    glVertex3i(3, 5, 4);   // Y
    glEnd();
    //�� �� ���� ������3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, -3);   // W
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 3, -4);   // C
    glVertex3i(3, 3, -3);   // U
    glEnd();

    //�� �� ����
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(3, 5, 3);   // R
    glVertex3i(3, 5, -3);   // S
    glVertex3i(3, 3, -3);   // U
    glVertex3i(3, 3, 3);   // T
    glEnd();

    //�� �� ���� ������1
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // P(X - 8)
    glVertex3i(-6, 6, 3);   // V
    glVertex3i(-6, 3, 3);   // T
    glVertex3i(-6, 3, 4);   // D
    glEnd();
    //�� �� ���� ������2
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // P
    glVertex3i(-6, 6, -4);   // Q
    glVertex3i(-6, 5, -4);   // X
    glVertex3i(-6, 5, 4);   // Y
    glEnd();
    //�� �� ���� ������3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -3);   // W
    glVertex3i(-6, 6, -4);   // Q
    glVertex3i(-6, 3, -4);   // C
    glVertex3i(-6, 3, -3);   // U
    glEnd();

    //�� �� ����
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 5, 3);   // R
    glVertex3i(-6, 5, -3);   // S
    glVertex3i(-6, 3, -3);   // U
    glVertex3i(-6, 3, 3);   // T
    glEnd();

    //�� �� ���� ������1 
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(-5, 6, 4);   // B1
    glVertex3i(-5, 3, 4);   // C1
    glVertex3i(-6, 3, 4);   // Z
    glEnd();
    //�� �� ���� ������2 
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 5, 4);   // Y
    glVertex3i(-6, 5, 4);   // H1
    glEnd();
    //�� �� ���� ������3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(2, 6, 4);   // G1
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 3, 4);   // D
    glVertex3i(2, 3, 4);   // F1
    glEnd();
    //�� �� ����
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-5, 5, 4);   // D1
    glVertex3i(2, 5, 4);   //E1
    glVertex3i(2, 3, 4);   // F1
    glVertex3i(-5, 3, 4);   // C1
    glEnd();

    // �� �� ���� ������1 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -4);   // A1
    glVertex3i(-5, 6, -4);   // B1
    glVertex3i(-5, 3, -4);   // C1
    glVertex3i(-6, 3, -4);   // Z
    glEnd();
    //�� �� ���� ������2 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -4);   // A1
    glVertex3i(3, 6, -4);   // P
    glVertex3i(3, 5, -4);   // Y
    glVertex3i(-6, 5, -4);   // H1
    glEnd();
    //�� �� ���� ������3 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(2, 6, -4);   // G1
    glVertex3i(3, 6, -4);   // P
    glVertex3i(3, 3, -4);   // D
    glVertex3i(2, 3, -4);   // F1
    glEnd();
    //�� �� ���� [RIGHT]
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-5, 5, -4);   // D1
    glVertex3i(2, 5, -4);   //E1
    glVertex3i(2, 3, -4);   // F1
    glVertex3i(-5, 3, -4);   // C1
    glEnd();

    //�� �Ѳ�
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 6, 4);   // P
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(-6, 6, -4);   // I1
    glEnd();

    //�ý� ǥ���ϴ� ��(�Ѳ� ����)1
    glColor3f(1.0, 1.0, 0.0);   //Yellow
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, 2);   // M1
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-1, 6, 2);   // K1
    glVertex3i(-3, 6, 2);   // J1
    glEnd();
    //�ý� ǥ���ϴ� ��(�Ѳ� ����)2
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-1, 6, -2);   // L1
    glVertex3i(-1, 6, 2);   // K1
    glEnd();
    //�ý� ǥ���ϴ� ��(�Ѳ� ����)3 O1 P1 Q1 L1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-3, 6, -2);   // Q1
    glVertex3i(-1, 6, -2);   // L1
    glEnd();
    //�ý� ǥ���ϴ� ��(�Ѳ� ����)4 P1 M1 J1 Q1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-3, 7, 2);   // M1
    glVertex3i(-3, 6, 2);   // J1
    glVertex3i(-3, 6, -2);   // Q1
    glEnd();
    //�ý� ǥ���ϴ� ��(�Ѳ� ����)5 [�Ѳ�] P1 O1 N1 M1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-3, 7, 2);   // M1
    glEnd();

    //�ĸ� ���� �� �κ� T1 O R1 S1
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 3, 4);   // T1
    glVertex3i(-3, 3, 4);   // O
    glVertex3i(-3, 1, 4);   // R1
    glVertex3i(-6, 1, 4);   // S1
    glEnd();

    //�ĸ� ���� �� �κ� [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-6, 3, 4);   // T1
    glVertex3i(-6, 0, 4);   // U1
    glVertex3i(-9, 0, 4);   // L
    glEnd();

    //�ĸ� ���� �� �κ� [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 3, -4);   // T1
    glVertex3i(-3, 3, -4);   // O
    glVertex3i(-3, 1, -4);   // R1
    glVertex3i(-6, 1, -4);   // S1
    glEnd();

    //�ĸ� ���� �� �κ� K T1 U1 L
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, -4);   // K
    glVertex3i(-6, 3, -4);   // T1
    glVertex3i(-6, 0, -4);   // U1
    glVertex3i(-9, 0, -4);   // L
    glEnd();

    //�� Ʈ��ũ
    glColor3f(1.0, 1.0, 1.0);   //RED
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-9, 3, -4);   // K(-Z)
    glVertex3i(-6, 3, -4);   // Z(-Z)
    glVertex3i(-6, 3, 4);   // Z

    glEnd();

    //�� �ĸ� ���� KL,
    glColor3f(1.0, 1.0, 1.0);   // PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-9, 0, 4);   // L
    glVertex3i(-9, 0, -4);   // L(-Z)
    glVertex3i(-9, 3, -4);   // K(-Z)
    glEnd();

    //�� ������1 [LEFT] A2 B2 C2 D2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(2, 2, 5);   // B2
    glVertex3i(0, 2, 5);   // C2
    glVertex3i(0, 3, 5);   // D2
    glEnd();
    //�� ������2 [LEFT] A2 D2 E2 F1
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(0, 3, 5);   // D2
    glVertex3i(0, 3, 4);   // E2
    glVertex3i(2, 3, 4);   // F1
    glEnd();
    //�� ������2 [LEFT] D2 E2 G2 C2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(0, 3, 5);   // D2
    glVertex3i(0, 3, 4);   // E2
    glVertex3i(0, 2, 4);   // G2
    glVertex3i(0, 2, 5);   // C2
    glEnd();
    //�� ������2 [LEFT] A2 F1 F2 B2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(2, 3, 4);   // F1
    glVertex3i(2, 2, 4);   // F2
    glVertex3i(2, 2, 5);   // B2
    glEnd();

    //�� ������1 [RIGHT] 
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(2, 2, -5);   // B2
    glVertex3i(0, 2, -5);   // C2
    glVertex3i(0, 3, -5);   // D2
    glEnd();
    //�� ������2 [RIGHT] A2 D2 E2 F1
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(0, 3, -5);   // D2
    glVertex3i(0, 3, -4);   // E2
    glVertex3i(2, 3, -4);   // F1
    glEnd();
    //�� ������2 [RIGHT] D2 E2 G2 C2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(0, 3, -5);   // D2
    glVertex3i(0, 3, -4);   // E2
    glVertex3i(0, 2, -4);   // G2
    glVertex3i(0, 2, -5);   // C2
    glEnd();
    //�� ������2 [RIGHT] A2 F1 F2 B2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(2, 3, -4);   // F1
    glVertex3i(2, 2, -4);   // F2
    glVertex3i(2, 2, -5);   // B2
    glEnd();

    //�� �ٴ�1
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 0, -4);   // A3
    glVertex3i(8, 0, -4);   // F
    glVertex3i(8, 0, 4);   // E
    glVertex3i(6, 0, 4);   // H
    glEnd();
    //�� �ٴ�2
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(-3, 0, -4);   // N(Z-8)
    glVertex3i(3, 0, -4);   // I(Z-8)
    glVertex3i(3, 0, 4);   // I
    glVertex3i(-3, 0, 4);   // N
    glEnd();
    //�� �ٴ�3
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(-9, 0, 4);   // L
    glVertex3i(-9, 0, -4);   // L(Z-8)
    glVertex3i(-6, 0, -4);   // U1(Z-8)
    glVertex3i(-6, 0, 4);   // U1
    glEnd();
}
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': case 'Q': exit(0); break;//����
        //case 'r': case 'R': glClearColor(1.0, 0.0, 0.0, 1.0); break;

    case 'c'://ĸó
        //screen_dump();
        ScreenCapture("strFilePath.bmp");
        Sleep(100);
        break;
    case 'm': simuwork = CRS_MAIN; break;//���θ޴�
    }
    if (simuwork != CRS_MAIN) {
        switch (key) {
        case 'w'://����
            /*acceration = true;
            if (speed < 5)
                speed += 0.1f;
            else speed = 0;
            dspeed += speed;*/
            speed = 1;
            dxcar = speed * cos((180 - rcar) * PI / 180.0); xcar += dxcar;
            dycar = speed * sin((180 - rcar) * PI / 180.0); ycar += dycar;
            cout << "x=" << xcar << ", y=" << ycar << endl;
            break;
        case 's'://����
            speed = 1;
            dxcar = speed * cos((180 - rcar) * PI / 180.0); xcar -= dxcar;
            dycar = speed * sin((180 - rcar) * PI / 180.0); ycar -= dycar;
            break;
        case 'a': rcar += 3; printf("r=%d\n", rcar); break;//��ȸ��
        case 'd': rcar -= 3; printf("r=%d\n", rcar); break;//��ȸ��
        }
    }
    if (rcar >= 360 || rcar <= -360) rcar = 0;
    glutPostRedisplay();
}

void drawingCar_ok(float car_size) {//�׽�Ʈ�� �ڵ��� �𵨸�, �̵�, ȸ��
    glPushMatrix();
    glTranslatef(xcar, 1, ycar);//�ڵ��� �̵�
    glRotatef(rcar, 0.0f, 1.0f, 0.0f);//�ڵ��� ȸ��
    //glRotatef(180, 0.0f, 1.0f, 0.0f);//�ڵ��� ȸ��
    //glBegin(GL_LINES);
    //    glColor3f(0.0, 0.0, 1.0);
    //    glVertex3f(0.0, 0.0, 0.0);  glVertex3f(-10.0, 0.0, 0.0); /* Z axis  */
    //glEnd();
    glRotatef(180, 0.0f, 1.0f, 0.0f);//�ڵ��� ȸ��
    glScalef(car_size, car_size, car_size);
    drawingCar();
    //glutWireCube(5.0);
    glPopMatrix();
}

void drawScene(float car_size = 0.4, int line_width = 5) {//�׸� ��ü��
    glPushMatrix();
        display_lines(line_width);
    glPopMatrix();
// ��� �׸���
    glPushMatrix();
        glColor3f(0.5, 0.5, 0.5);
        glTranslatef(0.0, -0.5, 0.0);
        glScalef(200.0, 1.0, 130.0);
        glutSolidCube(1.0);
        
        glPushMatrix();
            glColor3f(0.1, 0.1, 0.1);
            glLineWidth(2);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();

    drawingCar_ok(car_size);
    //drawingCar();

    //glColor3f(1.0, 0.0, 0.6);
    //glutWireTorus(1, 3, 10, 100);
}
void renderStrokeString(float x, float y, float z, float R, float G, float B, float Scale, void* font, char* string)
{
    char* c;
    glPushMatrix();
    glColor3f(R, G, B);
    glTranslatef(x, y, z);
    glScalef(Scale, Scale, Scale);
    for (c = string; *c != '\0'; c++) {
        glutStrokeCharacter(font, *c);
    }
    glPopMatrix();
}

char start[] = "START";
char info[] = "INFO";
char quit[] = "QUIT";
void main_menu() {
    glClearColor(.9f, .9f, .9f, 1.0f);//����

    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    glLineWidth(5);
    renderStrokeString(-20,   0-5, 1,     1, 1, 1, 0.1, GLUT_STROKE_ROMAN, start);
    renderStrokeString(-15, -30-5, 1,     1, 1, 1, 0.1, GLUT_STROKE_ROMAN, info);
    renderStrokeString(-15, -60-5, 1,     1, 1, 1, 0.1, GLUT_STROKE_ROMAN, quit);
    glPopMatrix(); 

    glColor3f(0.0, 0.3, 0.5);

    //glBegin(GL_QUADS);
    //glVertex3i(304, 0, 276);
    //glVertex3i(718, 0, 276);
    //glVertex3i(718, 0, 357);
    //glVertex3i(304, 0, 357);
    //glEnd();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glScalef(100.0, 1.0, 20.0);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.3, 0.5);
    glTranslatef(0.0, 0.0, -30.0);
    glScalef(100.0, 1.0, 20.0);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.3, 0.5);
    glTranslatef(0.0, 0.0, -60.0);
    glScalef(100.0, 1.0, 20.0);
    glutSolidCube(1.0);
    glPopMatrix();
}

void disp() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    if (simuwork == CRS_MAIN)
    {
        perspective();
        glClearColor(.9f, .9f, .9f, 1.0f);//����

        glViewport(0, 0, Width, Height);

        glPushMatrix();
        gluLookAt(0.0, -100.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);//ī�޶� ��ġ/�ٶ󺸴� ���� ��ġ/ī�޶� �����

        glRotatef(rotationX, 1.0, 0.0, 0.0);
        glRotatef(rotationY, 0.0, 1.0, 0.0);
        glMultMatrixf(view_rotate);
        main_menu();



        glPopMatrix();
    }
    else//�ù� ������
    {//-------------------------��ü ȭ��--------------------------------------
        perspective();
        glClearColor(.9f, .9f, .9f, 1.0f);//����
        glLoadIdentity();

        glViewport(0, 0, Width, Height);

        glPushMatrix();
        gluLookAt(xcar - 10 * cos((180 - rcar) * PI / 180.0), 20, ycar - 10 * sin((180 - rcar) * PI / 180.0),
            xcar + 10 * cos((180 - rcar) * PI / 180.0), 0.1, ycar + 10 * sin((180 - rcar) * PI / 180.0),
            0.0, 1.0, 0.0);//ī�޶� ��ġ/�ٶ󺸴� ���� ��ġ/ī�޶� �����

        glPushMatrix();

        glRotatef(rotationX, 1.0, 0.0, 0.0);
        glRotatef(rotationY, 0.0, 1.0, 0.0);
        glMultMatrixf(view_rotate);
        drawScene();

        glPopMatrix();
        glPopMatrix();

        //-----------------------������ �� ȭ��----------------------------
        glViewport(Width * 3 / 5, Height * 3 / 5, Width * 2 / 5, Height * 2 / 5);
        //glViewport(0, Height / 2, Width/3, Height/2);
        glPushMatrix();
        gluLookAt(0.0, 90.0, 5.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);
        glDisable(GLUT_DEPTH);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawScene(1.5, 2);
        glEnable(GLUT_DEPTH);
        glPopMatrix();
    }
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        cout << x << ", " << y << endl;
        mouse_x = x;
        mouse_y = y;
        if (mouse_x > 317 && mouse_y > 261 && mouse_x < 707 && mouse_y < 337 && simuwork == CRS_MAIN) {
            simuwork = CRS_START;
            xcar = -60, ycar = 50;//�ڵ��� �̵�/��ġ �ʱ�ȭ
            rcar = 180;//�����̼� �ʱ�ȭ
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) //���콺 ��ư���� �� �� ��
    {
    }
    glutPostRedisplay();
}

void motion(int x, int y)
{
    int dx = x - mouse_x;
    int dy = y - mouse_y;

    rotationX += dy;
    rotationY += dx;

    float view_rotate_init[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    for (int i = 0; i < 16; i++) {
        view_rotate[i] = view_rotate_init[i];
    }
    mouse_x = x;
    mouse_y = y;

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutInitDisplayMode(GLUT_DOUBLE);
    /*float v[] = {
      lightDiffuse[0],  lightDiffuse[1],
      lightDiffuse[2],  lightDiffuse[3] };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, v);*/
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Embedded system");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(disp);
    glutReshapeFunc(changeSize);

    glutTimerFunc(1, Timer, 0);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    //glutMotionFunc(motion);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}