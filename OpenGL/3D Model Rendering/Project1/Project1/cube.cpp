/*
	�ۼ��� : �̹ο�
	�ۼ��� : 20190531
	���α׷�: opengl �� �̿��� �� �׸���
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include "glut.h"

#define VERTEXNUM 2904 // �� ����
#define POLYNUM 5804 // �� ����

#define FILENUM "cow.obj" 

using namespace std;

float v[VERTEXNUM][3]; // vertex
int face[POLYNUM][3]; // face

float point_light[4][4];
float dir_light[4][4];
float spot_light[7][3];
float material[4][4];

float FaceNomalVector[POLYNUM][3] = { 0.0 };
float VertexNomalVector[VERTEXNUM][3] = { 0.0 };

float y_angle = 0.01;

void ProVec();
void Cross_Product(float a[3], float b[3], int i);
void VerNomalVect();
void MyLight();
void  MyMe();
int how = 3;
FILE* LineDelete(FILE *fp, int num) // ���� ����
{
	char buf[100];

	for (int i = 0; i < num; i++)
		fgets(buf, sizeof(buf), fp);
	return fp;
}

void light(FILE *fp) // ����Ʈ ���� �޾ƿ��� �Լ� 
{
	char buf[100];
	char temp[6][3][100];

	fp = LineDelete(fp, 1);

	//point light
	for (int i = 0; i < 4; i++) {
		fscanf(fp, "%s %s %s\n", temp[i][0], temp[i][1], temp[i][2]); // ���ڿ��� �޾� 

		for (int j = 0; j < 3; j++)
		{
			point_light[i][j] = atof(temp[i][j]); // �Ǽ��� ��ȯ 
		}
		point_light[i][3] = 1;

	}

	fp = LineDelete(fp, 1);


	// dir light ���������� ���ڿ��� ��������� �޾Ƽ� �Ǽ��� ��ȯ 
	for (int i = 0; i < 4; i++) {
		fscanf(fp, "%s %s %s\n", temp[i][0], temp[i][1], temp[i][2]);

		for (int j = 0; j < 3; j++)
		{
			dir_light[i][j] = atof(temp[i][j]);
		}
		dir_light[i][3] = 1;

	}
	dir_light[0][3] = 0; // dir_light �� ������ ���ڴ� 0 �̴� �̰����� point �� ���еȴ�.

	fp = LineDelete(fp, 1);


	// spot light �������� 
	for (int i = 0; i < 6; i++) {
		if (i != 2) {
			fscanf(fp, "%s %s %s\n", temp[i][0], temp[i][1], temp[i][2]);

			for (int j = 0; j < 3; j++)
			{
				spot_light[i][j] = atof(temp[i][j]);
			}
		}
		else {
			fscanf(fp, "%s\n", temp[i][0]);
			spot_light[i][0] = atof(temp[i][0]);
		}
	}
	fscanf(fp, "%s\n", &temp[0][0][0]);
	spot_light[6][0] = atof(temp[0][0]);

	fp = LineDelete(fp, 1);


	// meterial ���� ���� ���ڿ��� �޾� �Ǽ��� ��ȯ
	for (int i = 0; i < 3; i++) {
		fscanf(fp, "%s %s %s %s\n", temp[i][0], temp[i][1], temp[i][2], temp[i][3]);
		for (int j = 0; j < 4; j++)
		{
			material[i][j] = atof(temp[i][j]);
		}

	}
	fscanf(fp, "%s\n", temp[0][0]);
	material[3][0] = atof(temp[0][0]);

}

void readObj(FILE *fp) // obj ���� �о���� �Լ� 
{

	char ch = 0;
	char temp[POLYNUM][3][20];

	fp = LineDelete(fp, 11);


	// �� ���� �Է¹��� 
	for (int i = 0; i < VERTEXNUM; i++) {
		fscanf(fp, "%c %s %s %s\n", &ch, temp[i][0], temp[i][1], temp[i][2]);

		v[i][0] = atof(temp[i][0]);
		v[i][1] = atof(temp[i][1]);
		v[i][2] = atof(temp[i][2]);
	}

	fp = LineDelete(fp, 2);
	 // �� ���� �Է¹��� 
	for (int i = 0; i < POLYNUM; i++) {
		fscanf(fp, "%c %s %s %s\n", &ch, temp[i][0], temp[i][1], temp[i][2]);

		face[i][0] = atoi(temp[i][0]);
		face[i][1] = atoi(temp[i][1]);
		face[i][2] = atoi(temp[i][2]);
	}
	ProVec();
}

void ProVec() // ���� �̿��Ͽ� ���͸� ���ϴ� �Լ� 
{
	float vec[2][3];
	for (int i = 0; i < POLYNUM; i++)
	{
		// 2��° ������ 1��° ���� �����ν� ���� ����
		vec[0][0] = v[face[i][1] - 1][0] - v[face[i][0] - 1][0]; 
		vec[0][1] = v[face[i][1] - 1][1] - v[face[i][0] - 1][1];
		vec[0][2] = v[face[i][1] - 1][2] - v[face[i][0] - 1][2];


		// 3��° ������ 1��° ���� �����ν� ���� ����
		vec[1][0] = v[face[i][2] - 1][0] - v[face[i][0] - 1][0];
		vec[1][1] = v[face[i][2] - 1][1] - v[face[i][0] - 1][1];
		vec[1][2] = v[face[i][2] - 1][2] - v[face[i][0] - 1][2];

		Cross_Product(vec[0], vec[1], i); // ���� ���ϴ� �Լ� ȣ�� 
	}
	VerNomalVect();
}

void Cross_Product(float a[3], float b[3], int i) // ������ ���ϴ� �Լ�
{
	// ���� ������ �̿��Ͽ� ��� 
	FaceNomalVector[i][0] = a[1] * b[2] - a[2] * b[1];
	FaceNomalVector[i][1] = a[2] * b[0] - a[0] * b[2];
	FaceNomalVector[i][2] = a[0] * b[1] - a[1] * b[0];


	// ���� ���ͷ� �������  �� ���Ҹ� ũ��� �����ָ� �ȴ�. 
	float distance = sqrt(pow(FaceNomalVector[i][0], 2) + pow(FaceNomalVector[i][1], 2) + pow(FaceNomalVector[i][2], 2));
	FaceNomalVector[i][0] /= distance;
	FaceNomalVector[i][1] /= distance;
	FaceNomalVector[i][2] /= distance;
}


void VerNomalVect() // vertex nomal vector ����� �Լ� 
{
	float distance;
	for (int i = 0; i < VERTEXNUM; i++)
	{
		for (int j = 0; j < POLYNUM; j++)
		{
			if (face[j][0] == (i + 1) || face[j][1] == (i + 1) || face[j][2] == (i + 1)) // ��鿡 ���ԵǾ����� 
			{
				VertexNomalVector[i][0] += FaceNomalVector[j][0]; // ���Ե� ��� ���� ������ �� 
				VertexNomalVector[i][1] += FaceNomalVector[j][1];
				VertexNomalVector[i][2] += FaceNomalVector[j][2];

			}
		}
		// �������� ���� 
		distance = sqrt(pow(VertexNomalVector[i][0], 2) + pow(VertexNomalVector[i][1], 2) + pow(VertexNomalVector[i][2], 2));
		VertexNomalVector[i][0] /= distance;
		VertexNomalVector[i][1] /= distance;
		VertexNomalVector[i][2] /= distance;
	}
}

void MyReshape(int w, int h) {

	glLoadIdentity(); // �ʱ�ȭ
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); // ���� 
	glOrtho(-2.0, 2.0, -2.0, 2.0, -100.0, 100.0); // ���� 
	glMatrixMode(GL_MODELVIEW);

}


void FlatDisplay() // Flat shading
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	MyLight(); // �� ����
	MyMe(); // �������� 
	glScalef(1.0, 2.0, 1.0);
	
	glRotatef(y_angle, 0.0, 1.0, 0.0); // ȸ�� 

	glShadeModel(GL_FLAT);
	for (int i = 0; i < POLYNUM; i++)
	{
		glBegin(GL_POLYGON);
		glNormal3f(FaceNomalVector[i][0], FaceNomalVector[i][1], FaceNomalVector[i][2]); // ��� �� �ϳ��� ��������
		for (int j = 0; j < 3; j++)
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		glEnd();
	}
	glutSwapBuffers();
}

void GourDisplay() // �׷ο� ���̵� 
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	MyLight();
	MyMe();
	glScalef(1.0, 2.0, 1.0);
	
	glRotatef(y_angle, 0.0, 1.0, 0.0);

	glShadeModel(GL_SMOOTH); 
	for (int i = 0; i < POLYNUM; i++)
	{
		glBegin(GL_POLYGON);
		for (int j = 0; j < 3; j++) {
			glNormal3f(VertexNomalVector[face[i][j] - 1][0], VertexNomalVector[face[i][j] - 1][1], VertexNomalVector[face[i][j] - 1][2]); // ���� �ϳ��� ��������
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		}
		glEnd();
	}

	glutSwapBuffers();
}

void WireDisplay() // ���̾� ������ 
{
	/*
	 glEnable(GL_DEPTH_TEST) : ���� �˻�

	��ü�� �ϳ� �׸��� �� �������� ��ü�� �ϳ� �� �׸��� ó���� �׷ȴ� ��ü�� ���߿� �׸�

	��ü�� ������ ������ ���� �� �ִ´�.

	�̶� ����ϴ� �Լ��� glEnableGL_DEPTH_TEST) �̴�. �� �Լ��� ����ϸ� ���������� ��

	�ʿ� �ִ� ��ü�� ���ʿ� �ִ� ��ü�� ������ �� ���̴�.
	*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	MyLight();
	MyMe();
	glScalef(1.0, 2.0, 1.0);
	
	glRotatef(y_angle, 0.0, 1.0, 0.0);

	for (int i = 0; i < POLYNUM; i++)
	{
		glBegin(GL_LINE_LOOP); // Line loop �θ� 
		for (int j = 0; j < 3; j++)
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		glEnd();
	}
	glutSwapBuffers();
}

void MyIdle() // ȸ�� 
{

	y_angle += 0.3; // 0.01 �� ȸ�� 
	glutPostRedisplay();
}


void MyMe() // ����
{

	glMaterialfv(GL_FRONT, GL_AMBIENT, material[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[1]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material[2]);
	glMaterialf(GL_FRONT, GL_SHININESS, material[3][0]);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}
void MyLight() // �� Ű���忡 ���� ���� �����ϴ� ���� �޶��� 
{
	glEnable(GL_LIGHTING);

	if (how == 1) // point
	{
		glLightfv(GL_LIGHT0, GL_POSITION, point_light[0]);
		glLightfv(GL_LIGHT0, GL_AMBIENT, point_light[1]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, point_light[2]);
		glLightfv(GL_LIGHT0, GL_SPECULAR, point_light[3]);


		// ���� ��ø�� �� ��������
		// �������� ���ִ� �۾��� �ؾ��Ѵ�.
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);
	}
	else if (how == 2) // dir 
	{
		glLightfv(GL_LIGHT1, GL_POSITION, dir_light[0]);
		glLightfv(GL_LIGHT1, GL_AMBIENT, dir_light[1]);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, dir_light[2]);
		glLightfv(GL_LIGHT1, GL_SPECULAR, dir_light[3]);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT1);
	}
	else if (how == 3) // spot
	{
		float position[] = { spot_light[0][0],spot_light[0][1],spot_light[0][2],1.0 };
		float ambient[] = { spot_light[3][0],spot_light[3][1],spot_light[3][2],1.0 };
		float diffuse[] = { spot_light[4][0],spot_light[4][1],spot_light[4][2],1.0 };
		float specular[] = { spot_light[5][0],spot_light[5][1],spot_light[5][2],1.0 };


		glLightfv(GL_LIGHT2, GL_POSITION, position);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient); // �ֺ��� 
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse); // ���ݻ�
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular); // ���ݻ�

		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_light[1]);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spot_light[2][0]);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spot_light[6][0]);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}



}


void MyKey(unsigned char key, int x, int y) // ���� ��ȯ
{
	switch (key) // ���� Ű�� f g h �϶� ���̵� ��� ��ȯ   1 2 3 �϶� ����Ʈ ��� ��ȯ 
	{
	case 'F':
	case 'f':
		glutDisplayFunc(FlatDisplay);
		break;
	case 'G':
	case 'g':
		glutDisplayFunc(GourDisplay);
		break;
	case 'H':
	case 'h':
		glutDisplayFunc(WireDisplay);
		break;
	case '1':
		how = 1;
		break;
	case '2':
		how = 2;
		break;
	case '3':
		how = 3;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char ** argv) // main 
{

	FILE *fp = fopen(argv[1], "r"); // cmd â���� ����
	//FILE *fp = fopen(FILENUM, "r");
	FILE *fp2 = fopen("light_material.txt", "r");
	readObj(fp);
	light(fp2);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(400, 200);
	glutInitWindowSize(800, 400);
	glutCreateWindow("LIGHTING");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glutDisplayFunc(FlatDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKey);
	glutIdleFunc(MyIdle);
	glutMainLoop();
	return 0;
}