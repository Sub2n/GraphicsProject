#include "pch.h"
#include "Particle.h"



BOOL Particle::Init() {

	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // 입자 초기화
	{
		particle[loop].active = true;                 // Particle 활성화
		particle[loop].life = 1.0f;                   // 생명 할당
		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;       // 생명 소진 속도 랜덤 결정
		particle[loop].r = colors[loop * (12 / MAX_PARTICLES)][0];        // Red Color
		particle[loop].g = colors[loop * (12 / MAX_PARTICLES)][1];        // Green Color
		particle[loop].b = colors[loop * (12 / MAX_PARTICLES)][2];        // Blue Color
		particle[loop].x = xPos;										// 생성자에서 입력받은 Position으로 위치 설정
		particle[loop].y = yPos;                 
		particle[loop].z = zPos;               
		particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;       // X 축에서 속도 랜덤 결정
		particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;       // Y 축에서 속도 랜덤 결정
		particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;       // Z 축에서 속도 랜덤 결정
		// 중력 이용해서 시점에서 이동하는 위치 달라짐
		particle[loop].xg = 0.0f;                     // 수평 중력 없음
		particle[loop].yg = -0.5f;                    // 수직으로 작용하는 중력
		particle[loop].zg = 0.0f;                     // z축 중력 없음
	}
	col = 0;
	xspeed = 4.1f;
	yspeed = 4.3f;
	return TRUE;                                // Initialization Went OK
}

void Particle::DrawParticle() {
	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // Loop Through All The Particles
	{
		if (particle[loop].active)                  // If The Particle Is Active
		{
			float x = particle[loop].x;              
			float y = particle[loop].y;               
			float z = particle[loop].z;             
			
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);             
				glVertex3f(x + 0.004f, y + 0.004f, z); // Top Right
				glVertex3f(x - 0.004f, y + 0.004f, z); // Top Left
				glVertex3f(x + 0.004f, y - 0.004f, z); // Bottom Right
				glVertex3f(x - 0.004f, y - 0.004f, z); // Bottom Left
			glEnd();
			if (isThree) {
				glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(x + 3.0 + 0.004f, y + 0.004f, z); // Top Right
				glVertex3f(x + 3.0 - 0.004f, y + 0.004f, z); // Top Left
				glVertex3f(x + 3.0 + 0.004f, y - 0.004f, z); // Bottom Right
				glVertex3f(x + 3.0 - 0.004f, y - 0.004f, z); // Bottom Left
				glEnd();
				glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(x - 3.0 + 0.004f, y + 0.004f, z); // Top Right
				glVertex3f(x - 3.0 - 0.004f, y + 0.004f, z); // Top Left
				glVertex3f(x - 3.0 + 0.004f, y - 0.004f, z); // Bottom Right
				glVertex3f(x - 3.0 - 0.004f, y - 0.004f, z); // Bottom Left
				glEnd();
			}
			particle[loop].x += particle[loop].xi / (slowdown * 1000);    // 각 축 속도로 축에서 이동
			particle[loop].y += particle[loop].yi / (slowdown * 1000);    
			particle[loop].z += particle[loop].zi / (slowdown * 1000);    
			particle[loop].xi += particle[loop].xg;						// 중력 작용
			particle[loop].yi += particle[loop].yg;           
			particle[loop].zi += particle[loop].zg;           
			particle[loop].life -= particle[loop].fade;       // 랜덤 생명 감소
			if (particle[loop].life < 0.0f)                   
			{
				// 죽으면 다시 초기화
				particle[loop].life = 1.0f;               
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;   
				particle[loop].x = xPos;                 
				particle[loop].y = yPos;                  
				particle[loop].z = zPos;                
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);  // 각 축 방향, 속도
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f); 
				particle[loop].zi = float((rand() % 60) - 30.0f);     
				particle[loop].r = colors[col][0];            // 색깔 결정
				particle[loop].g = colors[col][1];            
				particle[loop].b = colors[col][2];           
				col++;
				if (col > 11) col = 0;       
			}
			// 파티클 입자 작게, 속도 느리게, 입자 많이
			if (b_tab) {	// TAB 키 누르면 폭발하고 다시 시작하는 효과
				particle[loop].x = xPos;                 
				particle[loop].y = yPos;                 
				particle[loop].z = zPos;                 
				particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;   // 랜덤으로 속도 결정
				particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;  
				particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;  
			}
		}
	}
}