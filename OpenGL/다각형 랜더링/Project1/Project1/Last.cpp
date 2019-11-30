/*
	작성자 : 이민우
	프로그램 설명: 사용자로 부터 키를 입력받아 도형이 전환하는 프로그램
*/
#include "glut.h"
#include <math.h>
#define PI 3.141592
float rot_angle = 0.0; // 회전
float ANGLE_COUNT = 5.0; // 다각형의 각 수 


void MyDisplay()
{

	double radian = (360.0 / ANGLE_COUNT)*(PI / 180); // math 라이브러리에 있는 cos sin 함수는
	// radian 값을 사용하기 때문에 degree 값을 radian 값으로 변환해준다.

	glClear(GL_COLOR_BUFFER_BIT);
	
	//glFrontFace(GL_CCW); 반시계방향이 앞면
	//glCullFace(GL_BACK); 기본값이 GL_BACK 
	//앞면을 제거할 수 있는지 GL_BACK 은 후면을 제거하겠다는 의미 
	glEnable(GL_CULL_FACE);//GL_CULL_FACE 활성화
	glMatrixMode(GL_MODELVIEW);
	// 모델뷰 행렬을 불러와 모델들의 버텍스들이 곱해져서
	// 최종 모델 위치 생성 
	// 생성한 모델을 GL_PROJECTION 을 통해 투영 행렬에 곱해져서
	// 화면에 위치가 출력
	glLoadIdentity(); // 초기화
	gluLookAt(-3.0, -9, -9, 0.2, 0.2, 0.2, 0.2, 1, 0.2);
	
	glRotatef(rot_angle, 0, 1, 0); // y축 기준으로 rot_angle 만큼 회전 
	
	glColor3f(1.0, 0.0, 0.0);
	
	glBegin(GL_POLYGON); // 윗면 
	for (int i = 0; i < ANGLE_COUNT; i++) {
	
		glVertex3f(cos(30 + i * radian), 1, sin(30 + i * radian)); // 30은 초기 정점의 위치를 임의로 정해줌
		// 어떤 값으로 해도 상관없다.
	}
	glEnd();
	

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POLYGON); // 뒷면
	for (int i = 0; i < ANGLE_COUNT; i++) {
		glVertex3f(cos(30 + i * radian), -1, sin(30 + i * radian));
	}
	glEnd();

	double color = 0.1;

	for (int i = 0; i < ANGLE_COUNT; i++) // 옆면
	{
		glColor3f(color, color, 1-color);

		glBegin(GL_POLYGON); // 옆면은 항상 사각
		glVertex3f(cos(30 + i * radian), 1, sin(30 + i * radian));
		
		glVertex3f(cos(30 + i * radian), -1, sin(30 + i * radian));
		
		glVertex3f(cos(30 + (i+1) * radian), -1, sin(30 + (i+1) * radian));
		
		glVertex3f(cos(30 + (i + 1) * radian), 1, sin(30 + (i + 1) * radian));
		glEnd();

		if (ANGLE_COUNT == 12) // 12 각 기둥일때 색 변경
			color += 0.08;
		else // 5각 기둥일때 색 변경 
			color += 0.15;
	}
	
	
	glutSwapBuffers();
}
void MyKey(unsigned char key, int x, int y) // 도형 전환
{
	switch (key) // 눌린 키가 q w 일때 도형 전환 
	{
	case 'Q':
	case 'q':
	case 'W':
	case 'w':
		if (ANGLE_COUNT == 5.0){
			ANGLE_COUNT = 12.0;
		}
		else{
			ANGLE_COUNT = 5;
		}
		glutPostRedisplay(); // 전환 후 다시 그리기 
		break;
	}	
}
void MyIdle() // 회전 
{	
	
	rot_angle += 0.01; // 0.01 씩 회전 
	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(400, 200);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Polygon Rotation");
	glClearColor(1.0, 1.0, 1.0, 1.0); // 흰색으로 지움

	glMatrixMode(GL_PROJECTION); // 투영 
	glLoadIdentity(); // 초기화 
	glOrtho(-3.0, 3.0, -3.0, 3.0, -15.0, 15.0); // 볼륨 크기 
	glutDisplayFunc(MyDisplay); //Call Back 함수 등록
	glutKeyboardFunc(MyKey);
	glutIdleFunc(MyIdle);
	
	glutMainLoop();
}