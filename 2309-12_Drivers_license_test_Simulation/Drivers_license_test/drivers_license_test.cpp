#ifdef __LINUX//리눅스전용
#include "GL/freeglut.h"
#else//윈도우전용
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <windows.h>
#endif
//공통
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simuwork.h"

//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
#include <iostream>
using namespace std;

#define PI 3.141592

int simuwork = 0; //시뮬레이션 게임(?) 동작 여부 확인변수
int safetybelt = 0, sidebrake = 0, leftlight = 0, rightlight = 0, emerlight = 0; // 안전벨트, 사이드브레이크, 방향지시등좌, 우, 비상등 변수
int fnddat; // FND(7segment) 데이터 변수

//이거 신경 안 써도 됨
const int Wid=1024, Hei=600;
int Width = 1024, Height = 600;
int mouse_x = 0;//마지막으로 클릭한 위치
int mouse_y = 0;
int rotationX = 0, rotationY = 0;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

//이게 전부 자동차 관련
float dspeed, speed;
float dxcar = 0, dycar = 0, xcar = -60, ycar = 50;//자동차 이동/위치
int rcar=180;//로테이션
bool acceration = false;

void perspective(GLdouble fovy, GLdouble zfar);
void changeSize(int w, int h);
bool isRectangleOnLines();

char pixel_data[Wid * Hei * 3];


void ScreenCapture(const char* strFilePath)
{
    //비트맵 파일 처리를 위한 헤더 구조체
    BITMAPFILEHEADER	BMFH;
    BITMAPINFOHEADER	BMIH;

    int nWidth = 0;
    int nHeight = 0;
    unsigned long dwQuadrupleWidth = 0;		//LJH 추가, 가로 사이즈가 4의 배수가 아니라면 4의 배수로 만들어서 저장

    GLbyte* pPixelData = NULL;				//front buffer의 픽셀 값들을 얻어 오기 위한 버퍼의 포인터

    nWidth = 1024;		//(나의 경우)리눅스에서의 경우 해상도 고정이므로 그 값을 입력
    nHeight = 600;

    //4의 배수인지 아닌지 확인해서 4의 배수가 아니라면 4의 배수로 맞춰준다.
    dwQuadrupleWidth = (nWidth % 4) ? ((nWidth)+(4 - (nWidth % 4))) : (nWidth);

    //비트맵 파일 헤더 처리
    BMFH.bfType = 0x4D42;		//B(42)와 M(4D)에 해당하는 ASCII 값을 넣어준다.
    //바이트 단위로 전체파일 크기
    BMFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwQuadrupleWidth * 3 * nHeight);
    //영상 데이터 위치까지의 거리
    BMFH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //비트맵 인포 헤더 처리
    BMIH.biSize = sizeof(BITMAPINFOHEADER);		//이 구조체의 크기
    BMIH.biWidth = nWidth;							//픽셀 단위로 영상의 폭
    BMIH.biHeight = nHeight;							//영상의 높이
    BMIH.biPlanes = 1;								//비트 플레인 수(항상 1)
    BMIH.biBitCount = 24;								//픽셀당 비트수(컬러, 흑백 구별)
    BMIH.biCompression = 0;							//압축 유무
    BMIH.biSizeImage = dwQuadrupleWidth * 3 * nHeight;	//영상의 크기
    BMIH.biXPelsPerMeter = 0;								//가로 해상도
    BMIH.biYPelsPerMeter = 0;								//세로 해상도
    BMIH.biClrUsed = 0;								//실제 사용 색상수
    BMIH.biClrImportant = 0;								//중요한 색상 인덱스

    pPixelData = new GLbyte[dwQuadrupleWidth * 3 * nHeight];	//LJH 수정

    //프런트 버퍼로 부터 픽셀 정보들을 얻어온다.
    glReadPixels(
        0, 0,					//캡처할 영역의 좌측상단 좌표
        nWidth, nHeight,		//캡처할 영역의 크기
        GL_BGR_EXT,					//캡처한 이미지의 픽셀 포맷
        GL_UNSIGNED_BYTE,		//캡처한 이미지의 데이터 포맷
        pPixelData				//캡처한 이미지의 정보를 담아둘 버퍼 포인터
    );

    {//저장 부분
        FILE* outFile = fopen(strFilePath, "wb");
        if (outFile == NULL)
        {
            //에러 처리
            printf("에러");
            //fclose( outFile );
        }

        fwrite(&BMFH, sizeof(char), sizeof(BITMAPFILEHEADER), outFile);			//파일 헤더 쓰기
        fwrite(&BMIH, sizeof(char), sizeof(BITMAPINFOHEADER), outFile);			//인포 헤더 쓰기
        fwrite(pPixelData, sizeof(unsigned char), BMIH.biSizeImage, outFile);	//glReadPixels로 읽은 데이터 쓰기

        fclose(outFile);	//파일 닫기
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

// 하얀 경로 좌표(바퀴가 닿으면 -5출력)
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
//주황 경로 좌표(바퀴가 닿으면 -5출력)
int lines[][4] = {
    {-87, 9, -87, 45},   // 1. {-87, 9}와 {-87, 45}를 이은 선
    {-87, 45, 82, 45},   // 2. {-87, 45}와 {82, 45}를 이은 선
    {82, 45, 82, 9},    // 3. {82, 45}와 {82, 9}를 이은 선
    {0, 9, 0, 36},      // 4. {0, 9}와 {0, 36}를 이은 선
    {-17, 0, -87, 0},   // 5. {0, 9}와 {0, 36}를 이은 선
    {-87, 0, -87, -52}, // 6. {-87, 0}와 {-87, -52}를 이은 선
    {-87, -52, 82, -52},// 7. {-87, -52}와 {82, -52}를 이은 선
    {82, -52, 82, -9},  // 8. {82, -52}와 {82, -9}를 이은 선
    {0, -52, 0, -12},   // 9. {0, -52}와 {0, -12}를 이은 선
    {7, 0, 67, 0}       // 10. {7, 0}와 {67, 0}를 이은 선
};
int rqLines1[][4] = {
    {-97, 26, -73, 26},   // 1번 줄
    {-19, 0, -19, 9},     // 2번 줄
    {67, 0, 67, 9},       // 3번 줄
    {9, 0, 9, -9},        // 4번 줄
    {-9, -17, 0, -17},     // 5번 줄
    {73, 9, 91, 9}
};

////////////////////////////////////////////////////////////////
bool isPointOnLine(float x, float y, float x1, float y1, float x2, float y2) {
    // 선분 (x1, y1) - (x2, y2) 위에 점 (x, y)가 있는지 확인
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
    // 사각형의 네 꼭짓점이 선 위에 있는지 확인
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (isPointOnPath(squareVertices[j][0], squareVertices[j][1],
                whitepath1[i][0], whitepath1[i][1], whitepath1[i][2], whitepath1[i][3])) {
                return true; // 사각형의 꼭짓점이 선 위에 있다면 true 반환
            }
        }
    }
    for (int k = 0; k < 10; ++k) {
        for (int t = 0; t < 4; ++t) {
            if (isPointOnLine(squareVertices[t][0], squareVertices[t][1],
                lines[k][0], lines[k][1], lines[k][2], lines[k][3])) {
                return true; // 사각형의 꼭짓점이 선 위에 있다면 true 반환
            }
        }
    }
    return false; // 사각형의 꼭짓점이 선 위에 없다면 false 반환
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
}// 선분 (x1, y1) - (x2, y2) 위에 점 (x, y)가 있는지 확인

void display_lines(int line_width = 5.0) {
    //
    glColor3f(1.0, 1.0, 1.0);  // 흰색설정
    glLineWidth(line_width);         // 두께 2 설정
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
    glColor3f(1.0, 1.0, 0.0);  // 노란색 설정
    glLineWidth(line_width);         // 두께 2 설정

    glBegin(GL_LINES);
    for (int i = 0; i < 10; ++i) {
        glVertex3f(lines[i][0], 0.1, lines[i][1]);
        glVertex3f(lines[i][2], 0.1, lines[i][3]);
    }
    glEnd();

    //
    glColor3f(0.0, 1.0, 0.0);  // 초록색 설정
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i) {
        glVertex3f(squareVertices[i][0], 0.1, squareVertices[i][1]);
    }
    glEnd();
    //

    glFlush();
}
void drawingCar() {//자동차 모델링, 이동, 회전
    // 차 앞위 부분
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(8, 3, -4);  // B
    glVertex3i(3, 3, -4);  // C
    glVertex3i(3, 3, 4);   // D
    glEnd();

    // 차 범퍼부분 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(8, 3, -4);  // B
    glVertex3i(8, 0, -4);  // F
    glVertex3i(8, 0, 4);   // E
    glEnd();

    // 차 바퀴 기준 앞 부분 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, 4);   // A
    glVertex3i(6, 3, 4);   // G
    glVertex3i(6, 0, 4);   // H
    glVertex3i(8, 0, 4);   // E
    glEnd();

    // 차 바퀴 기준 앞 부분 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(8, 3, -4);   // A
    glVertex3i(6, 3, -4);   // G
    glVertex3i(6, 0, -4);   // H
    glVertex3i(8, 0, -4);   // E
    glEnd();


    // 차 바퀴 기준 윗 부분 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 3, 4);   // G
    glVertex3i(3, 3, 4);   // D
    glVertex3i(3, 1, 4);  // J
    glVertex3i(6, 1, 4);   // M
    glEnd();

    // 차 바퀴 기준 가운데 부분 
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(3, 3, 4);   // D
    glVertex3i(3, 0, 4);   // I
    glVertex3i(-3, 0, 4);   // N 
    glVertex3i(-3, 3, 4);   // O
    glEnd();

    // 차 바퀴 기준 윗 부분 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 3, -4);   // G
    glVertex3i(3, 3, -4);   // D
    glVertex3i(3, 1, -4);  // J
    glVertex3i(6, 1, -4);   // M
    glEnd();

    // 차 바퀴 기준 가운데 부분 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(3, 3, -4);   // D
    glVertex3i(3, 0, -4);   // I
    glVertex3i(-3, 0, -4);   // N 
    glVertex3i(-3, 3, -4);   // O
    glEnd();

    //차 앞 유리 프레임1
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 6, 3);   // V
    glVertex3i(3, 3, 3);   // T
    glVertex3i(3, 3, 4);   // D
    glEnd();
    //차 앞 유리 프레임2
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 5, -4);   // X
    glVertex3i(3, 5, 4);   // Y
    glEnd();
    //차 앞 유리 프레임3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, -3);   // W
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 3, -4);   // C
    glVertex3i(3, 3, -3);   // U
    glEnd();

    //차 앞 유리
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(3, 5, 3);   // R
    glVertex3i(3, 5, -3);   // S
    glVertex3i(3, 3, -3);   // U
    glVertex3i(3, 3, 3);   // T
    glEnd();

    //차 뒷 유리 프레임1
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // P(X - 8)
    glVertex3i(-6, 6, 3);   // V
    glVertex3i(-6, 3, 3);   // T
    glVertex3i(-6, 3, 4);   // D
    glEnd();
    //차 뒷 유리 프레임2
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // P
    glVertex3i(-6, 6, -4);   // Q
    glVertex3i(-6, 5, -4);   // X
    glVertex3i(-6, 5, 4);   // Y
    glEnd();
    //차 뒷 유리 프레임3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -3);   // W
    glVertex3i(-6, 6, -4);   // Q
    glVertex3i(-6, 3, -4);   // C
    glVertex3i(-6, 3, -3);   // U
    glEnd();

    //차 뒷 유리
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 5, 3);   // R
    glVertex3i(-6, 5, -3);   // S
    glVertex3i(-6, 3, -3);   // U
    glVertex3i(-6, 3, 3);   // T
    glEnd();

    //차 옆 유리 프레임1 
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(-5, 6, 4);   // B1
    glVertex3i(-5, 3, 4);   // C1
    glVertex3i(-6, 3, 4);   // Z
    glEnd();
    //차 옆 유리 프레임2 
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 5, 4);   // Y
    glVertex3i(-6, 5, 4);   // H1
    glEnd();
    //차 옆 유리 프레임3
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(2, 6, 4);   // G1
    glVertex3i(3, 6, 4);   // P
    glVertex3i(3, 3, 4);   // D
    glVertex3i(2, 3, 4);   // F1
    glEnd();
    //차 옆 유리
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-5, 5, 4);   // D1
    glVertex3i(2, 5, 4);   //E1
    glVertex3i(2, 3, 4);   // F1
    glVertex3i(-5, 3, 4);   // C1
    glEnd();

    // 차 옆 유리 프레임1 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -4);   // A1
    glVertex3i(-5, 6, -4);   // B1
    glVertex3i(-5, 3, -4);   // C1
    glVertex3i(-6, 3, -4);   // Z
    glEnd();
    //차 옆 유리 프레임2 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(-6, 6, -4);   // A1
    glVertex3i(3, 6, -4);   // P
    glVertex3i(3, 5, -4);   // Y
    glVertex3i(-6, 5, -4);   // H1
    glEnd();
    //차 옆 유리 프레임3 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(2, 6, -4);   // G1
    glVertex3i(3, 6, -4);   // P
    glVertex3i(3, 3, -4);   // D
    glVertex3i(2, 3, -4);   // F1
    glEnd();
    //차 옆 유리 [RIGHT]
    glColor3f(0.0, 0.0, 0.0);   //BLACK
    glBegin(GL_POLYGON);
    glVertex3i(-5, 5, -4);   // D1
    glVertex3i(2, 5, -4);   //E1
    glVertex3i(2, 3, -4);   // F1
    glVertex3i(-5, 3, -4);   // C1
    glEnd();

    //차 뚜껑
    glColor3f(1.0, 1.0, 1.0);   //WHITE
    glBegin(GL_POLYGON);
    glVertex3i(3, 6, -4);   // Q
    glVertex3i(3, 6, 4);   // P
    glVertex3i(-6, 6, 4);   // A1
    glVertex3i(-6, 6, -4);   // I1
    glEnd();

    //택시 표시하는 거(뚜껑 위에)1
    glColor3f(1.0, 1.0, 0.0);   //Yellow
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, 2);   // M1
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-1, 6, 2);   // K1
    glVertex3i(-3, 6, 2);   // J1
    glEnd();
    //택시 표시하는 거(뚜껑 위에)2
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-1, 6, -2);   // L1
    glVertex3i(-1, 6, 2);   // K1
    glEnd();
    //택시 표시하는 거(뚜껑 위에)3 O1 P1 Q1 L1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-3, 6, -2);   // Q1
    glVertex3i(-1, 6, -2);   // L1
    glEnd();
    //택시 표시하는 거(뚜껑 위에)4 P1 M1 J1 Q1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-3, 7, 2);   // M1
    glVertex3i(-3, 6, 2);   // J1
    glVertex3i(-3, 6, -2);   // Q1
    glEnd();
    //택시 표시하는 거(뚜껑 위에)5 [뚜껑] P1 O1 N1 M1
    glColor3f(1.0, 1.0, 0.0);   //YELLOW
    glBegin(GL_POLYGON);
    glVertex3i(-3, 7, -2);   // P1
    glVertex3i(-1, 7, -2);   // O1
    glVertex3i(-1, 7, 2);   // N1
    glVertex3i(-3, 7, 2);   // M1
    glEnd();

    //후면 바퀴 윗 부분 T1 O R1 S1
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 3, 4);   // T1
    glVertex3i(-3, 3, 4);   // O
    glVertex3i(-3, 1, 4);   // R1
    glVertex3i(-6, 1, 4);   // S1
    glEnd();

    //후면 바퀴 뒷 부분 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-6, 3, 4);   // T1
    glVertex3i(-6, 0, 4);   // U1
    glVertex3i(-9, 0, 4);   // L
    glEnd();

    //후면 바퀴 윗 부분 [RIGHT]
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-6, 3, -4);   // T1
    glVertex3i(-3, 3, -4);   // O
    glVertex3i(-3, 1, -4);   // R1
    glVertex3i(-6, 1, -4);   // S1
    glEnd();

    //후면 바퀴 뒷 부분 K T1 U1 L
    glColor3f(1.0, 1.0, 1.0);   //PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, -4);   // K
    glVertex3i(-6, 3, -4);   // T1
    glVertex3i(-6, 0, -4);   // U1
    glVertex3i(-9, 0, -4);   // L
    glEnd();

    //차 트렁크
    glColor3f(1.0, 1.0, 1.0);   //RED
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-9, 3, -4);   // K(-Z)
    glVertex3i(-6, 3, -4);   // Z(-Z)
    glVertex3i(-6, 3, 4);   // Z

    glEnd();

    //차 후면 범퍼 KL,
    glColor3f(1.0, 1.0, 1.0);   // PINK
    glBegin(GL_POLYGON);
    glVertex3i(-9, 3, 4);   // K
    glVertex3i(-9, 0, 4);   // L
    glVertex3i(-9, 0, -4);   // L(-Z)
    glVertex3i(-9, 3, -4);   // K(-Z)
    glEnd();

    //차 손잡이1 [LEFT] A2 B2 C2 D2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(2, 2, 5);   // B2
    glVertex3i(0, 2, 5);   // C2
    glVertex3i(0, 3, 5);   // D2
    glEnd();
    //차 손잡이2 [LEFT] A2 D2 E2 F1
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(0, 3, 5);   // D2
    glVertex3i(0, 3, 4);   // E2
    glVertex3i(2, 3, 4);   // F1
    glEnd();
    //차 손잡이2 [LEFT] D2 E2 G2 C2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(0, 3, 5);   // D2
    glVertex3i(0, 3, 4);   // E2
    glVertex3i(0, 2, 4);   // G2
    glVertex3i(0, 2, 5);   // C2
    glEnd();
    //차 손잡이2 [LEFT] A2 F1 F2 B2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, 5);   // A2
    glVertex3i(2, 3, 4);   // F1
    glVertex3i(2, 2, 4);   // F2
    glVertex3i(2, 2, 5);   // B2
    glEnd();

    //차 손잡이1 [RIGHT] 
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(2, 2, -5);   // B2
    glVertex3i(0, 2, -5);   // C2
    glVertex3i(0, 3, -5);   // D2
    glEnd();
    //차 손잡이2 [RIGHT] A2 D2 E2 F1
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(0, 3, -5);   // D2
    glVertex3i(0, 3, -4);   // E2
    glVertex3i(2, 3, -4);   // F1
    glEnd();
    //차 손잡이2 [RIGHT] D2 E2 G2 C2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(0, 3, -5);   // D2
    glVertex3i(0, 3, -4);   // E2
    glVertex3i(0, 2, -4);   // G2
    glVertex3i(0, 2, -5);   // C2
    glEnd();
    //차 손잡이2 [RIGHT] A2 F1 F2 B2
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3i(2, 3, -5);   // A2
    glVertex3i(2, 3, -4);   // F1
    glVertex3i(2, 2, -4);   // F2
    glVertex3i(2, 2, -5);   // B2
    glEnd();

    //차 바닥1
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(6, 0, -4);   // A3
    glVertex3i(8, 0, -4);   // F
    glVertex3i(8, 0, 4);   // E
    glVertex3i(6, 0, 4);   // H
    glEnd();
    //차 바닥2
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3i(-3, 0, -4);   // N(Z-8)
    glVertex3i(3, 0, -4);   // I(Z-8)
    glVertex3i(3, 0, 4);   // I
    glVertex3i(-3, 0, 4);   // N
    glEnd();
    //차 바닥3
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
    case 'q': case 'Q': exit(0); break;//종료
        //case 'r': case 'R': glClearColor(1.0, 0.0, 0.0, 1.0); break;

    case 'c'://캡처
        //screen_dump();
        ScreenCapture("strFilePath.bmp");
        Sleep(100);
        break;
    case 'm': simuwork = CRS_MAIN; break;//메인메뉴
    }
    if (simuwork != CRS_MAIN) {
        switch (key) {
        case 'w'://전진
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
        case 's'://후진
            speed = 1;
            dxcar = speed * cos((180 - rcar) * PI / 180.0); xcar -= dxcar;
            dycar = speed * sin((180 - rcar) * PI / 180.0); ycar -= dycar;
            break;
        case 'a': rcar += 3; printf("r=%d\n", rcar); break;//우회전
        case 'd': rcar -= 3; printf("r=%d\n", rcar); break;//좌회전
        }
    }
    if (rcar >= 360 || rcar <= -360) rcar = 0;
    glutPostRedisplay();
}

void drawingCar_ok(float car_size) {//테스트용 자동차 모델링, 이동, 회전
    glPushMatrix();
    glTranslatef(xcar, 1, ycar);//자동차 이동
    glRotatef(rcar, 0.0f, 1.0f, 0.0f);//자동차 회전
    //glRotatef(180, 0.0f, 1.0f, 0.0f);//자동차 회전
    //glBegin(GL_LINES);
    //    glColor3f(0.0, 0.0, 1.0);
    //    glVertex3f(0.0, 0.0, 0.0);  glVertex3f(-10.0, 0.0, 0.0); /* Z axis  */
    //glEnd();
    glRotatef(180, 0.0f, 1.0f, 0.0f);//자동차 회전
    glScalef(car_size, car_size, car_size);
    drawingCar();
    //glutWireCube(5.0);
    glPopMatrix();
}

void drawScene(float car_size = 0.4, int line_width = 5) {//그릴 물체만
    glPushMatrix();
        display_lines(line_width);
    glPopMatrix();
// 평면 그리기
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
    glClearColor(.9f, .9f, .9f, 1.0f);//배경색

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
        glClearColor(.9f, .9f, .9f, 1.0f);//배경색

        glViewport(0, 0, Width, Height);

        glPushMatrix();
        gluLookAt(0.0, -100.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);//카메라 위치/바라보는 초점 위치/카메라 기울임

        glRotatef(rotationX, 1.0, 0.0, 0.0);
        glRotatef(rotationY, 0.0, 1.0, 0.0);
        glMultMatrixf(view_rotate);
        main_menu();



        glPopMatrix();
    }
    else//시뮬 진행중
    {//-------------------------전체 화면--------------------------------------
        perspective();
        glClearColor(.9f, .9f, .9f, 1.0f);//배경색
        glLoadIdentity();

        glViewport(0, 0, Width, Height);

        glPushMatrix();
        gluLookAt(xcar - 10 * cos((180 - rcar) * PI / 180.0), 20, ycar - 10 * sin((180 - rcar) * PI / 180.0),
            xcar + 10 * cos((180 - rcar) * PI / 180.0), 0.1, ycar + 10 * sin((180 - rcar) * PI / 180.0),
            0.0, 1.0, 0.0);//카메라 위치/바라보는 초점 위치/카메라 기울임

        glPushMatrix();

        glRotatef(rotationX, 1.0, 0.0, 0.0);
        glRotatef(rotationY, 0.0, 1.0, 0.0);
        glMultMatrixf(view_rotate);
        drawScene();

        glPopMatrix();
        glPopMatrix();

        //-----------------------오른쪽 위 화면----------------------------
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
            xcar = -60, ycar = 50;//자동차 이동/위치 초기화
            rcar = 180;//로테이션 초기화
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) //마우스 버튼에서 손 뗄 때
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