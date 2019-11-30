/*
	작성자 : 이민우
	작성일 : 20190531
	프로그램: opengl 을 이용한 소 그리기
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include "glut.h"

#define VERTEXNUM 2904 // 점 개수
#define POLYNUM 5804 // 면 개수

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
FILE* LineDelete(FILE *fp, int num) // 라인 제거
{
	char buf[100];

	for (int i = 0; i < num; i++)
		fgets(buf, sizeof(buf), fp);
	return fp;
}

void light(FILE *fp) // 라이트 정보 받아오는 함수 
{
	char buf[100];
	char temp[6][3][100];

	fp = LineDelete(fp, 1);

	//point light
	for (int i = 0; i < 4; i++) {
		fscanf(fp, "%s %s %s\n", temp[i][0], temp[i][1], temp[i][2]); // 문자열로 받아 

		for (int j = 0; j < 3; j++)
		{
			point_light[i][j] = atof(temp[i][j]); // 실수로 변환 
		}
		point_light[i][3] = 1;

	}

	fp = LineDelete(fp, 1);


	// dir light 마찬가지로 문자열로 공백단위로 받아서 실수로 전환 
	for (int i = 0; i < 4; i++) {
		fscanf(fp, "%s %s %s\n", temp[i][0], temp[i][1], temp[i][2]);

		for (int j = 0; j < 3; j++)
		{
			dir_light[i][j] = atof(temp[i][j]);
		}
		dir_light[i][3] = 1;

	}
	dir_light[0][3] = 0; // dir_light 에 마지막 인자는 0 이다 이것으로 point 와 구분된다.

	fp = LineDelete(fp, 1);


	// spot light 마찬가지 
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


	// meterial 정보 또한 문자열로 받아 실수로 전환
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

void readObj(FILE *fp) // obj 파일 읽어오는 함수 
{

	char ch = 0;
	char temp[POLYNUM][3][20];

	fp = LineDelete(fp, 11);


	// 점 정보 입력받음 
	for (int i = 0; i < VERTEXNUM; i++) {
		fscanf(fp, "%c %s %s %s\n", &ch, temp[i][0], temp[i][1], temp[i][2]);

		v[i][0] = atof(temp[i][0]);
		v[i][1] = atof(temp[i][1]);
		v[i][2] = atof(temp[i][2]);
	}

	fp = LineDelete(fp, 2);
	 // 면 정보 입력받음 
	for (int i = 0; i < POLYNUM; i++) {
		fscanf(fp, "%c %s %s %s\n", &ch, temp[i][0], temp[i][1], temp[i][2]);

		face[i][0] = atoi(temp[i][0]);
		face[i][1] = atoi(temp[i][1]);
		face[i][2] = atoi(temp[i][2]);
	}
	ProVec();
}

void ProVec() // 점을 이용하여 벡터를 구하는 함수 
{
	float vec[2][3];
	for (int i = 0; i < POLYNUM; i++)
	{
		// 2번째 점에서 1번째 점을 뺌으로써 벡터 생성
		vec[0][0] = v[face[i][1] - 1][0] - v[face[i][0] - 1][0]; 
		vec[0][1] = v[face[i][1] - 1][1] - v[face[i][0] - 1][1];
		vec[0][2] = v[face[i][1] - 1][2] - v[face[i][0] - 1][2];


		// 3번째 점에서 1번째 점을 뺌으로써 벡터 생성
		vec[1][0] = v[face[i][2] - 1][0] - v[face[i][0] - 1][0];
		vec[1][1] = v[face[i][2] - 1][1] - v[face[i][0] - 1][1];
		vec[1][2] = v[face[i][2] - 1][2] - v[face[i][0] - 1][2];

		Cross_Product(vec[0], vec[1], i); // 외적 구하는 함수 호출 
	}
	VerNomalVect();
}

void Cross_Product(float a[3], float b[3], int i) // 외적을 구하는 함수
{
	// 외적 공식을 이용하여 계산 
	FaceNomalVector[i][0] = a[1] * b[2] - a[2] * b[1];
	FaceNomalVector[i][1] = a[2] * b[0] - a[0] * b[2];
	FaceNomalVector[i][2] = a[0] * b[1] - a[1] * b[0];


	// 단위 벡터로 만들어줌  각 원소를 크기로 나눠주면 된다. 
	float distance = sqrt(pow(FaceNomalVector[i][0], 2) + pow(FaceNomalVector[i][1], 2) + pow(FaceNomalVector[i][2], 2));
	FaceNomalVector[i][0] /= distance;
	FaceNomalVector[i][1] /= distance;
	FaceNomalVector[i][2] /= distance;
}


void VerNomalVect() // vertex nomal vector 만드는 함수 
{
	float distance;
	for (int i = 0; i < VERTEXNUM; i++)
	{
		for (int j = 0; j < POLYNUM; j++)
		{
			if (face[j][0] == (i + 1) || face[j][1] == (i + 1) || face[j][2] == (i + 1)) // 평면에 포함되었을시 
			{
				VertexNomalVector[i][0] += FaceNomalVector[j][0]; // 포함된 평면 법선 벡터의 함 
				VertexNomalVector[i][1] += FaceNomalVector[j][1];
				VertexNomalVector[i][2] += FaceNomalVector[j][2];

			}
		}
		// 단위백터 생성 
		distance = sqrt(pow(VertexNomalVector[i][0], 2) + pow(VertexNomalVector[i][1], 2) + pow(VertexNomalVector[i][2], 2));
		VertexNomalVector[i][0] /= distance;
		VertexNomalVector[i][1] /= distance;
		VertexNomalVector[i][2] /= distance;
	}
}

void MyReshape(int w, int h) {

	glLoadIdentity(); // 초기화
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); // 투영 
	glOrtho(-2.0, 2.0, -2.0, 2.0, -100.0, 100.0); // 직교 
	glMatrixMode(GL_MODELVIEW);

}


void FlatDisplay() // Flat shading
{

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	MyLight(); // 빛 정보
	MyMe(); // 재질정보 
	glScalef(1.0, 2.0, 1.0);
	
	glRotatef(y_angle, 0.0, 1.0, 0.0); // 회전 

	glShadeModel(GL_FLAT);
	for (int i = 0; i < POLYNUM; i++)
	{
		glBegin(GL_POLYGON);
		glNormal3f(FaceNomalVector[i][0], FaceNomalVector[i][1], FaceNomalVector[i][2]); // 평면 당 하나의 법선벡터
		for (int j = 0; j < 3; j++)
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		glEnd();
	}
	glutSwapBuffers();
}

void GourDisplay() // 그로우 쉐이딩 
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
			glNormal3f(VertexNomalVector[face[i][j] - 1][0], VertexNomalVector[face[i][j] - 1][1], VertexNomalVector[face[i][j] - 1][2]); // 점당 하나의 법선벡터
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		}
		glEnd();
	}

	glutSwapBuffers();
}

void WireDisplay() // 와이어 프레임 
{
	/*
	 glEnable(GL_DEPTH_TEST) : 깊이 검사

	물체를 하나 그리고 그 앞쪽으로 물체를 하나 더 그리면 처음에 그렸던 물체에 나중에 그린

	물체가 가리는 현상이 생길 수 있는다.

	이때 사용하는 함수가 glEnableGL_DEPTH_TEST) 이다. 이 함수를 사용하면 정상적으로 앞

	쪽에 있는 물체가 뒷쪽에 있는 물체를 가리게 될 것이다.
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
		glBegin(GL_LINE_LOOP); // Line loop 로만 
		for (int j = 0; j < 3; j++)
			glVertex3f(v[face[i][j] - 1][0], v[face[i][j] - 1][1], v[face[i][j] - 1][2]);
		glEnd();
	}
	glutSwapBuffers();
}

void MyIdle() // 회전 
{

	y_angle += 0.3; // 0.01 씩 회전 
	glutPostRedisplay();
}


void MyMe() // 재질
{

	glMaterialfv(GL_FRONT, GL_AMBIENT, material[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material[1]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material[2]);
	glMaterialf(GL_FRONT, GL_SHININESS, material[3][0]);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}
void MyLight() // 빛 키보드에 값에 따라 적용하는 빛이 달라짐 
{
	glEnable(GL_LIGHTING);

	if (how == 1) // point
	{
		glLightfv(GL_LIGHT0, GL_POSITION, point_light[0]);
		glLightfv(GL_LIGHT0, GL_AMBIENT, point_light[1]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, point_light[2]);
		glLightfv(GL_LIGHT0, GL_SPECULAR, point_light[3]);


		// 빛이 중첩될 수 있음으로
		// 나머지는 꺼주는 작업을 해야한다.
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
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient); // 주변광 
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse); // 난반사
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular); // 정반사

		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_light[1]);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spot_light[2][0]);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spot_light[6][0]);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}



}


void MyKey(unsigned char key, int x, int y) // 도형 전환
{
	switch (key) // 눌린 키가 f g h 일때 쉐이딩 방법 전환   1 2 3 일때 라이트 방법 전환 
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

	FILE *fp = fopen(argv[1], "r"); // cmd 창에서 실행
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