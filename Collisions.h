#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "GLRender.h"
#include "GameObject.h"


//Returns normals of a list of vertices 
std::vector<glm::vec3> Normals(std::vector<glm::vec3> vertices)
{
	std::vector<glm::vec3> normals;
	glm::vec3 edge;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (i + 1 == vertices.size())
		{
			edge = vertices[0] - vertices[i];
		}
		else
		{
			edge = vertices[i + 1] - vertices[i];
		}

		normals.push_back(glm::normalize(glm::vec3(edge.y, -edge.x, 0)));
	}

	return normals;
}


//Testing for collision using the seperating axis theorem
//It relies on the basic rule that if there is a line (axis) that can be drawn that seperates two polygons they do not collide
//The lines we use are the normals to the edges of both polygons, and we do checks using the orthogonal projections of both polygons onto that line
//A great explanation can be found here https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169 but it only uses OBBs
bool TestSAT(GameObject A, GameObject B)
{
	//Getting global verts of the objects
	std::vector<glm::vec3> verticesA =A.Vertices(); 
	std::vector<glm::vec3> verticesB=B.Vertices();

	//Getting normals of the objects (in world space)
	std::vector<glm::vec3> normalsA = Normals(verticesA);
	std::vector<glm::vec3> normalsB = Normals(verticesB);

	//Looping through the normals of object A
	for (int i = 0; i < normalsA.size(); i++)
	{
		//The first step is to find the maximum and minimum projections of the vertices of object A onto the axis of the normal
		float min1, max1;
		min1 = max1 = glm::dot(normalsA[i], verticesA[0]); //Setting inital min and max

		for (int j = 1; j < verticesA.size(); j++)
		{
			float current = glm::dot(normalsA[i], verticesA[j]);

			if (current < min1)
				min1 = current;
			else if (current > max1)
				max1 = current;
		}

		//The scond step is to find the maximum and minimum projections of the vertices of object B onto the axis of the normal
		float min2, max2;
		min2 = max2 = glm::dot(normalsA[i], verticesB[0]);
		for (int j = 1; j < verticesB.size(); j++)
		{

			float current = glm::dot(normalsA[i], verticesB[j]);
			if (current < min2)
				min2 = current;
			else if (current > max2)
				max2 = current;
		}


		//If the two sets of values do no overlap, the two polygons are not colliding.
		//For the SAT check to pass, every axis must have overlap of the vertex projections.
		if (!(min1 <= max2 && max1 >= min2))
			return false;		
	}

	//Looping through the normals of object B using the same method as above.
	for (int i = 0; i < normalsB.size(); i++)
	{
		float min1, max1;

		min1 = max1 = glm::dot(normalsB[i], verticesA[0]);

		for (int j = 1; j < verticesA.size(); j++)
		{
			float current = glm::dot(normalsB[i], verticesA[j]);
			if (current < min1)
				min1 = current;
			else if (current > max1)
				max1 = current;
		}
		float min2, max2;

		min2 = max2 = glm::dot(normalsB[i], verticesB[0]);
		for (int j = 1; j < verticesB.size(); j++)
		{

			float current = glm::dot(normalsB[i], verticesB[j]);
			if (current < min2)
				min2 = current;
			else if (current > max2)
				max2 = current;
		}

		if (!(min1 < max2 && max1 > min2))
			return false;
	}

	//As the method has not returned false yet, it means that there is no axis that seperates the two polygons therefore they are colliding.
	return true;
}





#endif _COLLISIONS_H
