#pragma once
#include "ParticleProject.h"

#define MAX_PARTICLES 100000        // 생성할 파티클 수

class Particle
{
public:
	BOOL b_tab = FALSE;
	BOOL isThree = FALSE;
	float   slowdown = 10.0f;          // Slow Down Particles
	float   xspeed;					// x 스피드
	float   yspeed;					// y 스피드
	GLuint  loop;               
	GLuint  col;                // 현재 색상 선택
	float xPos, yPos, zPos;
	Particle(float px = 0, float py = 0, float pz = 0) {
		xPos = px;
		yPos = py;
		zPos = pz;
	}
	typedef struct                      // Particle Structure
	{
		bool    active;                 // 활성화
		float   life;                   // 생명
		float   fade;                   // 생명 꺼지는 속도 (랜덤 결정)
		float   r;                  // Red 값
		float   g;                  // Green 값
		float   b;                  // Blue 값
		float   x;                  // X 위치
		float   y;                  // Y 위치
		float   z;                  // Z 위치
		float   xi;                 // X 방향
		float   yi;                 // Y 방향
		float   zi;                 // Z 방향
		float   xg;                 // X 중력
		float   yg;                 // Y 중력
		float   zg;                 // Z 중력
	}
	particles;                      
	particles particle[MAX_PARTICLES];          // 생성할 Particle 배열
	const GLfloat colors[12][3] =               // Rainbow Of Colors
	{
		{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
		{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
		{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
	};
	BOOL Init();
	void DrawParticle();
};

