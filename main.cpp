/*
File Name: main.cpp
Copyright © 2018
Original authors: Srinivasan Thiagarajan
Refactored by Sanketh Bhat
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
This is the skeleton for a basic 2D engine in OpenGL.
*/


#include "GLRender.h"


// The basic structure for a body. We need a center, a radius, velocity, acceleration, mass, and VBO and total number of vertices.
struct SimpleBody {
	glm::vec3 origin;
	float radius;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	Drawer base; //VBO
	float mass;
	glm::mat4 MVP; //Vertices
};

SimpleBody circle;


#pragma region program specific Data members
// We change this variable upon detecting collision
float timestep = 0.012f;
//the number of disvision used to make the structure of the circle
int NumberOfDivisions = 20;
//Acceleration and force
glm::vec3 acc,force;

// vector of scene bodies
std::vector<SimpleBody*> bodies;
#pragma endregion


glm::vec3 AcceleratedVel(glm::vec3 Acc, glm::vec3 velocity, float h)
{
	// Calculate the change in velocity ina given time h, for an acceleration of Acc
	return velocity + (Acc*h);
}


glm::vec3 EulerIntegrator(glm::vec3 pos, float h, glm::vec3 &velocity, glm::vec3 &Acceleration)
{
	glm::vec3 P;

	//Calculate the displacement in that time step with the current velocity.
	P = pos + (h * velocity);

	//calculate the velocity at the end of the timestep.
	velocity = AcceleratedVel(Acceleration, velocity, h);
	Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	//return the position P
	return P;
}




//This function sets up a basic circle, change variables as required
void setup()
{
	circle.MVP = MVP;

	//Set up the global variables.
	acc = glm::vec3(0.0f, 0.0f, 0.0f);
	force = glm::vec3(0.0f, 0.0f, 0.0f);
	
	//Setting up the Circle
	circle.origin = glm::vec3(0.0f, 0.0f, 0.0f);
	circle.velocity = glm::vec3(0.0f, 0.0f,0.0f);
	circle.radius = 0.15f;
	circle.mass = 1.0f;
	circle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	float radius = circle.radius;

	VertexFormat center(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	std::vector<VertexFormat> vertices;

	float theta = 360.0f / NumberOfDivisions;

	//Circle vertex generation
	//In this example we are not implementing the proper the code for indices. We are just going to produce redundant information in the buffer.
	//since we are only having a small number of objects on the screen currently, this redundancy should not matter.
	for (int i = 0; i < NumberOfDivisions; i++)
	{
		//In every iteration, the center, the point at angle theta and at angle (theta+delta) are fed into the buffer.
		vertices.push_back(center);
		vertices.push_back(VertexFormat(glm::vec3(radius * cos(glm::radians(i*theta)), radius * sin(glm::radians(i*theta)), 0.0f), glm::vec4(0.7f, 0.20f, 0.0f, 1.0f)));
		vertices.push_back(VertexFormat(glm::vec3(radius * cos(glm::radians((i + 1)*theta)), radius * sin(glm::radians((i + 1)*theta)), 0.0f), glm::vec4(0.7f, 0.20f, 0.0f, 1.0f)));
	}

	circle.base.initBuffer(NumberOfDivisions * 3, &vertices[0]);

	//Adding circle to vector of PhysicsBodies that get rendered
	bodies.push_back(&circle);
}


// Functions called between every frame.
#pragma region util_functions

// This runs once every physics timestep.
void update()
{
	
	// Integrate the position of the object using the integrator.
	circle.origin = EulerIntegrator(circle.origin, timestep, circle.velocity, circle.acceleration);

	//The position of the object needs to be changed and sent to the GPU in form of a matrix
	glm::mat4 translation = glm::translate(glm::vec3(circle.origin));
	circle.MVP = PV * translation;
}


// This function is used to handle key inputs.
// It is a callback funciton. i.e. glfw takes the pointer to this function (via function pointer) and calls this function every time a key is pressed in the during event polling.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Sets the current window to a close flag when ESC is pressed
	if (key == GLFW_KEY_ESCAPE && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		glfwSetWindowShouldClose(window,1);
	}
	
}
#pragma endregion

void main()
{
	// Initializes most things needed before the main loop
	init();

	// Sends the funtion as a funtion pointer along with the window to which it should be applied to.
	glfwSetKeyCallback(window, key_callback);

	//Sets up bodies in the scene
	setup();

	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{
		// Call to update() which will update the gameobjects.
		update();

		// Call the render function(s).
		renderScene();

		//Rendering each body after the scene
		for each (SimpleBody *body in bodies)
			renderBody(body);

		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();
	}

	//Cleans shaders and the program and frees up GLFW memory
	cleanup();

	return;
}