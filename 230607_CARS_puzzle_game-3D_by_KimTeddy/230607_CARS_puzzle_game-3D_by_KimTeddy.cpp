#include <GL/glut.h>
#include <GL/glui.h>

#define HORIZONTAL 0
#define VERTICAL 1
#define MARGIN 2//격자|자동차 여백

void play_solution();
void reset(int id);
void car_range(int i);

GLUI* glui, *glui2;
GLUI_Checkbox* checkbox_wireframe;
GLUI_Listbox* listbox;
GLUI_RadioGroup* radioGroup;
GLUI_Panel* obj_panel;
GLUI_Rotation* view_rot;
GLUI_Scrollbar* scrollbar_car_r, * scrollbar_car_g, * scrollbar_car_b;
GLUI_Scrollbar* scrollbar_light_r, * scrollbar_light_g, * scrollbar_light_b;
GLUI_Translation* trans_xy, * trans_x, * trans_y, * trans_z;
int mainWindow;

int mouse_x = 0;//마지막으로 클릭한 위치
int mouse_y = 0;
float car_w = 0, car_h = 0;//자동차 영역 계산용 변수들
float x_low = 0, x_high = 0, y_low = 0, y_high = 0;
char game_clear = 0;

//솔루션
char solution_level = 0, solution_complete = 0;
int solution_level_complete = 0;
char anim = 0;

float car_pos[] = { 0.0, 0.0, 0.0 };
float car_pos_prev[] = { 0.0, 0.0, 0.0 };
int car_pos_delta[] = {0, 0, 0};

float positionX = 0, positionY = 0;
// 회전을 위한 변수들
int rotationX = 0, rotationY = 0;
float rotationText = 0;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

// 조명 관련 변수들
bool lightingEnabled = true;
GLfloat lightPosition[] = { 0.0f, 2.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0f };
GLfloat lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat carColor[3] = { 0.9, 0.9, 0.0 };

int wireFrame = 0;

// 라디오 버튼 관련 변수들
int selected_car = 0;//선택한 차
int radiogroup_value = 0;
// 리스트 박스 관련 변수들
int listbox_value = 0;

typedef struct Car {
    GLfloat x, y;  //자동차 왼쪽 위 좌표
    GLubyte direction, size;//자동차 방향, 크기
} Car;
typedef struct CarColor {
    GLfloat r, g, b;
} CarColor;
typedef struct Solution {
    GLint car, delta;
} Solution;

Car cars[] = {
    { 10.0, 20.0, HORIZONTAL, 2 },//빼내야 하는 노란색 자동차

    { 0.0,  30.0, HORIZONTAL, 2 },//가로 방향으로 배치된 자동차들
    { 20.0, 50.0, HORIZONTAL, 2 },

    { 10.0, 40.0, VERTICAL, 2 },//세로 방향으로 배치된 자동차들
    { 20.0, 30.0, VERTICAL, 2 },
    { 30.0, 10.0, VERTICAL, 3 },
    { 40.0, 10.0, VERTICAL, 3 }
}; 

const int car_num = sizeof(cars) / sizeof(Car);//자동차 대수 자동 카운트

Car cars_init[car_num];
CarColor car_colors[car_num];

Solution solution[] = {
    {0, -10},
    {4, -30},
    {5, -10},
    {6, -10},
    {1, 40},
    {3, -10},
    {2, -20},
    {5, 30},
    {0, 20},
    {3, -30},
    {0, -20},
    {5, -30},
    {1, -40},
    {5, 30},
    {6, 30},
    {0, 60},
};

const int solution_num = sizeof(solution) / sizeof(Solution);

void LightInit()
{
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glPopMatrix();
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightAmbient);
    // Global ambient light
    float GAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GAmbient);
    // Enable local viewpoint
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    // Material
    GLfloat material_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat material_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat material_shininess[] = { 32.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void renderStrokeString(float x, float y, float z, float R, float G, float B, float Scale, void* font, char* string)
{
    char* c;
    glColor3f(R, G, B);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(Scale, Scale, Scale);
    for (c = string; *c != '\0'; c++) {
        glutStrokeCharacter(font, *c);
    }
    glPopMatrix();
}

void Timer(int value) {
    if (value == 0)//엔딩 애니메이션
    {
        if (cars[0].x >= 40 && cars[0].x < 110) {
            cars[0].x += 0.1;
            game_clear = 1;
            glutPostRedisplay();
        }
        rotationText += 0.1;
        glutTimerFunc(100, Timer, 0);
    }
    else if (value == 1) {
        if (game_clear == 0) {
            if (cars[selected_car].direction == HORIZONTAL) {

                if (((int)cars[selected_car].x % 10) <= 5)//x값을 가까운 곳으로 자동 정렬
                    cars[selected_car].x -= ((int)cars[selected_car].x % 10);
                else
                {
                    cars[selected_car].x += (10 - (int)cars[selected_car].x % 10);
                    cars[selected_car].x = (int)cars[selected_car].x;
                }

            }
            else {
                if (((int)cars[selected_car].y % 10) <= 5)//y값을 가까운 곳으로 자동 정렬
                    cars[selected_car].y -= ((int)cars[selected_car].y % 10);
                else {

                    cars[selected_car].y += (int)(10 - (int)cars[selected_car].y % 1);
                    cars[selected_car].y = (int)cars[selected_car].y;
                }
            }
            glutPostRedisplay();
        }
    }
    else if (value == 2) {
        if (anim == 1)
        {
            play_solution();
            glutPostRedisplay();
            if (solution_level < solution_num) glutTimerFunc(50, Timer, 2);
            else {
                solution_complete = 1;
                anim = 0;
                listbox->enable();
                radioGroup->enable();
                GLUI_Master.sync_live_all();

                glutTimerFunc(100, Timer, 0);
            }
        }
    }
}


void play_solution() {
    if (solution_complete == 0) {
        if (cars[solution[solution_level].car].direction == HORIZONTAL) {

            cars[solution[solution_level].car].x += solution[solution_level].delta / 10;
        }
        else {
            cars[solution[solution_level].car].y += solution[solution_level].delta / 10;
        }
        solution_level_complete += solution[solution_level].delta / 10;
        if (solution_level_complete == solution[solution_level].delta) {
            solution_level++;
            solution_level_complete = 0;
        }
    }
    if (solution_level >= solution_num)solution_complete = 1;
    else {
        selected_car = solution[solution_level].car;
        radioGroup->set_int_val(selected_car);
        scrollbar_car_r->set_float_val(car_colors[selected_car].r);
        scrollbar_car_g->set_float_val(car_colors[selected_car].g);
        scrollbar_car_b->set_float_val(car_colors[selected_car].b);
        GLUI_Master.sync_live_all();
    }
}

void draw_axes(float scale)
{
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glScalef(scale, scale, scale);

    glBegin(GL_LINES);

    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(.8f, 0.05f, 0.0);  glVertex3f(1.0, 0.25f, 0.0); /* Letter X*/
    glVertex3f(0.8f, .25f, 0.0);  glVertex3f(1.0, 0.05f, 0.0);
    glVertex3f(0.0, 0.0, 0.0);  glVertex3f(1.0, 0.0, 0.0); /* X axis */

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);  glVertex3f(0.0, 1.0, 0.0); /* Y axis */

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);  glVertex3f(0.0, 0.0, 1.0); /* Z axis  */
    glEnd();

    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 60.0, 30.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();
    glTranslatef(0, 0, -car_pos[2]);
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glMultMatrixf(view_rotate);
    // 평면 그리기
    glPushMatrix();

    glColor3f(0.2, 0.2, 0.2);
    glTranslatef(0.0, -0.5, 0.0);
    glScalef(60.0, 1.0, 60.0);
    if (wireFrame == 0)
        glutSolidCube(1.0);
    else glutWireCube(1.0);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-30.0, 0.02, -30.0);
    glBegin(GL_LINES);//격자 생성 시작
    glColor3f(0.5, 0.5, 0.5);
    glLineWidth(3);
    for (int x = 0.0; x <= 60; x += 10) {
        glVertex3f(x, 0.0, 0.0);
        glVertex3f(x, 0.0, 60.0);
    }
    for (int z = 0.0; z <= 60; z += 10) {
        glVertex3f(0.0, 0.0, z);
        glVertex3f(60.0, 0.0, z);
    }
    glEnd();//격자 생성 완료
    if (wireFrame == 0)
    {
        glColor3f(0.1, 0.1, 0.1);
        glTranslatef(0.0, -0.01, 0.0);
        glBegin(GL_QUADS);//노란색 차 전용 진한 도로
        glVertex3f(0, 0, 20);
        glVertex3f(0, 0, 30);
        glVertex3f(60, 0, 30);
        glVertex3f(60, 0, 20);
        glEnd();
    }
    glPopMatrix();


    //자동차
    for (int i = 0; i < car_num; i++) {
        char num[2] = { 0 };
        num[0] = i + 1 + '0';
        glPushMatrix();
        glTranslatef(cars[i].x, 0.0, cars[i].y);
        glTranslatef(-30.0, 3.0, -30.0);

        glDisable(GL_LIGHTING);
        glPushMatrix();
        car_range(i);
        glTranslatef(car_w/2, 3, car_h/2+4);
        glRotatef(-90, 1.0, 0.0, 0.0);
        renderStrokeString(0, 0, 0, 1 - car_colors[i].r, 1 - car_colors[i].g, 1 - car_colors[i].b, 0.04, GLUT_STROKE_MONO_ROMAN, num);
        glPopMatrix();
        glEnable(GL_LIGHTING);

        if (cars[i].direction == HORIZONTAL) {
            glScalef(cars[i].size * 10.0, 5.0, 10.0);
        }
        else {
            glScalef(10.0, 5.0, cars[i].size * 10.0);
        }
        if (cars[i].direction == HORIZONTAL)
            glTranslatef(0.5, 0.0, 0.5);
        else
            glTranslatef(0.5, 0.0, 0.5);
        glScalef(0.9, 1.0, 0.9);

        glColor3f(car_colors[i].r+lightDiffuse[0]-1, car_colors[i].g + lightDiffuse[1] - 1, car_colors[i].b + lightDiffuse[2] - 1);
        if (wireFrame == 0)
        {
            glutSolidCube(1.0f);
            if (i == selected_car)
                glColor3f(1.0, 0.0, 0.0);
            else
                glColor3f(0.0, 0.0, 0.0);

            glPushMatrix();
            glLineWidth(5);
            glutWireCube(1.0f);
            glPopMatrix();
        }
        else glutWireCube(1.0f);
        glPopMatrix();

        glDisable(GL_LIGHTING);
        glPushMatrix();
        if (game_clear == 1) {//클리어 후 나타나는 글자
            glRotatef(rotationText*10, 0.0f, 1.0f, 0.0f);
            renderStrokeString(-20, 10.0+5*sin(rotationText), 0, 0.0, 1.0, 0.0, 0.04, GLUT_STROKE_MONO_ROMAN, (char*)"C L E A R");
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);
        
    }
    
glPopMatrix();

    glPushMatrix();
    glTranslatef(-100, -100, 10);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glMultMatrixf(view_rotate);
    draw_axes(10);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, (double)width / (double)height, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouse_x = x;
        mouse_y = y;
        view_rot->reset();
        GLUI_Master.sync_live_all();
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) //마우스 버튼에서 손 뗄 때
    {
        if (cars[0].x >= 45 && game_clear == 0) {
            cars[0].x = 50;
            glutTimerFunc(100, Timer, 0);//자동정렬 전 퍼즐 해결시 엔딩
        }
        if (game_clear == 0) {
            for (int i = 0; i < car_num; i++) {
                if (cars[i].direction == HORIZONTAL) {
                    if (((int)cars[i].x % 10) < 5)//x값을 가까운 곳으로 자동 정렬
                        cars[i].x -= (int)cars[i].x % 10;
                    else
                    {
                        cars[i].x += 10 - (int)cars[i].x % 10;
                        cars[i].x = (int)cars[i].x;
                    }
                }
                else {
                    if (((int)cars[i].y % 10) < 5)//y값을 가까운 곳으로 자동 정렬
                        cars[i].y -= (int)cars[i].y % 10;
                    else
                    {
                        cars[i].y += 10 - (int)cars[i].y % 10;
                        cars[i].y = (int)cars[i].y;
                    }
                }
            }
        }
        glutPostRedisplay();
    }
}

void car_range(int i) {//i번째 자동차 꼭지점 위치 계산
    if (cars[i].direction == HORIZONTAL) {
        car_w = cars[i].size * 10 - MARGIN * 2;
        car_h = 10 - MARGIN * 2;
    }
    else {
        car_w = 10 - MARGIN * 2;
        car_h = cars[i].size * 10 - MARGIN * 2;
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
void translation(float x, float y) {
    if (game_clear == 1){
        trans_xy->disable();
        trans_x->disable();
        trans_y->disable();
        GLUI_Master.sync_live_all();
    }
    else if (cars[selected_car].direction == HORIZONTAL) {//가로 배치된 자동차는 가로로만 움직임
        if (cars[selected_car].x >= 0.0 && ((cars[selected_car].x + cars[selected_car].size * 10) <= 60 || selected_car == 0)) {
            float dx = x - positionX;//x이동 거리 계산
            cars[selected_car].x += dx;//x이동 거리 적용
            if (car_parked(selected_car)) cars[selected_car].x -= dx;//이미 다른 자동차가 주차된 칸이면 그 x이동 거리 취소
            if (cars[selected_car].x < 0) cars[selected_car].x = 0.0;//화면 밖으로 못 나가게 제한
            else if (((cars[selected_car].x + cars[selected_car].size * 10) > 60) && selected_car != 0)
                cars[selected_car].x = 60 - cars[selected_car].size * 10;
            positionX = x;//이전 좌표 저장
        }
    }
    else if (cars[selected_car].direction == VERTICAL) {//세로 배치된 자동차는 가로로만 움직임
        if (cars[selected_car].y >= 0.0 && ((cars[selected_car].y + cars[selected_car].size * 10) <= 60)) {
            float dy = y - positionY;//y이동 거리 계산
            cars[selected_car].y += dy;//y이동 거리 적용
            if (car_parked(selected_car)) cars[selected_car].y -= dy;//이미 다른 자동차가 주차된 칸이면 그 y이동 거리 취소
            if (cars[selected_car].y < 0) cars[selected_car].y = 0.0;//화면 밖으로 못 나가게 제한
            else if ((cars[selected_car].y + cars[selected_car].size * 10) > 60)
                cars[selected_car].y = 60 - cars[selected_car].size * 10;
            positionY = y;//이전 좌표 저장
        }
    }
    //if (game_clear == 0) glutTimerFunc(2000, Timer, 1);
}

void translationCallback(int id) {
    if (cars[selected_car].direction == HORIZONTAL) {
        trans_x->enable();
        trans_y->disable();
    }
    else {
        trans_y->enable();
        trans_x->disable();
    }
    //printf("%d, delta=%d, %d, car_pos=%f, %f\n", selected_car, car_pos_delta[0], car_pos_delta[1], car_pos[0], car_pos[1]);
    translation(cars[selected_car].x + car_pos[0], cars[selected_car].y - car_pos[1]);
    if (cars[0].x >= 45 && game_clear == 0) {
        cars[0].x = 50;
        glutTimerFunc(100, Timer, 0);//자동정렬 전 퍼즐 해결시 엔딩
    }
    GLUI_Master.sync_live_all();
    glutPostRedisplay();
}

void checkboxCallback(int id)
{
    if (id == 1) // 조명 체크박스
    {
        glEnable(GL_LIGHTING);
    }
    glutPostRedisplay();
}

void scrollbarCallback(int id)
{
    if (id == 1) {
        float v[] = {
              lightDiffuse[0],  lightDiffuse[1],
              lightDiffuse[2],  lightDiffuse[3] };

        glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
    }
    else if (id == 2) {
    printf("%d", selected_car);
        car_colors[selected_car].r = carColor[0];
        car_colors[selected_car].g = carColor[1];
        car_colors[selected_car].b = carColor[2];
    }
    GLUI_Master.sync_live_all();
    glutPostRedisplay();
}
void edittextCallback(int id) {
    if (id == 1) {
        float v[] = {
              lightDiffuse[0],  lightDiffuse[1],
              lightDiffuse[2],  lightDiffuse[3] };

        glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, v);
    }
    else if (id == 2) {
        printf("%d", selected_car);
        car_colors[selected_car].r = carColor[0];
        car_colors[selected_car].g = carColor[1];
        car_colors[selected_car].b = carColor[2];
    }
    GLUI_Master.sync_live_all();
}

void radioButtonCallback(int id)
{
    //car_range(selected_car); printf("car_w=%f, car_h=%f, x_low=%f, x_high=%f, y_low=%f, y_high=%f\n", car_w, car_h, x_low, x_high, y_low, y_high);
    positionX = cars[selected_car].x;
    positionY = cars[selected_car].y;
    //car_pos[0] = 0;
    //car_pos[1] = 0;
    float arr[] = { 0,0,car_pos[2] };
    trans_y->set_float_val(0);
    trans_x->set_float_val(0);
    trans_xy->set_float_array_val(arr);

    scrollbar_car_r->set_float_val(car_colors[selected_car].r);
    scrollbar_car_g->set_float_val(car_colors[selected_car].g);
    scrollbar_car_b->set_float_val(car_colors[selected_car].b);

    if (cars[selected_car].direction == HORIZONTAL) {
        trans_x->enable();
        trans_y->disable();
    }
    else {
        trans_y->enable();
        trans_x->disable();
    }
    printf("%d\n", selected_car);
    GLUI_Master.sync_live_all();

    glutPostRedisplay();
}

void listboxnCallback(int id) {
    positionX = cars[selected_car].x;
    positionY = cars[selected_car].y;
    car_pos[0] = 0;
    car_pos[1] = 0;
    float arr[] = { 0,0,car_pos[2] };
    trans_y->set_float_val(0);
    trans_x->set_float_val(0);
    trans_xy->set_float_array_val(arr);

    scrollbar_car_r->set_float_val(car_colors[selected_car].r);
    scrollbar_car_g->set_float_val(car_colors[selected_car].g);
    scrollbar_car_b->set_float_val(car_colors[selected_car].b);

    if (cars[selected_car].direction == HORIZONTAL) {
        trans_x->enable();
        trans_y->disable();
    }
    else {
        trans_y->enable();
        trans_x->disable();
    }
    printf("%d\n", selected_car);
    GLUI_Master.sync_live_all();

    glutPostRedisplay();
}

void spinnerCallback(int id) {
    if (id == 1) {
        float v[] = {
              lightDiffuse[0],  lightDiffuse[1],
              lightDiffuse[2],  lightDiffuse[3] };

        glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, v);
    }
    GLUI_Master.sync_live_all();
}

void myGlutIdle(void)
{
    if (glutGetWindow() != mainWindow)
        glutSetWindow(mainWindow);

    glutPostRedisplay();
}

void rotationCallback(int id) {
    rotationX = 0, rotationY = 0;

    glutPostRedisplay();
}

void buttonCallback(int id) {
    switch (id) {
    case 1:
        reset(1);
        listbox->disable();
        radioGroup->disable();
        trans_xy->disable();
        trans_x->disable();
        trans_y->disable();
        anim = 1;
        glutTimerFunc(100, Timer, 2);
        break;
    default: break;
    }
}

void reset(int id) {
    for (int i = 0; i < car_num; i++) {
        if (i == 0)car_colors[i] = { 0.9, 0.9, 0.0 };//자동차 색상 적용
        else if (cars[i].size == 2 && cars[i].direction == VERTICAL)car_colors[i] = { 0.2, 0.4, 0.8 };
        else if (cars[i].size == 3 && cars[i].direction == VERTICAL)car_colors[i] = { 0.1, 0.1, 0.8 };
        else if (i != 0 && cars[i].direction == HORIZONTAL)car_colors[i] = { 0.9, 0.9, 0.9 };
    }
    if (id == 0) {
        for (int i = 0; i < car_num; i++) {
            cars_init[i] = cars[i];
        }
    }
    if (id == 1)
    {    //변수 초기값
        mouse_x = 0;//마지막으로 클릭한 위치
        mouse_y = 0;
        car_w = 0, car_h = 0;//자동차 영역 계산용 변수들
        x_low = 0, x_high = 0, y_low = 0, y_high = 0;
        game_clear = 0;

        //솔루션
        solution_level = 0, solution_complete = 0;
        solution_level_complete = 0;
        anim = 0;

        for (int i = 0; i < 3; i++) {
            car_pos[i] = 0;
        }
        for (int i = 0; i < 3; i++) {
            car_pos_prev[i] = 0;
        }
        for (int i = 0; i < 3; i++) {
            car_pos_delta[i] = 0;
        }
        positionX = 0, positionY = 0;
        // 회전을 위한 변수들
        rotationX = 0, rotationY = 0, rotationText = 0;
        float view_rotate_init[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
        for (int i = 0; i < 16; i++) {
            view_rotate[i] = view_rotate_init[i];
        }

        // 조명 관련 변수들
        lightingEnabled = true;
        GLfloat lightPosition_init[] = { 0.0f, 3.0f, 0.0f, 0.0f };
        for (int i = 0; i < 4; i++) {
            lightPosition[i] = lightPosition_init[i];
        }
        GLfloat lightDiffuse_init[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        for (int i = 0; i < 4; i++) {
            lightDiffuse[i] = lightDiffuse_init[i];
        }
        GLfloat lightAmbient_init[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        for (int i = 0; i < 4; i++) {
            lightAmbient[i] = lightAmbient_init[i];
        }
        GLfloat carColor_init[3] = { 0.0 };
        for (int i = 0; i < 3; i++) {
            carColor[i] = carColor_init[i];
        }
        for (int i = 0; i < car_num; i++) {
            cars[i] = cars_init[i];
        }

        float v[] = {
              lightDiffuse[0],  lightDiffuse[1],
              lightDiffuse[2],  lightDiffuse[3] };

        glLightfv(GL_LIGHT0, GL_DIFFUSE, v);

        wireFrame = 0;
        selected_car = 0;//선택한 차
        listbox_value = 0;
        radiogroup_value = 0;
        checkbox_wireframe->set_int_val(wireFrame);

        scrollbar_car_r->set_float_val(car_colors[selected_car].r);
        scrollbar_car_g->set_float_val(car_colors[selected_car].g);
        scrollbar_car_b->set_float_val(car_colors[selected_car].b);

        listbox->enable();
        radioGroup->enable();
        trans_xy->enable();
        trans_x->enable();
        trans_y->disable();

        view_rot->reset();
        GLUI_Master.sync_live_all();
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    reset(0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 1020);
    glutInitWindowPosition(450, 50);

    mainWindow = glutCreateWindow("60191798 김영찬");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // GLUI 윈도우 초기화
    glutInitWindowPosition(200, 100);
    glui = GLUI_Master.create_glui("Settings");
    glui->set_main_gfx_window(mainWindow);

    GLUI_Panel* name = new GLUI_Panel(glui, "Name");
    GLUI_StaticText* statictext = new GLUI_StaticText(name,"60191798 Kim YeongChan");

    GLUI_Separator* separator = new GLUI_Separator(glui);

    // 체크박스
    GLUI_Panel* checkboxes = new GLUI_Panel(glui, "WireMode");
    checkbox_wireframe = new GLUI_Checkbox(checkboxes, "Wire Frame", &wireFrame, 0, checkboxCallback);
    // 색깔
    GLUI_Rollout* color = new GLUI_Rollout(glui, "Colors", true);
    GLUI_Panel* light0 = new GLUI_Panel(color, "Light");

    GLUI_Spinner* spinner_light_r, * spinner_light_g, * spinner_light_b;
    spinner_light_r =  new GLUI_Spinner(light0, "Red:", &lightDiffuse[0], 1, spinnerCallback);
    spinner_light_g = new GLUI_Spinner(light0, "Green:", &lightDiffuse[1], 1, spinnerCallback);
    spinner_light_b = new GLUI_Spinner(light0, "Blue:", &lightDiffuse[2], 1, spinnerCallback);
    spinner_light_r->set_float_limits(0, 1);
    spinner_light_g->set_float_limits(0, 1);
    spinner_light_b->set_float_limits(0, 1);
    spinner_light_r->set_alignment(GLUI_ALIGN_RIGHT);
    spinner_light_g->set_alignment(GLUI_ALIGN_RIGHT);
    spinner_light_b->set_alignment(GLUI_ALIGN_RIGHT);

    glui2->add_column_to_panel(light0, false);

    scrollbar_light_r = new GLUI_Scrollbar(light0, "Red", GLUI_SCROLL_HORIZONTAL, &lightDiffuse[0], 1, scrollbarCallback);
    scrollbar_light_g = new GLUI_Scrollbar(light0, "Green", GLUI_SCROLL_HORIZONTAL, &lightDiffuse[1], 1, scrollbarCallback);
    scrollbar_light_b = new GLUI_Scrollbar(light0, "Blue", GLUI_SCROLL_HORIZONTAL, &lightDiffuse[2], 1, scrollbarCallback);
    scrollbar_light_r->set_float_limits(0, 1);
    scrollbar_light_g->set_float_limits(0, 1);
    scrollbar_light_b->set_float_limits(0, 1);

    GLUI_Panel* panel_car_color = new GLUI_Panel(color, "Car Color");

    GLUI_EditText* edittext_car_r, * edittext_car_g, * edittext_car_b;
    edittext_car_r = new GLUI_EditText(panel_car_color, "Red", GLUI_EDITTEXT_FLOAT, &carColor[0], 2, edittextCallback);
    edittext_car_g = new GLUI_EditText(panel_car_color, "Green", GLUI_EDITTEXT_FLOAT, &carColor[1], 2, edittextCallback);
    edittext_car_b = new GLUI_EditText(panel_car_color, "Blue", GLUI_EDITTEXT_FLOAT, &carColor[2], 2, edittextCallback);
    edittext_car_r->set_float_limits(0, 1);
    edittext_car_g->set_float_limits(0, 1);
    edittext_car_b->set_float_limits(0, 1);

    glui2->add_column_to_panel(panel_car_color, false);

    scrollbar_car_r = new GLUI_Scrollbar(panel_car_color, "Red", GLUI_SCROLL_HORIZONTAL, &carColor[0], 2, scrollbarCallback);
    scrollbar_car_g = new GLUI_Scrollbar(panel_car_color, "Green", GLUI_SCROLL_HORIZONTAL, &carColor[1], 2, scrollbarCallback);
    scrollbar_car_b = new GLUI_Scrollbar(panel_car_color, "Blue", GLUI_SCROLL_HORIZONTAL, &carColor[2], 2, scrollbarCallback);
    scrollbar_car_r->set_float_limits(0, 1);
    scrollbar_car_g->set_float_limits(0, 1);
    scrollbar_car_b->set_float_limits(0, 1);

    scrollbar_car_r->set_float_val(car_colors[selected_car].r);
    scrollbar_car_g->set_float_val(car_colors[selected_car].g);
    scrollbar_car_b->set_float_val(car_colors[selected_car].b);

    new GLUI_Button(glui, "Reset", 1, reset);
    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(mainWindow);
    // GLUI2 ------------------------------------------------------------------------------------------------------------
    glutInitWindowPosition(100, 720);

    glui2 = GLUI_Master.create_glui("Joy Stick");
    glui2->set_main_gfx_window(mainWindow);
    // 라디오 버튼
    GLUI_Panel* carSelecter = glui2->add_panel("Car Selecter");
    listbox = glui2->add_listbox_to_panel(carSelecter, "" , &selected_car, 0, listboxnCallback);
    listbox->add_item(0, "Car 1[-]");
    listbox->add_item(1, "Car 2[-]");
    listbox->add_item(2, "Car 3[-]");
    listbox->add_item(3, "Car 4[ | ]");
    listbox->add_item(4, "Car 5[ | ]");
    listbox->add_item(5, "Car 6[ | ]");
    listbox->add_item(6, "Car 7[ | ]");
    radioGroup = glui2->add_radiogroup_to_panel(carSelecter, &selected_car, 0, radioButtonCallback);
    glui2->add_radiobutton_to_group(radioGroup, "Car 1[-]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 2[-]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 3[-]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 4[ | ]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 5[ | ]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 6[ | ]");
    glui2->add_radiobutton_to_group(radioGroup, "Car 7[ | ]");
    glui2->add_column_to_panel(carSelecter, false);

    view_rot = new GLUI_Rotation(glui2, "View Rotation", view_rotate, 0, rotationCallback);
    view_rot->set_spin(1.0);
    new GLUI_Column(glui2, false);

    GLUI_Panel* transition = new GLUI_Panel(glui2, "Transition");
     trans_xy =
        new GLUI_Translation(transition, "Objects XY", GLUI_TRANSLATION_XY, car_pos, 0, translationCallback);
    trans_xy->set_speed(.05);

     trans_x =
        new GLUI_Translation(transition, "Objects X", GLUI_TRANSLATION_X, car_pos, 0, translationCallback);
    trans_x->set_speed(.05);
    new GLUI_Column(transition, false);
     trans_y =
        new GLUI_Translation(transition, "Objects Y", GLUI_TRANSLATION_Y, &car_pos[1], 0, translationCallback);
    trans_y->set_speed(.05);
    trans_y->disable();
    
    trans_z =
        new GLUI_Translation(transition, "Objects Z", GLUI_TRANSLATION_Z, &car_pos[2]);
    trans_z->set_speed(.05);

    new GLUI_Button(glui2, "Solution", 1, buttonCallback);

    GLUI_Master.set_glutIdleFunc(myGlutIdle);

    LightInit();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}
