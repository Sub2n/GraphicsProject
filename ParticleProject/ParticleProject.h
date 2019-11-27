
// ParticleProject.h: ParticleProject 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#include "resource.h"       // 주 기호입니다.

#include <gl/gl.H>
#include <gl/glu.H>
#include <gl/glut.h>
#include <gl/glaux.H>

#include "vector.h"

#include <vector>

#pragma comment(lib, "OPENGL32.LIB")
#pragma comment(lib, "GLAUX.LIB")
#pragma comment(lib, "GLU32.LIB")
#pragma comment(lib, "GLUT32.LIB")

// CParticleProjectApp:
// 이 클래스의 구현에 대해서는 ParticleProject.cpp을(를) 참조하세요.
//

class CParticleProjectApp : public CWinAppEx
{
public:
	CParticleProjectApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CParticleProjectApp theApp;
