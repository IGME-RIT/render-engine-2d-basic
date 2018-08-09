/*
File Name: main.cpp
Copyright © 2018
Original authors: Sanketh Bhat
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
2D rigidbody engine
*/


#include "GLRender.h"
#include "Collisions.h"



#pragma region program specific Data members

float timestep = .016;
//the number of disvision used to make the structure of the circle
int NumberOfDivisions = 20;


glm::vec3 mousePos;


GameObject* objA;
GameObject* objB;

Model* meshA;
Model* meshB;

// vector of scene bodies
std::vector<GameObject*> bodies;


#pragma endregion

glm::vec3 GetMousePos()
{

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	//normalized device coordinates
	float NDCX = (x / 400.0f) - 1.0f;
	float NDCY = -(y / 400.0f) + 1.0f;

	return glm::vec3(NDCX, NDCY, 0.0f);


}

#pragma region util_functions



void update()
{

	for each (GameObject* body in bodies)
	{
		body->update(timestep, PV);
	}
}





// This function is used to handle key inputs.
// It is a callback funciton. i.e. glfw takes the pointer to this function (via function pointer) and calls this function every time a key is pressed in the during event polling.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Sets the current window to a close flag when ESC is pressed
	if (key == GLFW_KEY_ESCAPE && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		glfwSetWindowShouldClose(window, 1);
	}

	//Object 1 Controls
	if (key == GLFW_KEY_A && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->Position(glm::vec3(objA->Position().x- 0.05f, objA->Position().y,0));
	}
	if (key == GLFW_KEY_D && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->Position(glm::vec3(objA->Position().x + 0.05f, objA->Position().y, 0));
	}
	if (key == GLFW_KEY_W && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->Position(glm::vec3(objA->Position().x , objA->Position().y + 0.05f, 0));
	}
	if (key == GLFW_KEY_S && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->Position(glm::vec3(objA->Position().x , objA->Position().y - 0.05f, 0));
	}
	if (key == GLFW_KEY_Q && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->rotate(glm::vec3(0, 0, 0.1f));
	}
	if (key == GLFW_KEY_E && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objA->rotate(glm::vec3(0, 0, -0.1f));
	}

	//Object 2 Controls
	if (key == GLFW_KEY_J && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->Position(glm::vec3(objB->Position().x - 0.05f, objB->Position().y, 0));
	}
	if (key == GLFW_KEY_L && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->Position(glm::vec3(objB->Position().x + 0.05f, objB->Position().y, 0));
	}
	if (key == GLFW_KEY_I && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->Position(glm::vec3(objB->Position().x, objB->Position().y + 0.05f, 0));
	}
	if (key == GLFW_KEY_K && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->Position(glm::vec3(objB->Position().x, objB->Position().y - 0.05f, 0));
	}
	if (key == GLFW_KEY_U && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->rotate(glm::vec3(0, 0, 0.1f));
	}
	if (key == GLFW_KEY_O && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		objB->rotate(glm::vec3(0, 0, -0.1f));
	}

}
#pragma endregion

//Makes a 2D polygon based on the vertex number arguement
Model* setupModel(int n, glm::vec4 color, float size = 0.25f)
{
	std::vector<GLuint> indices;
	std::vector<VertexFormat> vertices;
	VertexFormat center(glm::vec3(0.0f, 0.0f, 0.0f), color);
	
	//Only add indices if you drawing a polygon with more than 3 points.
	if (n > 3)
	{
		//Indices are added in threes to form tris
		for (int i = 1; i < n+1; i++)
		{
			indices.push_back(0); //Start at the center
			if(i==n) //If we are at the last last vertex, go back to the first  non-center vertex and add it
			{
				indices.push_back(i);
				indices.push_back(1);

			}
			else
			{	//Adds current vertex and the next one
				indices.push_back(i);
				indices.push_back(i + 1);

			}

		}

		//Only 3+ point polygons need a center vertex
		vertices.push_back(center);
	}	

	float theta = 360.0f / n;
	

	//vertex generation
	for (int i = 0; i < n; i++)
	{
		//The point at angle theta  are fed into the buffer.
		vertices.push_back(VertexFormat(glm::vec3(size * cos(glm::radians(i*theta)), size * sin(glm::radians(i*theta)), 0.0f), color));
		
	}
	 return new Model(vertices.size(), vertices.data(), indices.size(), indices.data());

}




void main()
{
	// Initializes most things needed before the main loop
	init();

	// Sends the funtion as a funtion pointer along with the window to which it should be applied to.

	glfwSetKeyCallback(window, key_callback);

	int v1;
	int v2;
	bool currentState = false;
	bool prevState = false;

	std::cout << "Enter number of sides for object 1: ";
	std::cin >> v1;
	std::cout << "Enter number of sides for object 2: ";
	std::cin >> v2;

	std::cout << "WASD to move object 1, Q & E to rotate it. \n";
	std::cout << "IJKL to move object 2, U & O to rotate it. \n";
	
	//Creating models
	meshA = setupModel(v1, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	meshB = setupModel(v2, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	

	//Sets up bodies in the scene
	objA = new GameObject(meshA);
	objB = new GameObject(meshB);

	bodies.push_back(objA);
	bodies.push_back(objB);

	objA->Position(glm::vec3(-0.5, 0, 0));
	objB->Position(glm::vec3(0.5, 0, 0));

	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{

		// Call to update() which will update the gameobjects.
		update();

		if (TestSAT(*objA, *objB))
			currentState = true;
		else
			currentState = false;

		//Printing current collision info
		if(currentState && !prevState)
			std::cout << "Currently Colliding!\n";
		else if(!currentState && prevState)
			std::cout << "Currently not Colliding!\n";

		// Call the render function(s).
		renderScene();

		for each (GameObject* body in bodies)
		{
			body->render(uniMVP);
		}
		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();

		prevState = currentState;

	}

	delete objA,objB,meshA,meshB;

	//Cleans shaders and the program and frees up GLFW memory
	cleanup();

	return;
}