
// ParticleProjectView.cpp: CParticleProjectView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ParticleProject.h"
#endif

#include "ParticleProjectDoc.h"
#include "ParticleProjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CParticleProjectView

IMPLEMENT_DYNCREATE(CParticleProjectView, CView)

BEGIN_MESSAGE_MAP(CParticleProjectView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CParticleProjectView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SPOTLIGHT, &CParticleProjectView::OnSpotlight)
	ON_COMMAND(ID_POSITIONAL, &CParticleProjectView::OnPositional)
END_MESSAGE_MAP()

BOOL CParticleProjectView::SetDevicePixelFormat(HDC hdc) {
	int pixelformat;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//size of this pfd
		1,	//default version
		PFD_DRAW_TO_WINDOW |	//support window
		PFD_SUPPORT_OPENGL |	// support OpenGL
		PFD_GENERIC_FORMAT |
		PFD_DOUBLEBUFFER,		// double buffered 하나가 보여주는 동안 나머지 하나는 그리고, 다 그려지면 buffer swap
		PFD_TYPE_RGBA,			// RGBA type
		32,	// 32 bit color depth (rgba를 각 8bit씩)
		0, 0, 0, 0, 0, 0,	// color bits ignored
		8,	// no aplha buffer
		0,	// shift bit ignored
		8,	// no accumulation buffer
		0, 0, 0, 0,	// accum bits ignored
		16,	// 16 bit z buffer
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};	// 그림 그리는 화면의 정보를 setting

	// MFC Device Context 선택작업
	if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE) {
		MessageBox(LPCTSTR("ChoosePixelFormat failed"), LPCTSTR("Error"), MB_OK);
		return FALSE;
	}

	if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) {
		MessageBox(LPCTSTR("ChoosePixelFormat failed"), LPCTSTR("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}


// GL 초기화 함수
void CParticleProjectView::InitGL(GLvoid) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // 윈도우 배경색 설정
	glClearDepth(1.0f); // z buffer 값 초기화
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vector p(0.0f, 3.0f, 10.0f);
	Vector f(0.0f, 0.0f, -1.0f);
	Vector u(0.0f, 1.0f, 0.0f);

	cameraPos = p;
	cameraFront = f;
	cameraUp = u;

	firstMouse = mouseMove = TRUE;
	
	m_posLight = m_spotLight = FALSE;

	yaw = -90.0f;
	pitch = 0.0f;

	m_particle.Init();


	glEnable(GL_TEXTURE_2D);

	glGenTextures(5, m_Textures);
	loadTexture("../res/top.bmp", &m_Textures[0]);
	loadTexture("../res/front.bmp", &m_Textures[1]);
	loadTexture("../res/back.bmp", &m_Textures[2]);
	loadTexture("../res/left.bmp", &m_Textures[3]);
	loadTexture("../res/right.bmp", &m_Textures[4]);

}

// 화면 크기 바뀔 때마다 다시 계산
void CParticleProjectView::ReSizeGLScene(GLsizei width, GLsizei height) {
	// 0으로 나누면 안됨 (시야각 계산시)
	if (height == 0) height = 1;

	lastX = width / 2;
	lastY = height / 2;

	Width = width;
	Height = height;

	glViewport(0, 0, width, height); // 그림이 그려지는 영역의 픽셀단위 크기 재설정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 윈도우 시야각 계산 (중요)
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f); // 45.0f = 수직화각 (카메라가 위아래로 45도만큼 볼 수 있음), 수평화각 = 폭 / 높이, 앞에서 자르는 단위, 뒤에서 자르는 단위 (meter 단위로 생각)

	// 모델뷰 매트릭스 설정
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

Vector CParticleProjectView::rotateVector(float angle, int x, int y, int z, Vector& v) {
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	float R[3][3];

	R[0][0] = ((1 - cosTheta) * x * x + cosTheta);
	R[0][1] = ((1 - cosTheta) * x * y - sinTheta * z);
	R[0][2] = ((1 - cosTheta) * x * z + sinTheta * y);

	R[1][0] = ((1 - cosTheta) * x * y + sinTheta * z);
	R[1][1] = ((1 - cosTheta) * y * y + cosTheta);
	R[1][2] = ((1 - cosTheta) * y * z - sinTheta * x);

	R[2][0] = ((1 - cosTheta) * x * z - sinTheta * y);
	R[2][1] = ((1 - cosTheta) * y * z + sinTheta * x);
	R[2][2] = ((1 - cosTheta) * z * z + cosTheta);

	Vector result;
	result.x = R[0][0] * v.x + R[0][1] * v.y + R[0][2] * v.z;
	result.y = R[1][0] * v.x + R[1][1] * v.y + R[1][2] * v.z;
	result.z = R[2][0] * v.x + R[2][1] * v.y + R[2][2] * v.z;

	return result;
}

GLuint loadBMP_custom(const char* imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file = fopen(imagepath, "rb");
	if (!file) { printf("Image could not be opened\n"); return 0; }

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*) & (header[0x0A]);
	imageSize = *(int*) & (header[0x22]);
	width = *(int*) & (header[0x12]);
	height = *(int*) & (header[0x16]);

	// 몇몇 BMP 파일들은 포맷이 잘못되었습니다. 정보가 누락됬는지 확인해봅니다. 
// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// 버퍼 생성
	data = new unsigned char[imageSize];

	// 파일에서 버퍼로 실제 데이터 넣기. 
	fread(data, 1, imageSize, file);

	//이제 모두 메모리 안에 있으니까, 파일을 닫습니다. 
	//Everything is in memory now, the file can be closed
	fclose(file);

	// OpenGL Texture를 생성합니다. 
	GLuint textureID;
	glGenTextures(1, &textureID);

	// 새 텍스처에 "Bind" 합니다 : 이제 모든 텍스처 함수들은 이 텍스처를 수정합니다. 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// OpenGL에게 이미지를 넘겨줍니다. 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}

GLuint CParticleProjectView::loadTextureBMP(const char* filename) {
	GLuint texture;

	unsigned char* data;

	FILE* file = fopen(filename, "rb");

	if (file == NULL) return 0;


	data = (unsigned char*)malloc(Width * Height * 3);
	//int size = fseek(file,);
	fread(data, Width * Height * 3, 1, file);
	fclose(file);

	for (int i = 0; i < Width * Height; ++i)
	{
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];

		data[index] = R;
		data[index + 2] = B;

	}


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}

void CParticleProjectView::loadTexture(char* file, GLuint* p_texture) {
	FILE* p_File = fopen(file, "r");

	if (p_File) {
		AUX_RGBImageRec* p_Bitmap = auxDIBImageLoad((LPCWSTR)file);

		glBindTexture(GL_TEXTURE_2D, *p_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p_Bitmap->sizeX, p_Bitmap->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, p_Bitmap->data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		fclose(p_File);
	}
	return;
}

void CParticleProjectView::DrawSkyBox(float width, float height, float length) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3f(0.4, 0.4, 0.7);

	glBindTexture(GL_TEXTURE_2D, m_Textures[0]); // top
	glBegin(GL_QUADS);
	// glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0f, 0.0f); glVertex3f(width, height, -length); // Top Right Of The Quad (Top)
	glTexCoord2d(1.0f, 1.0f); glVertex3f(-width, height, -length); // Top Left Of The Quad (Top)
	glTexCoord2d(0.0, 1.0f); glVertex3f(-width, height, length); // Bottom Left Of The Quad (Top)
	glTexCoord2d(0.0, 0.0f); glVertex3f(width, height, length); // Bottom Right Of The Quad (Top)
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_Textures[1]); // front
	glBegin(GL_QUADS);
	// glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0f, 0.0f); glVertex3f(width, height, length); // Top Right Of The Quad (Front)
	glTexCoord2d(1.0f, 0.0f); glVertex3f(-width, height, length); // Top Left Of The Quad (Front)
	glTexCoord2d(1.0f, 1.0f); glVertex3f(-width, -height, length); // Bottom Left Of The Quad (Front)
	glTexCoord2d(0.0f, 1.0f); glVertex3f(width, -height, length); // Bottom Right Of The Quad (Front)
	glEnd();

	// back
	glBindTexture(GL_TEXTURE_2D, m_Textures[2]);
	glBegin(GL_QUADS);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0f, 1.0f); glVertex3f(width, -height, -length); // Bottom Left Of The Quad (Back)
	glTexCoord2d(1.0f, 1.0f); glVertex3f(-width, -height, -length); // Bottom Right Of The Quad (Back)
	glTexCoord2d(1.0f, 0.0f); glVertex3f(-width, height, -length); // Top Right Of The Quad (Back)
	glTexCoord2d(0.0f, 0.0f); glVertex3f(width, height, -length); // Top Left Of The Quad (Back)
	glEnd();

	// left
	glBindTexture(GL_TEXTURE_2D, m_Textures[3]);
	glBegin(GL_QUADS);
	// glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0f, 0.0f); glVertex3f(-width, height, -length); // Top Right Of The Quad (Left)
	glTexCoord2d(1.0f, 1.0f); glVertex3f(-width, -height, -length); // Top Left Of The Quad (Left)
	glTexCoord2d(0.0f, 1.0f); glVertex3f(-width, -height, length); // Bottom Left Of The Quad (Left)
	glTexCoord2d(0.0f, 0.0f); glVertex3f(-width, height, length); // Bottom Right Of The Quad (Left)
	glEnd();

	// right
	glBindTexture(GL_TEXTURE_2D, m_Textures[4]);
	glBegin(GL_QUADS);
	// glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0f, 0.0f); glVertex3f(width, height, -length); // Top Right Of The Quad (Right)
	glTexCoord2d(1.0f, 0.0f); glVertex3f(width, height, length); // Top Left Of The Quad (Right)
	glTexCoord2d(1.0f, 1.0f); glVertex3f(width, -height, length); // Bottom Left Of The Quad (Right)
	glTexCoord2d(0.0f, 1.0f); glVertex3f(width, -height, -length); // Bottom Right Of The Quad (Right)
	glEnd();
}

void CParticleProjectView::DrawCube() {
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);

	glColor3f(0, 1, 1);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);

	glColor3f(0.5, 0.5, 1);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);

	glColor3f(0.5, 0.5, 1);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);

	glColor3f(1, 1, 0);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);

	glColor3f(1, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glEnd();
}

void CParticleProjectView::DrawSphere(float r) {
	GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
	GLfloat radius = r;
	int gradation = 20;

	for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
		{
			x = radius * cos(beta) * sin(alpha);
			y = radius * sin(beta) * sin(alpha);
			z = radius * cos(alpha);
			glVertex3f(x, y, z);
			x = radius * cos(beta) * sin(alpha + PI / gradation);
			y = radius * sin(beta) * sin(alpha + PI / gradation);
			z = radius * cos(alpha + PI / gradation);
			glVertex3f(x, y, z);
		}
		glEnd();
	}
}


// draw 함수
// MFC가 자동으로 부르는 onDraw에서 호출되어 그림 그림
void CParticleProjectView::DrawGLScene(void) {
	// screen이랑 depth buffer 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glViewport(0, 0, Width, Height); // 그림이 그려지는 영역의 픽셀단위 크기 재설정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 윈도우 시야각 계산 (중요)
	gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 1000.0f); // 45.0f = 수직화각 (카메라가 위아래로 45도만큼 볼 수 있음), 수평화각 = 폭 / 높이, 앞에서 자르는 단위, 뒤에서 자르는 단위 (meter 단위로 생각)

	// 모델뷰 매트릭스 설정
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 카메라 위치(x, y, z), 카메라가 바라보는 점의 위치(x, y, z), 카메라의 회전상태(roll) (vector)
	Vector look = cameraPos + cameraFront;
	gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, look.x, look.y, look.z, cameraUp.x, cameraUp.y, cameraUp.z);
	// z : 카메라 앞 뒤, y : 높이
	
	if (m_posLight == TRUE || m_spotLight == TRUE) {
		glEnable(GL_LIGHTING);
		GLfloat light1_ambient[] = { 0.5, 0.7, 0.3, 1.0 };
		GLfloat light1_diffuse[] = { 0.5, 0.5, 0.9, 1.0 };
		GLfloat light1_position[] = { 3.0, 7.0, 10.0, 1.0 }; // x,y,z,w w=1이면 positional
		GLfloat light1_specular[] = { 0.9, 0.9, 0.9, 1.0 };
		GLfloat light1_shiness[] = { 50.0 };

		GLfloat spot_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_diffuse[] = { 0.8, 0.8, 0.9, 1.0 };
		GLfloat spot_specular[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_position[] = { 7.0, 7.0, 10.0, 1.0 }; // spot light는 positional light의 특별한 경우! w = 1 해줘야했음
		GLfloat spot_direction[] = { -1.0, -1.0, 1.0 };
		if (m_posLight == TRUE) {
			glEnable(GL_LIGHT1);

			glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
			glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
			glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
			glLightfv(GL_LIGHT1, GL_SHININESS, light1_shiness);
		}

		if (m_spotLight == TRUE) {
			glEnable(GL_LIGHT2);

			glLightfv(GL_LIGHT2, GL_POSITION, spot_position);
			glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
			glLightfv(GL_LIGHT2, GL_AMBIENT, spot_ambient);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, spot_diffuse);
			glLightfv(GL_LIGHT2, GL_SPECULAR, spot_specular);
			glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10.0);
			glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 10.0);
		}

		GLfloat mat_ambient[] = { 0.3, 0.2, 0.8, 1.0 };
		GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_shiness[] = { 30.0 };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiness);

	}

	glPushMatrix();
	glScalef(1.5, 1.5, 1.5);
	m_particle.DrawParticle();
	glPopMatrix();

	DrawSphere(1.5f);

	glPushMatrix();
	glTranslatef(0.0, 2.5, 0.0);
	DrawSphere(1.0f);
	glPopMatrix();



	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	// 버퍼 스왑 MFC 함수
	SwapBuffers(m_hDC);
}

// CParticleProjectView 생성/소멸

CParticleProjectView::CParticleProjectView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CParticleProjectView::~CParticleProjectView()
{
}

BOOL CParticleProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CParticleProjectView 그리기

void CParticleProjectView::OnDraw(CDC* /*pDC*/)
{
	CParticleProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	DrawGLScene();

	Invalidate(FALSE);
}


// CParticleProjectView 인쇄


void CParticleProjectView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CParticleProjectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CParticleProjectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CParticleProjectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CParticleProjectView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CParticleProjectView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CParticleProjectView 진단

#ifdef _DEBUG
void CParticleProjectView::AssertValid() const
{
	CView::AssertValid();
}

void CParticleProjectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CParticleProjectDoc* CParticleProjectView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CParticleProjectDoc)));
	return (CParticleProjectDoc*)m_pDocument;
}
#endif //_DEBUG


// CParticleProjectView 메시지 처리기


void CParticleProjectView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ReSizeGLScene(cx, cy);
}


int CParticleProjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_hDC = GetDC()->m_hDC;

	// set the pixel format
	if (!SetDevicePixelFormat(m_hDC)) return -1;

	// create rendering context and make it current
	m_hglRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hglRC);

	// initialize renderring mode
	InitGL();

	return 0;
}


void CParticleProjectView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	float cameraSpeed = 0.5f; // adjust accordingly

	switch (nChar) {

	case 0x41:
	{
		Vector right = cameraFront.crossProduct(cameraUp);
		cameraPos -= right.normalize() * cameraSpeed;
		break;
	}

	case 0x44:
	{
		Vector right = cameraFront.crossProduct(cameraUp);
		cameraPos += right.normalize() * cameraSpeed;
		break;
	}

	case 0x57:
	{
		cameraPos += cameraFront * cameraSpeed;
		break;
	}

	case 0x53:
	{
		cameraPos -= cameraFront * cameraSpeed;
		break;
	}
	
	case VK_TAB: {
		m_particle.b_tab = !m_particle.b_tab;
	}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

float CParticleProjectView::radians(float degree) {
	return (degree * M_PI) / 180;
}


void CParticleProjectView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);

	if (!mouseMove) return;

	if (firstMouse)
	{
		lastX = point.x;
		lastY = point.y;
		firstMouse = false;
	}

	float xOffset = point.x - lastX;
	float yOffset = lastY - point.y; // y 좌표의 범위는 밑에서부터 위로가기 때문에 반대로 바꿉니다.
	lastX = point.x;
	lastY = point.y;

	float sensitivity = 0.15f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	Vector front;
	front.x = cos(radians(pitch)) * cos(radians(yaw));
	front.y = sin(radians(pitch));
	front.z = cos(radians(pitch)) * sin(radians(yaw));

	cameraFront = front.normalize();

	Vector look = cameraPos + cameraFront;
	gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, look.x, look.y, look.z, cameraUp.x, cameraUp.y, cameraUp.z);
}


void CParticleProjectView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	mouseMove = !mouseMove;
	CView::OnLButtonDown(nFlags, point);

}


BOOL CParticleProjectView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta > 0) {
		cameraPos.z -= 1;
	}
	else {
		cameraPos.z += 1;
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}



void CParticleProjectView::OnSpotlight()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_spotLight = !m_spotLight;
}


void CParticleProjectView::OnPositional()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_posLight = !m_posLight;
}
