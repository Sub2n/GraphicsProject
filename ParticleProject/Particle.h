#pragma once
#include "ParticleProject.h"

#define MAX_PARTICLES 100000        // ������ ��ƼŬ ��

class Particle
{
public:
	BOOL b_tab = FALSE;
	BOOL isThree = FALSE;
	float   slowdown = 10.0f;          // Slow Down Particles
	float   xspeed;					// x ���ǵ�
	float   yspeed;					// y ���ǵ�
	GLuint  loop;               
	GLuint  col;                // ���� ���� ����
	float xPos, yPos, zPos;
	Particle(float px = 0, float py = 0, float pz = 0) {
		xPos = px;
		yPos = py;
		zPos = pz;
	}
	typedef struct                      // Particle Structure
	{
		bool    active;                 // Ȱ��ȭ
		float   life;                   // ����
		float   fade;                   // ���� ������ �ӵ� (���� ����)
		float   r;                  // Red ��
		float   g;                  // Green ��
		float   b;                  // Blue ��
		float   x;                  // X ��ġ
		float   y;                  // Y ��ġ
		float   z;                  // Z ��ġ
		float   xi;                 // X ����
		float   yi;                 // Y ����
		float   zi;                 // Z ����
		float   xg;                 // X �߷�
		float   yg;                 // Y �߷�
		float   zg;                 // Z �߷�
	}
	particles;                      
	particles particle[MAX_PARTICLES];          // ������ Particle �迭
	const GLfloat colors[12][3] =               // Rainbow Of Colors
	{
		{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
		{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
		{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
	};
	BOOL Init();
	void DrawParticle();
};

