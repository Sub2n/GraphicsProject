#include "pch.h"
#include "Particle.h"



BOOL Particle::Init() {

	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // ���� �ʱ�ȭ
	{
		particle[loop].active = true;                 // Particle Ȱ��ȭ
		particle[loop].life = 1.0f;                   // ���� �Ҵ�
		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;       // ���� ���� �ӵ� ���� ����
		particle[loop].r = colors[loop * (12 / MAX_PARTICLES)][0];        // Red Color
		particle[loop].g = colors[loop * (12 / MAX_PARTICLES)][1];        // Green Color
		particle[loop].b = colors[loop * (12 / MAX_PARTICLES)][2];        // Blue Color
		particle[loop].x = xPos;										// �����ڿ��� �Է¹��� Position���� ��ġ ����
		particle[loop].y = yPos;                 
		particle[loop].z = zPos;               
		particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;       // X �࿡�� �ӵ� ���� ����
		particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;       // Y �࿡�� �ӵ� ���� ����
		particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;       // Z �࿡�� �ӵ� ���� ����
		// �߷� �̿��ؼ� �������� �̵��ϴ� ��ġ �޶���
		particle[loop].xg = 0.0f;                     // ���� �߷� ����
		particle[loop].yg = -0.5f;                    // �������� �ۿ��ϴ� �߷�
		particle[loop].zg = 0.0f;                     // z�� �߷� ����
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
			particle[loop].x += particle[loop].xi / (slowdown * 1000);    // �� �� �ӵ��� �࿡�� �̵�
			particle[loop].y += particle[loop].yi / (slowdown * 1000);    
			particle[loop].z += particle[loop].zi / (slowdown * 1000);    
			particle[loop].xi += particle[loop].xg;						// �߷� �ۿ�
			particle[loop].yi += particle[loop].yg;           
			particle[loop].zi += particle[loop].zg;           
			particle[loop].life -= particle[loop].fade;       // ���� ���� ����
			if (particle[loop].life < 0.0f)                   
			{
				// ������ �ٽ� �ʱ�ȭ
				particle[loop].life = 1.0f;               
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;   
				particle[loop].x = xPos;                 
				particle[loop].y = yPos;                  
				particle[loop].z = zPos;                
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);  // �� �� ����, �ӵ�
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f); 
				particle[loop].zi = float((rand() % 60) - 30.0f);     
				particle[loop].r = colors[col][0];            // ���� ����
				particle[loop].g = colors[col][1];            
				particle[loop].b = colors[col][2];           
				col++;
				if (col > 11) col = 0;       
			}
			// ��ƼŬ ���� �۰�, �ӵ� ������, ���� ����
			if (b_tab) {	// TAB Ű ������ �����ϰ� �ٽ� �����ϴ� ȿ��
				particle[loop].x = xPos;                 
				particle[loop].y = yPos;                 
				particle[loop].z = zPos;                 
				particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;   // �������� �ӵ� ����
				particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;  
				particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;  
			}
		}
	}
}