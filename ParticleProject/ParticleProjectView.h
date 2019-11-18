
// ParticleProjectView.h: CParticleProjectView 클래스의 인터페이스
//

#pragma once


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
	void InitGL(void);
	void ReSizeGLScene(GLsizei width, GLsizei height);
	void DrawGLScene(void);	// 그림 그리는 거 (여기 draw 코드만 수정하면 됨)

public:
	Vector pos, look, up; // 카메라

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
};

#ifndef _DEBUG  // ParticleProjectView.cpp의 디버그 버전
inline CParticleProjectDoc* CParticleProjectView::GetDocument() const
   { return reinterpret_cast<CParticleProjectDoc*>(m_pDocument); }
#endif

