#include <GL/glut.h>
#include <stdlib.h>

#define HORIZONTAL 0
#define VERTICAL 1
#define MARGIN 10//격자|자동차 여백

int window_w = 600;//창 크기
int window_h = 600;
int selected_car = -1;//클릭한 차
int mouse_x = 0;//마지막으로 클릭한 위치
int mouse_y = 0;
float car_w = 0, car_h = 0;//자동차 영역 계산용 변수들
float x_low = 0, x_high = 0, y_low = 0, y_high = 0;
char game_clear = 0;

void car_range(int i);
char car_parked(int car);
void draw_car(int i);

typedef struct Car {
    GLfloat x, y;  //자동차 왼쪽 위 좌표
    GLubyte direction, size;//자동차 방향, 크기
} Car;

Car cars[] = {
    { 100, 200, HORIZONTAL, 2 },//빼내야 하는 노란색 자동차
    
    { 0,   0,   HORIZONTAL, 2 },//가로 방향으로 배치된 자동차들
    { 400, 300, HORIZONTAL, 2 },
    { 200, 400, HORIZONTAL, 2 },
    { 200, 500, HORIZONTAL, 3 },
    
    { 0,   200, VERTICAL, 2 },//세로 방향으로 배치된 자동차들
    { 0,   400, VERTICAL, 2 },
    { 100, 400, VERTICAL, 2 },
    { 300, 0,   VERTICAL, 2 },
    { 300, 200, VERTICAL, 2 },
    { 400, 0,   VERTICAL, 3 }
};

const int car_num = sizeof(cars) / sizeof(Car);//자동차 대수 자동 카운트

void renderBitmapString(float x, float y, float R, float G, float B, void* font, const char* string)
{
    const char* c;
    glColor3f(R, G, B);
    glPushMatrix();
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
    glPopMatrix();
}

void Timer(int value) {//엔딩 애니메이션
    if (cars[0].x >= 450 && cars[0].x < 900){
        cars[0].x += 1;
        game_clear = 1;
        glutPostRedisplay();
        glutTimerFunc(10, Timer, 0);
    }
    else if(cars[0].x >= 800) {
        exit(0);
    }
}

void MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {//마우스 버튼 누를 때
        for (int i = 0; i < car_num; i++) {
            car_range(i);//자동차 좌표 계산
            if (x >= x_low && x <= x_high && y >= y_low && y <= y_high) {//선택한 자동차 확정
                selected_car = i;
                mouse_x = x;//이전 좌표 저장
                mouse_y = y;
                break;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {//마우스 버튼에서 손 뗄 때
        if (cars[selected_car].direction == HORIZONTAL) {
            if (cars[0].x >= 450) {
                cars[0].x = 500;
                glutTimerFunc(1, Timer, 0);//자동정렬 전 퍼즐 해결시 엔딩
            }

            if (((int)cars[selected_car].x % 100) < 50)//x값을 가까운 곳으로 자동 정렬
                cars[selected_car].x -= (int)cars[selected_car].x % 100;
            else cars[selected_car].x += 100 - (int)cars[selected_car].x % 100;
        }
        else {
            if (((int)cars[selected_car].y % 100) < 50)//y값을 가까운 곳으로 자동 정렬
                cars[selected_car].y -= (int)cars[selected_car].y % 100;
            else cars[selected_car].y += 100 - (int)cars[selected_car].y % 100;
        }
        selected_car = -1;
        glutPostRedisplay();
    }
}

void MouseDrag(int x, int y) {
    if (selected_car != -1) {
        if (cars[selected_car].direction == HORIZONTAL) {//가로 배치된 자동차는 가로로만 움직임
            if ( cars[selected_car].x >= 0.0 && ( (cars[selected_car].x + cars[selected_car].size * 100) <= 600 || selected_car == 0) ) {
                float dx = x - mouse_x;//x이동 거리 계산
                cars[selected_car].x += dx;//x이동 거리 적용
                if (car_parked(selected_car)) cars[selected_car].x -= dx;//이미 다른 자동차가 주차된 칸이면 그 x이동 거리 취소
                if (cars[selected_car].x < 0.0) cars[selected_car].x = 0.0;//화면 밖으로 못 나가게 제한
                else if (((cars[selected_car].x + cars[selected_car].size * 100) > 600) && selected_car != 0)
                    cars[selected_car].x = 600 - cars[selected_car].size * 100;
                mouse_x = x;//이전 좌표 저장
            }
        }
        else {//세로 배치된 자동차는 가로로만 움직임
            if ( cars[selected_car].y >= 0.0 && ( (cars[selected_car].y + cars[selected_car].size * 100) <= 600) ) {
                float dy = y - mouse_y;//y이동 거리 계산
                cars[selected_car].y += dy;//y이동 거리 적용
                if (car_parked(selected_car))cars[selected_car].y -= dy;//이미 다른 자동차가 주차된 칸이면 그 y이동 거리 취소
                if (cars[selected_car].y < 0.0) cars[selected_car].y = 0.0;//화면 밖으로 못 나가게 제한
                else if ((cars[selected_car].y + cars[selected_car].size * 100) > 600)
                    cars[selected_car].y = 600 - cars[selected_car].size * 100;
                mouse_y = y;//이전 좌표 저장
            }
        }
        glutPostRedisplay();
    }
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);//격자 생성 시작
    glColor3f(0.5, 0.5, 0.5);
    for (int x = 0.0; x <= 600; x += 100) {
        glVertex2f(x, 0.0);
        glVertex2f(x, 600.0);
    }
    for (int y = 0.0; y <= 600; y += 100) {
        glVertex2f(0.0, y);
        glVertex2f(600.0, y);
    }
    glEnd();//격자 생성 완료

    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_QUADS);//노란색 차 전용 진한 도로
    glVertex2f(0, 199);
    glVertex2f(0, 300);
    glVertex2f(700, 300);
    glVertex2f(700, 199);
    glEnd();

    for (int i = 0; i < car_num; ++i) {
        car_range(i);//자동차 좌표 계산
        if (i == 0)glColor3f(0.9, 0.9, 0.0);//자동차 색상 적용
        else if (cars[i].size == 2 && cars[i].direction == VERTICAL)glColor3f(0.2, 0.4, 0.8);
        else if (cars[i].size == 3 && cars[i].direction == VERTICAL)glColor3f(0.1, 0.1, 0.8);
        else if (i != 0 && cars[i].direction == HORIZONTAL)glColor3f(0.9, 0.9, 0.9);
        draw_car(i);
    }

    if (game_clear == 1) {//클리어 후 노란색 자동차 따라가는 글자
        renderBitmapString(cars[0].x - 300, 260, 1.0, 1.0, 1.0, GLUT_BITMAP_TIMES_ROMAN_24, "C  L  E  A  R");
    }

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(600, 200);
    glutCreateWindow("60191798 김영찬");
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glOrtho(0.0, window_w, window_h, 0.0, -1.0, 1.0);
    glutDisplayFunc(Display);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseDrag);
    glutMainLoop();
    return 0;
}

void car_range(int i) {//i번째 자동차 꼭지점 위치 계산
    if (cars[i].direction == HORIZONTAL) {
        car_w = cars[i].size * 100 - MARGIN * 2;
        car_h = 100 - MARGIN * 2;
    }
    else {
        car_w = 100 - MARGIN * 2;
        car_h = cars[i].size * 100 - MARGIN * 2;
    }
    x_low = cars[i].x + MARGIN; x_high = x_low + car_w;
    y_low = cars[i].y + MARGIN; y_high = y_low + car_h;
}

char car_parked(int car) {
    car_range(car);//현재 자동차의 꼭지점 좌표 저장
    int car_x_low = x_low;
    int car_x_high = x_high;
    int car_y_low = y_low;
    int car_y_high = y_high;
    for (int i = 0; i < car_num; i++) {
        if (i != car) {//현재 자동차를 제외한 다른 자동차와 현재 자동차를 비교
            car_range(i);//MARGIN을 제외하여 다른 자동차가 있는 주차구역을 범위로 지정
            int parked_car_x_low = (int)(x_low - MARGIN);
            int parked_car_x_high = (int)(x_high + MARGIN);
            int parked_car_y_low = (int)(y_low - MARGIN);
            int parked_car_y_high = (int)(y_high + MARGIN);

            if (car_x_low >= parked_car_x_low && car_x_low <= parked_car_x_high && car_y_low >= parked_car_y_low && car_y_low <= parked_car_y_high)
                return 1;//low-low
            if (car_x_high >= parked_car_x_low && car_x_high <= parked_car_x_high && car_y_low >= parked_car_y_low && car_y_low <= parked_car_y_high)
                return 1;//high-low
            if (car_x_low >= parked_car_x_low && car_x_low <= parked_car_x_high && car_y_high >= parked_car_y_low && car_y_high <= parked_car_y_high)
                return 1;//low-high
            if (car_x_high >= parked_car_x_low && car_x_high <= parked_car_x_high && car_y_high >= parked_car_y_low && car_y_high <= parked_car_y_high)
                return 1;//high-high
        }
    }
    return 0;//안 겹침
}

void draw_car(int i)
{
    float window_margin = 3;
    float point = 0;
    float point_window = 0;
    int backmirror = 0;
    //그림 그릴 때 좌표 정하기 편하도록 자동차 크기를 등분한 변수
    if (cars[i].direction == HORIZONTAL) {
        point = (y_high - y_low) / 4;
    }
    else {
        point = (x_high - x_low) / 4;
    }
    //자동차 생성 시작
    glBegin(GL_POLYGON);//꼭지점 둥근 사각형 생성 시작
    if (cars[i].direction == HORIZONTAL) {
        glVertex2f(x_low + point, y_low);
        glVertex2f(x_high - point, y_low);
    }
    else {
        glVertex2f(x_low, y_low);
        glVertex2f(x_high, y_low);
    }
    glVertex2f(x_high, y_low + point);
    glVertex2f(x_high, y_high - point);
    glVertex2f(x_high - point, y_high);
    glVertex2f(x_low + point, y_high);
    glVertex2f(x_low, y_high - point);
    glVertex2f(x_low, y_low + point);
    glEnd();

    glPushMatrix();//꼭지점 둥글게
    glTranslatef(x_low + point, y_low + point, 0.0);
    GLUquadricObj* vertex1 = gluNewQuadric();
    gluDisk(vertex1, 0.0, point, 100, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x_high - point, y_low + point, 0.0);
    GLUquadricObj* vertex2 = gluNewQuadric();
    gluDisk(vertex2, 0.0, point, 100, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x_high - point, y_high - point, 0.0);
    GLUquadricObj* vertex3 = gluNewQuadric();
    gluDisk(vertex3, 0.0, point, 100, 1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x_low + point, y_high - point, 0.0);
    GLUquadricObj* vertex4 = gluNewQuadric();
    gluDisk(vertex4, 0.0, point, 100, 1);
    glPopMatrix();//꼭지점 둥근 사각형 생성 완료

    point_window = point * 2;//창문 위치 전용 변수

    if (i == selected_car)
        backmirror = 1;//선택한 자동차는 백미러 펴짐
    glPushMatrix();//자동차 방향, 선택된 자동차에 따른 백미러 생성
    GLUquadricObj* vertex5 = gluNewQuadric();
    if (cars[i].direction == HORIZONTAL) {
        glTranslatef(x_high - point_window - 10, y_low + 1, 0.0);
        gluPartialDisk(vertex5, 0.0, point_window / 4, 100, 1, -110 - backmirror * 60, -180);
    }
    else {
        glTranslatef(x_high - 1, y_high - point_window, 0.0);
        gluPartialDisk(vertex5, 0.0, point_window / 4, 100, 1, -20 - backmirror * 60, 180);
    }
    glPopMatrix();
    glPushMatrix();
    GLUquadricObj* vertex6 = gluNewQuadric();
    if (cars[i].direction == HORIZONTAL) {
        glTranslatef(x_high - point_window - 10, y_high - 1, 0.0);
        gluPartialDisk(vertex6, 0.0, point_window / 4, 100, 1, 110 + backmirror * 60, -180);
    }
    else {
        glTranslatef(x_low + 1, y_high - point_window, 0.0);
        gluPartialDisk(vertex6, 0.0, point_window / 4, 100, 1, 20 + backmirror * 60, -180);
    }
    glPopMatrix();

    if (i == selected_car) glColor3f(1.0, 1.0, 0.2);
    else glColor3f(0.5, 0.5, 0.5);//선택한 자동차는 전조등 켜짐
    glPushMatrix();//자동차 방향, 선택된 자동차에 따른 전조등 생성
    GLUquadricObj* vertex7 = gluNewQuadric();
    if (cars[i].direction == HORIZONTAL) {
        glTranslatef(x_high - point, y_low + point, 0.0);
        gluPartialDisk(vertex7, point - 5, point, 100, 1, 90, 70);
    }
    else {
        glTranslatef(x_low + point, y_high - point, 0.0);
        gluPartialDisk(vertex7, point - 5, point, 100, 1, 0, -70);
    }
    glPopMatrix();
    glPushMatrix();
    GLUquadricObj* vertex8 = gluNewQuadric();
    if (cars[i].direction == HORIZONTAL) {
        glTranslatef(x_high - point, y_high - point, 0.0);
        gluPartialDisk(vertex8, point - 5, point, 100, 1, 90, -70);
    }
    else {
        glTranslatef(x_high - point, y_high - point, 0.0);
        gluPartialDisk(vertex8, point - 5, point, 100, 1, 0, 70);
    }
    glPopMatrix();

    glBegin(GL_QUADS);//창문 생성 시작
    if (cars[i].direction == HORIZONTAL) {
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window, y_low + window_margin * 2);//back-1
        glColor3f(0.2, 0.2, 0.2); glVertex2f(x_low + point_window + 20, y_low + window_margin * 4);//back-2
        glColor3f(0.3, 0.3, 0.3); glVertex2f(x_low + point_window + 20, y_high - window_margin * 4);//back-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window, y_high - window_margin * 2);//back-4

        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window, y_low + window_margin * 2);//front-1
        glColor3f(0.2, 0.2, 0.2); glVertex2f(x_high - point_window, y_high - window_margin * 2);//front-2
        glColor3f(0.3, 0.3, 0.3); glVertex2f(x_high - point_window - 25, y_high - window_margin * 4);//front-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window - 25, y_low + window_margin * 4);//front-4

        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window + window_margin * 3, y_low + window_margin * 2);//left-1
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window - window_margin * 3, y_low + window_margin * 2);//left-2
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window - window_margin * 3 - 25, y_low + window_margin * 4);//left-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window + window_margin * 3 + 20, y_low + window_margin * 4);//left-4

        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window - window_margin * 3, y_high - window_margin * 2);//right-1
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window + window_margin * 3, y_high - window_margin * 2);//right-2
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + point_window + window_margin * 3 + 20, y_high - window_margin * 4);//right-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - point_window - window_margin * 3 - 25, y_high - window_margin * 4);//right-4
    }
    else {//세로 방향일 때 트럭
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 2, y_high - point_window / 2);//front-1
        glColor3f(0.2, 0.2, 0.2); glVertex2f(x_high - window_margin * 2, y_high - point_window / 2);//front-2
        glColor3f(0.3, 0.3, 0.3); glVertex2f(x_high - window_margin * 4, y_high - point_window / 2 - 20);//front-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 4, y_high - point_window / 2 - 20);//front-4

        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 2, y_high - point_window / 2 - 40 - window_margin * 3);//right-1
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 2, y_high - point_window / 2 - window_margin * 3);//right-2
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 4, y_high - point_window / 2 - 20 - window_margin * 3);//right-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_low + window_margin * 4, y_high - point_window / 2 - 40 - window_margin * 3);//right-4

        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - window_margin * 2, y_high - point_window / 2 - 40 - window_margin * 3);//right-1
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - window_margin * 2, y_high - point_window / 2 - window_margin * 3);//right-2
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - window_margin * 4, y_high - point_window / 2 - 20 - window_margin * 3);//right-3
        glColor3f(0.1, 0.1, 0.1); glVertex2f(x_high - window_margin * 4, y_high - point_window / 2 - 40 - window_margin * 3);//right-4
    }
    glEnd();//창문 생성 완료

    if (cars[i].direction == VERTICAL) {//세로 방향일 때만 트럭 모양
        float color[3] = { 0.2, 0.4, 0.8 };
        glBegin(GL_QUADS);//트럭 짐칸 생성 시작
        if (cars[i].size == 3) {
            color[0] = 0.1; color[1] = 0.1; color[2] = 0.8;
        }
        glColor3f(color[0] - 0.1, color[1] - 0.1, color[2] - 0.1); glVertex2f(x_low + 2, y_low + 2);
        glColor3f(color[0] + 0.1, color[1] + 0.1, color[2] + 0.1); glVertex2f(x_low + 2, y_high - 80);
        glColor3f(color[0] + 0.2, color[1] + 0.2, color[2] + 0.2); glVertex2f(x_high - 2, y_high - 80);
        glColor3f(color[0] + 0.0, color[1] + 0.0, color[2] + 0.0); glVertex2f(x_high - 2, y_low + 2);
        glEnd();//트럭 짐칸 생성 완료
    }
}
