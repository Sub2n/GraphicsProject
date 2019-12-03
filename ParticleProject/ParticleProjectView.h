
// ParticleProjectView.h: CParticleProjectView 클래스의 인터페이스
//

#pragma once

# define M_PI           3.14159265358979323846  /* pi */

class CParticleProjectView : public CView
{
protected: // serialization에서만 만들어집니다.
	CParticleProjectView() noexcept;
	DECLARE_DYNCREATE(CParticleProjectView)

// 특성입니다.
public:
	CParticleProjectDoc* GetDocument() const;

// 작업입니다.
public:
	HDC m_hDC;	// device context (MFC에서 그림 그리기 위해서 사용)
	HGLRC m_hglRC;	// rendering context

public:
	BOOL SetDevicePixelFormat(HDC hdc);
	void InitGL(GLvoid);
	void ReSizeGLScene(GLsizei width, GLsizei height);
	void DrawGLScene(void);	// 그림 그리는 거 (여기 draw 코드만 수정하면 됨)
	GLuint loadTextureBMP(const char* filename);
	void DrawCube();
	void DrawSphere(float r);
	void loadTexture(char *file, GLuint *p_texture);
	void DrawSkyBox(float width, float height, float length);

public:
	const GLuint Num_Particles = 5000;
	Vector cameraPos, cameraFront, cameraUp; // 카메라
	Particle m_particle = Particle(0.0, 3.0, 0.0); // particle instance
	float Width, Height;
	float lastX, lastY;
	BOOL firstMouse, mouseMove;
	float pitch, yaw;
	GLuint m_Textures[6];
	BOOL m_posLight, m_spotLight;

public:
	float radians(float degree);
	Vector rotateVector(float angle, int x, int y, int z, Vector& v);
	
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CParticleProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSpotlight();
	afx_msg void OnPositional();
};

#ifndef _DEBUG  // ParticleProjectView.cpp의 디버그 버전
inline CParticleProjectDoc* CParticleProjectView::GetDocument() const
   { return reinterpret_cast<CParticleProjectDoc*>(m_pDocument); }
#endif

