
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
	ON_COMMAND(ID_LightOn, &CParticleProjectView::OnLighton)
	ON_COMMAND(ID_LightOff, &CParticleProjectView::OnLightoff)
	ON_COMMAND(ID_THREE_ONE, &CParticleProjectView::OnThreeOne)
	ON_COMMAND(ID_THREE_THREE, &CParticleProjectView::OnThreeThree)
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

	Vector p(0.0f, 2.5f, 10.0f);
	Vector f(0.0f, 0.0f, -1.0f);
	Vector u(0.0f, 1.0f, 0.0f);

	cameraPos = p;
	cameraFront = f;
	cameraUp = u;

	firstMouse = mouseMove = TRUE;
	
	yaw = -90.0f;
	pitch = 0.0f;

	m_particle.Init();

	b_Light = TRUE;

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

	glColor3f(0.8, 0.8, 1.0);
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
	
	if (b_Light == TRUE) {
		glEnable(GL_LIGHTING);

		GLfloat light0_ambient[] = { 0.5, 0.5, 0.9, 1.0 };
		GLfloat light0_diffuse[] = { 0.3, 0.3, 0.9, 1.0 };
		GLfloat light0_position[] = { 0.0, 20.0, 10.0, 0.0 };
		GLfloat light0_specular[] = { 0.8, 0.5, 0.8, 1.0 };
		GLfloat light0_shiness[] = { 25.0 };


		GLfloat light1_ambient[] = { 0.5, 0.7, 0.3, 1.0 };
		GLfloat light1_diffuse[] = { 0.5, 0.5, 0.9, 1.0 };
		GLfloat light1_position[] = { 3.0, 7.0, 10.0, 1.0 }; // x,y,z,w w=1이면 positional
		GLfloat light1_specular[] = { 0.9, 0.9, 0.9, 1.0 };
		GLfloat light1_shiness[] = { 70.0 };

		GLfloat spot_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_diffuse[] = { 0.8, 0.8, 0.9, 1.0 };
		GLfloat spot_specular[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_position[] = { 0.0, 6.0, 0.0, 1.0 }; // spot light는 positional light의 특별한 경우! w = 1 해줘야함
		GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };

		GLfloat spot_ambient2[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_diffuse2[] = { 0.8, 0.8, 0.9, 1.0 };
		GLfloat spot_specular2[] = { 0.7, 0.7, 0.7, 1.0 };
		GLfloat spot_position2[] = { 3.0, 3.0, 0.0, 1.0 }; // spot light는 positional light의 특별한 경우! w = 1 해줘야함
		GLfloat spot_direction2[] = { -1.0, -1.0, 0.0 };




		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
		glLightfv(GL_LIGHT0, GL_SHININESS, light0_shiness);

		glEnable(GL_LIGHT1);

		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
		glLightfv(GL_LIGHT1, GL_SHININESS, light1_shiness);
	
		
		glEnable(GL_LIGHT2);

		glLightfv(GL_LIGHT2, GL_POSITION, spot_position);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
		glLightfv(GL_LIGHT2, GL_AMBIENT, spot_ambient);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, spot_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, spot_specular);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 15.0);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 20.0);

		glEnable(GL_LIGHT3);

		glLightfv(GL_LIGHT3, GL_POSITION, spot_position2);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction2);
		glLightfv(GL_LIGHT3, GL_AMBIENT, spot_ambient2);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, spot_diffuse2);
		glLightfv(GL_LIGHT3, GL_SPECULAR, spot_specular2);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 10.0);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 10.0);

	

		GLfloat mat_ambient[] = { 0.3, 0.5, 0.8, 1.0 };
		GLfloat mat_diffuse[] = { 0.5, 0.5, 0.7, 1.0 };
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

	DrawSphere(1.0f);

	glPushMatrix();
	glTranslatef(0.0, 1.5, 0.0);
	DrawSphere(0.5f);
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



void CParticleProjectView::OnLighton()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	b_Light = TRUE;
}


void CParticleProjectView::OnLightoff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	b_Light = FALSE;

}


void CParticleProjectView::OnThreeOne()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_particle.isThree = FALSE;
}


void CParticleProjectView::OnThreeThree()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_particle.isThree = TRUE;
}
