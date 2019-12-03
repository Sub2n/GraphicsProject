#include "pch.h"
#include "Particle.h"



BOOL Particle::Init() {
	/*
		glShadeModel(GL_SMOOTH);                        // Enables Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                  // Black Background
		glClearDepth(1.0f);                         // Depth Buffer Setup
		glDisable(GL_DEPTH_TEST);                       // Disables Depth Testing
		glEnable(GL_BLEND);                         // Enable Blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);                   // Type Of Blending To Perform
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);           // Really Nice Perspective Calculations
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);                 // Really Nice Point Smoothing
	*/


	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // Initialize All The Textures
	{
		particle[loop].active = true;                 // Make All The Particles Active
		particle[loop].life = 1.0f;                   // Give All The Particles Full Life
		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;       // Random Fade Speed
		particle[loop].r = colors[loop * (12 / MAX_PARTICLES)][0];        // Select Red Rainbow Color
		particle[loop].g = colors[loop * (12 / MAX_PARTICLES)][1];        // Select Red Rainbow Color
		particle[loop].b = colors[loop * (12 / MAX_PARTICLES)][2];        // Select Red Rainbow Color
		particle[loop].x = xPos;                  // Center On X Axis
		particle[loop].y = yPos;                  // Center On Y Axis
		particle[loop].z = zPos;                  // Center On Z Axis
		particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;       // Random Speed On X Axis
		particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;       // Random Speed On Y Axis
		particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;       // Random Speed On Z Axis
		particle[loop].xg = 0.0f;                     // Set Horizontal Pull To Zero
		particle[loop].yg = -0.8f;                    // Set Vertical Pull Downward
		particle[loop].zg = 0.0f;                     // Set Pull On Z Axis To Zero
	}
	col = 0;
	xspeed = 4.1f;
	yspeed = 0.3f;
	return TRUE;                                // Initialization Went OK
}

void Particle::DrawParticle() {
	for (loop = 0; loop < MAX_PARTICLES; loop++)                   // Loop Through All The Particles
	{
		if (particle[loop].active)                  // If The Particle Is Active
		{
			float x = particle[loop].x;               // Grab Our Particle X Position
			float y = particle[loop].y;               // Grab Our Particle Y Position
			float z = particle[loop].z;              // Particle Z Pos + Zoom
			// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);             // Build Quad From A Triangle Strip
				glVertex3f(x + 0.005f, y + 0.005f, z); // Top Right
				glVertex3f(x - 0.005f, y + 0.005f, z); // Top Left
				glVertex3f(x + 0.005f, y - 0.005f, z); // Bottom Right
				glVertex3f(x - 0.005f, y - 0.005f, z); // Bottom Left
			glEnd();                        // Done Building Triangle Strip
			particle[loop].x += particle[loop].xi / (slowdown * 1000);    // Move On The X Axis By X Speed
			particle[loop].y += particle[loop].yi / (slowdown * 1000);    // Move On The Y Axis By Y Speed
			particle[loop].z += particle[loop].zi / (slowdown * 1000);    // Move On The Z Axis By Z Speed
			particle[loop].xi += particle[loop].xg;           // Take Pull On X Axis Into Account
			particle[loop].yi += particle[loop].yg;           // Take Pull On Y Axis Into Account
			particle[loop].zi += particle[loop].zg;           // Take Pull On Z Axis Into Account
			particle[loop].life -= particle[loop].fade;       // Reduce Particles Life By 'Fade'
			if (particle[loop].life < 0.0f)                    // If Particle Is Burned Out
			{
				particle[loop].life = 1.0f;               // Give It New Life
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;   // Random Fade Value
				particle[loop].x = xPos;                  // Center On X Axis
				particle[loop].y = yPos;                  // Center On Y Axis
				particle[loop].z = zPos;                  // Center On Z Axis
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);  // X Axis Speed And Direction
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);  // Y Axis Speed And Direction
				particle[loop].zi = float((rand() % 60) - 30.0f);     // Z Axis Speed And Direction
				particle[loop].r = colors[col][0];            // Select Red From Color Table
				particle[loop].g = colors[col][1];            // Select Green From Color Table
				particle[loop].b = colors[col][2];            // Select Blue From Color Table
				col++;
				if (col > 11) col = 0;       // If Color Is To High Reset It
			}
			// 파티클 입자 작게, 속도 느리게, 입자 많이
			if (b_tab) {
				particle[loop].x = xPos;                  // Center On X Axis
				particle[loop].y = yPos;                  // Center On Y Axis
				particle[loop].z = zPos;                  // Center On Z Axis
				particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;   // Random Speed On X Axis
				particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;   // Random Speed On Y Axis
				particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;   // Random Speed On Z Axis
			}
		}
	}
}