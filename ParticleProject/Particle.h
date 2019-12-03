#pragma once
#include "ParticleProject.h"

#define MAX_PARTICLES   3000        // Number Of Particles To Create ( NEW )

class Particle
{
public:
	BOOL b_tab = FALSE;
	BOOL rainbow = TRUE;			// Rainbow Mode?    ( ADD )
	BOOL sp = FALSE;						// Spacebar Pressed?    ( ADD )
	BOOL rp = FALSE;						// Return Key Pressed?  ( ADD )
	float   slowdown = 5.0f;          // Slow Down Particles
	float   xspeed;					// Base X Speed (To Allow Keyboard Direction Of Tail)
	float   yspeed;					// Base Y Speed (To Allow Keyboard Direction Of Tail)
	float   zoom = -40.0f;            // Used To Zoom Out
	GLuint  loop;               // Misc Loop Variable
	GLuint  col;                // Current Color Selection
	GLuint  delay;              // Rainbow Effect Delay
	GLuint  texture[1];         // Storage For Our Particle Texture
	float xPos, yPos, zPos;
	Particle(float px = 0, float py = 0, float pz = 0) {
		xPos = px;
		yPos = py;
		zPos = pz;
	}
	typedef struct                      // Create A Structure For Particle
	{
		bool    active;                 // Active (Yes/No)
		float   life;                   // Particle Life
		float   fade;                   // Fade Speed
		float   r;                  // Red Value
		float   g;                  // Green Value
		float   b;                  // Blue Value
		float   x;                  // X Position
		float   y;                  // Y Position
		float   z;                  // Z Position
		float   xi;                 // X Direction
		float   yi;                 // Y Direction
		float   zi;                 // Z Direction
		float   xg;                 // X Gravity
		float   yg;                 // Y Gravity
		float   zg;                 // Z Gravity
	}
	particles;                      // Particles Structure
	particles particle[MAX_PARTICLES];          // Particle Array (Room For Particle Info)
	const GLfloat colors[12][3] =               // Rainbow Of Colors
	{
		{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
		{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
		{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
	};
	BOOL Init();
	void DrawParticle();
};

