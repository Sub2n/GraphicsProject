
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

	Vector p(0, 20, 20);
	Vector l(0, 0, -30);
	Vector u(0, 1, 0);
	pos = p;
	look = l;
	up = u;

	angle = 0;
	speed = 2;
}

// 화면 크기 바뀔 때마다 다시 계산
void CParticleProjectView::ReSizeGLScene(GLsizei width, GLsizei height) {
	// 0으로 나누면 안됨 (시야각 계산시)
	if (height == 0) height = 1;

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

// draw 함수
// MFC가 자동으로 부르는 onDraw에서 호출되어 그림 그림
void CParticleProjectView::DrawGLScene(void) {
	// screen이랑 depth buffer 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// 카메라 위치(x, y, z), 카메라가 바라보는 점의 위치(x, y, z), 카메라의 회전상태(roll) (vector)
	// 0, 20, 20, 0, 0, -30, 0, 1, 0


	gluLookAt(pos.x, pos.y, pos.z, look.x, look.y, look.z, up.x, up.y, up.z);
	// z : 카메라 앞 뒤, y : 높이

	// 잔디바닥
	/*
	glBegin(GL_QUADS);
		glColor4f(0.f, 1.f, 0.f, 0.7f);
		glVertex3f(-50.0f, 0.f, 50.0f);
		glVertex3f(50.0f, 0.f, 50.0f);
		glVertex3f(50.0f, 0.f, -50.0f);
		glVertex3f(-50.0f, 0.f, -50.0f);
	glEnd();
	*/
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
	switch (nChar) {

	case VK_LEFT:
	{
		angle -= 0.09f;
		Vector newLook = rotateVector(angle, 0, 1, 0, (look - pos) ) + pos;
		look = newLook;
		break;
	}


	case VK_RIGHT:
	{
		angle += 0.09f;
		Vector newLook = rotateVector(angle, 0, 1, 0, (look - pos)) + pos;
		look = newLook;
		break;
	}


	case VK_UP:
	{
		angle += 0.09f;
		Vector cross = up.crossProduct(look - pos);
		cross = cross * speed;
		Vector newLook = rotateVector(angle, cross.x, cross.y, cross.z, look - pos) + pos;
		look = newLook;
		break;
	}


	case VK_DOWN:
	{
		angle -= 0.09f;
		Vector cross = up.crossProduct(look - pos);
		cross = cross * speed;
		Vector newLook = rotateVector(angle, cross.x, cross.y, cross.z, look - pos) + pos;
		look = newLook;
		break;
	}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
