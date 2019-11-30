/*
	�ۼ��� : �̹ο�
	���α׷� ����: ����ڷ� ���� Ű�� �Է¹޾� ������ ��ȯ�ϴ� ���α׷�
*/
#include "glut.h"
#include <math.h>
#define PI 3.141592
float rot_angle = 0.0; // ȸ��
float ANGLE_COUNT = 5.0; // �ٰ����� �� �� 


void MyDisplay()
{

	double radian = (360.0 / ANGLE_COUNT)*(PI / 180); // math ���̺귯���� �ִ� cos sin �Լ���
	// radian ���� ����ϱ� ������ degree ���� radian ������ ��ȯ���ش�.

	glClear(GL_COLOR_BUFFER_BIT);
	
	//glFrontFace(GL_CCW); �ݽð������ �ո�
	//glCullFace(GL_BACK); �⺻���� GL_BACK 
	//�ո��� ������ �� �ִ��� GL_BACK �� �ĸ��� �����ϰڴٴ� �ǹ� 
	glEnable(GL_CULL_FACE);//GL_CULL_FACE Ȱ��ȭ
	glMatrixMode(GL_MODELVIEW);
	// �𵨺� ����� �ҷ��� �𵨵��� ���ؽ����� ��������
	// ���� �� ��ġ ���� 
	// ������ ���� GL_PROJECTION �� ���� ���� ��Ŀ� ��������
	// ȭ�鿡 ��ġ�� ���
	glLoadIdentity(); // �ʱ�ȭ
	gluLookAt(-3.0, -9, -9, 0.2, 0.2, 0.2, 0.2, 1, 0.2);
	
	glRotatef(rot_angle, 0, 1, 0); // y�� �������� rot_angle ��ŭ ȸ�� 
	
	glColor3f(1.0, 0.0, 0.0);
	
	glBegin(GL_POLYGON); // ���� 
	for (int i = 0; i < ANGLE_COUNT; i++) {
	
		glVertex3f(cos(30 + i * radian), 1, sin(30 + i * radian)); // 30�� �ʱ� ������ ��ġ�� ���Ƿ� ������
		// � ������ �ص� �������.
	}
	glEnd();
	

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POLYGON); // �޸�
	for (int i = 0; i < ANGLE_COUNT; i++) {
		glVertex3f(cos(30 + i * radian), -1, sin(30 + i * radian));
	}
	glEnd();

	double color = 0.1;

	for (int i = 0; i < ANGLE_COUNT; i++) // ����
	{
		glColor3f(color, color, 1-color);

		glBegin(GL_POLYGON); // ������ �׻� �簢
		glVertex3f(cos(30 + i * radian), 1, sin(30 + i * radian));
		
		glVertex3f(cos(30 + i * radian), -1, sin(30 + i * radian));
		
		glVertex3f(cos(30 + (i+1) * radian), -1, sin(30 + (i+1) * radian));
		
		glVertex3f(cos(30 + (i + 1) * radian), 1, sin(30 + (i + 1) * radian));
		glEnd();

		if (ANGLE_COUNT == 12) // 12 �� ����϶� �� ����
			color += 0.08;
		else // 5�� ����϶� �� ���� 
			color += 0.15;
	}
	
	
	glutSwapBuffers();
}
void MyKey(unsigned char key, int x, int y) // ���� ��ȯ
{
	switch (key) // ���� Ű�� q w �϶� ���� ��ȯ 
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
		glutPostRedisplay(); // ��ȯ �� �ٽ� �׸��� 
		break;
	}	
}
void MyIdle() // ȸ�� 
{	
	
	rot_angle += 0.01; // 0.01 �� ȸ�� 
	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(400, 200);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Polygon Rotation");
	glClearColor(1.0, 1.0, 1.0, 1.0); // ������� ����

	glMatrixMode(GL_PROJECTION); // ���� 
	glLoadIdentity(); // �ʱ�ȭ 
	glOrtho(-3.0, 3.0, -3.0, 3.0, -15.0, 15.0); // ���� ũ�� 
	glutDisplayFunc(MyDisplay); //Call Back �Լ� ���
	glutKeyboardFunc(MyKey);
	glutIdleFunc(MyIdle);
	
	glutMainLoop();
}