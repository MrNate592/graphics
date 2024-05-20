//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath> 

#define M_PI 3.14159265359


const unsigned int width = 1400;
const unsigned int height = 1200;


std::vector<float> CylinderVertices(float bottomRadius, float topRadius, float height, int numSegments) {
	std::vector<float> vertices;

	// Generate vertices for the sides of the cylinder
	for (int i = 0; i < numSegments; ++i) {
		float theta1 = 2.0 * M_PI * i / numSegments;
		float theta2 = 2.0 * M_PI * (i + 1) / numSegments;

		float x1_bottom = bottomRadius * cos(theta1);
		float z1_bottom = bottomRadius * sin(theta1);
		float x2_bottom = bottomRadius * cos(theta2);
		float z2_bottom = bottomRadius * sin(theta2);

		float x1_top = topRadius * cos(theta1);
		float z1_top = topRadius * sin(theta1);
		float x2_top = topRadius * cos(theta2);
		float z2_top = topRadius * sin(theta2);

		// Calculate texture coordinates
		float texCoord1 = static_cast<float>(i) / numSegments;
		float texCoord2 = static_cast<float>(i + 1) / numSegments;

		// Vertices for the first triangle of the side quad
		vertices.push_back(x1_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z1_bottom);
		vertices.push_back(texCoord1);
		vertices.push_back(0.0f);

		vertices.push_back(x1_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z1_top);
		vertices.push_back(texCoord1);
		vertices.push_back(1.0f);

		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z2_bottom);
		vertices.push_back(texCoord2);
		vertices.push_back(0.0f);

		// Vertices for the second triangle of the side quad
		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z2_bottom);
		vertices.push_back(texCoord2);
		vertices.push_back(0.0f);

		vertices.push_back(x1_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z1_top);
		vertices.push_back(texCoord1);
		vertices.push_back(1.0f);

		vertices.push_back(x2_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z2_top);
		vertices.push_back(texCoord2);
		vertices.push_back(1.0f);
	}

	return vertices;
}


std::vector<float> generateHemisphereVertices(float centerX, float centerY, float centerZ, float radius, int numSegments) {
	std::vector<float> vertices;

	// Generate vertices for the upper hemisphere
	for (int j = 0; j <= numSegments / 4; ++j) {
		float theta1 = M_PI * j / (numSegments / 2);
		float theta2 = M_PI * (j + 1) / (numSegments / 2);

		for (int i = 0; i <= numSegments; ++i) {
			float phi = 2.0f * M_PI * i / numSegments;

			float x1 = radius * sin(theta1) * cos(phi) + centerX;
			float y1 = radius * cos(theta1) + centerY;
			float z1 = radius * sin(theta1) * sin(phi) + centerZ;

			float x2 = radius * sin(theta2) * cos(phi) + centerX;
			float y2 = radius * cos(theta2) + centerY;
			float z2 = radius * sin(theta2) * sin(phi) + centerZ;

			vertices.push_back(x1);
			vertices.push_back(y1 + 1.0f);
			vertices.push_back(z1);

			vertices.push_back(x2);
			vertices.push_back(y2 + 1.0f);
			vertices.push_back(z2);
		}
	}

	return vertices;
}

std::vector<float> generatePlatform(float centerX, float centerY, float radius, int numSegments, float yaxis, float outerRadius, float innerRadius, float thickness) {
	std::vector<float> vertices;

	float innerThickness = thickness * 1.5f; // Adjust the inner thickness here

	// Calculate the angle increment for each segment of the octago
	float angleIncrement = 2.0f * M_PI / numSegments;

	// Generate vertices for each segment of the octagon
	for (int i = 0; i < numSegments; ++i) {
		// Calculate the angle for the current segment
		float angle = i * angleIncrement;

		// Calculate the x and z coordinates for the current segment
		float xOuter = centerX + outerRadius * cos(angle);
		float zOuter = centerY + outerRadius * sin(angle);

		float xInner = centerX + innerRadius * cos(angle);
		float zInner = centerY + innerRadius * sin(angle);

		// Calculate the x and z coordinates for the next segment
		float nextAngle = (i + 1) * angleIncrement;
		if (i == numSegments - 1) {
			// Wrap around from the last segment to the first one
			nextAngle = 0.0f;
		}
		float nextXOuter = centerX + outerRadius * cos(nextAngle);
		float nextZOuter = centerY + outerRadius * sin(nextAngle);

		float nextXInner = centerX + innerRadius * cos(nextAngle);
		float nextZInner = centerY + innerRadius * sin(nextAngle);

		// Push back the vertices for the current segment
		// Bottom vertices
		vertices.push_back(xOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Top vertices
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);

		vertices.push_back(xInner);
		vertices.push_back(yaxis - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(yaxis - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(nextZInner);

		// Top vertices
		vertices.push_back(xInner);
		vertices.push_back(yaxis + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(yaxis + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(nextZInner);

		// Side faces
		// First triangle
		vertices.push_back(xOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Second triangle
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);
	}

	return vertices;
}

std::vector<float> generateCylinderVertices1(float bottomRadius, float topRadius, float height, int numSegments) {
	std::vector<float> vertices;

	// Generate vertices for the sides of the cylinder
	for (int i = 0; i < numSegments; ++i) {
		float theta1 = 2.5 * M_PI * i / numSegments;
		float theta2 = 2.5 * M_PI * (i + 1) / numSegments;

		float x1_bottom = bottomRadius * cos(theta1);
		float z1_bottom = bottomRadius * sin(theta1);
		float x2_bottom = bottomRadius * cos(theta2);
		float z2_bottom = bottomRadius * sin(theta2);

		float x1_top = topRadius * cos(theta1);
		float z1_top = topRadius * sin(theta1);
		float x2_top = topRadius * cos(theta2);
		float z2_top = topRadius * sin(theta2);

		// Move vertices up by 5 units along the y-axis
		float yOffset = 36.5f;

		// Vertices for the first triangle of the side quad
		vertices.push_back(x1_bottom);
		vertices.push_back(-height / 1.0f + yOffset);
		vertices.push_back(z1_bottom);
		vertices.push_back(x1_top);
		vertices.push_back(height / 1.0f + yOffset);
		vertices.push_back(z1_top);
		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 1.0f + yOffset);
		vertices.push_back(z2_bottom);

		// Vertices for the second triangle of the side quad
		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 1.0f + yOffset);
		vertices.push_back(z2_bottom);
		vertices.push_back(x1_top);
		vertices.push_back(height / 1.0f + yOffset);
		vertices.push_back(z1_top);
		vertices.push_back(x2_top);
		vertices.push_back(height / 1.0f + yOffset);
		vertices.push_back(z2_top);
	}

	return vertices;
}

// Function to generate vertices for a fence post
std::vector<float> generateFencePostVertices(float xPosition, float yPosition, float zPosition) {
	// Define fence post dimensions
	float width = 0.05f;
	float height = 1.0f;
	float depth = 0.1f;

	// Calculate vertices for the fence post
	std::vector<float> vertices = {
		// Front face
		-width / 2.0f + xPosition, -height / 2.0f + yPosition,  -depth / 2.0f + zPosition,  // Bottom left
		 width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Bottom right
		 width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Top right
		-width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Top left

		// Back face
		-width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Bottom left
		 width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Bottom right
		 width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Top right
		-width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Top left

		// Left face
		-width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Bottom front
		-width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Top front
		-width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Top back
		-width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Bottom back

		// Right face
		 width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Bottom front
		 width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Top front
		 width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Top back
		 width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Bottom back

		 // Top face
		 -width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Front left
		  width / 2.0f + xPosition,  height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Front right
		  width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Back right
		 -width / 2.0f + xPosition,  height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Back left

		 // Bottom face
		 -width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Front left
		  width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Front right
		  width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition,  // Back right
		 -width / 2.0f + xPosition, -height / 2.0f + yPosition, -depth / 2.0f + zPosition   // Back left
	};

	return vertices;
}

std::vector<float> generateGroundVertices(float centerX, float centerY, float radius, int numSegments, float yaxis) {
	std::vector<float> vertices;
	float outerRadius = 3.5f;
	float innerRadius = 3.4f;
	// Define the thickness of the platform
	float thickness = 0.05f;
	float innerThickness = thickness * 1.5f; // Adjust the inner thickness here

	// Calculate the angle increment for each segment of the octagon
	float angleIncrement = 2.0f * M_PI / numSegments;

	// Generate vertices for each segment of the octagon
	for (int i = 0; i < numSegments; ++i) {
		// Calculate the angle for the current segment
		float angle = i * angleIncrement;

		// Calculate the x and z coordinates for the current segment
		float xOuter = centerX + outerRadius * cos(angle);
		float zOuter = centerY + outerRadius * sin(angle);

		float xInner = centerX + innerRadius * cos(angle);
		float zInner = centerY + innerRadius * sin(angle);

		// Calculate the x and z coordinates for the next segment
		float nextAngle = (i + 1) * angleIncrement;
		if (i == numSegments - 1) {
			// Wrap around from the last segment to the first one
			nextAngle = 0.0f;
		}
		float nextXOuter = centerX + outerRadius * cos(nextAngle);
		float nextZOuter = centerY + outerRadius * sin(nextAngle);

		float nextXInner = centerX + innerRadius * cos(nextAngle);
		float nextZInner = centerY + innerRadius * sin(nextAngle);

		// Push back the vertices for the current segment
		// Bottom vertices
		vertices.push_back(xOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Top vertices
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);

		vertices.push_back(xInner);
		vertices.push_back(yaxis - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(yaxis - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(nextZInner);

		// Top vertices
		vertices.push_back(xInner);
		vertices.push_back(yaxis + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(yaxis + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(nextZInner);

		// Side faces
		// First triangle
		vertices.push_back(xOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Second triangle
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);
		vertices.push_back(xOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(yaxis + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);
	}

	return vertices;
}

std::vector<float> generatePostCircleVertices(float centerX, float centerY, float radius, float height, int numPosts) {
	std::vector<float> vertices;

	// Calculate the angles between each post for an octagon
	float angleIncrement = 2.0f * M_PI / 8; // 8 sides for an octagon
	float apothem = radius * cos(M_PI / 8); // Angle between center and side midpoint in an octagon

	// Adjust the angle increment based on the number of posts
	angleIncrement *= 8.0f / 9;

	// Generate vertices for each post
	for (int i = 0; i < 9; ++i) {
		// Calculate the angle for this post
		float angle = angleIncrement * i;

		// Calculate the position on the edge of the octagon
		float x = centerX + apothem * cos(angle);
		float z = centerY + apothem * sin(angle);

		// Calculate vertices for the post at the current angle
		std::vector<float> postVertices = generateFencePostVertices(x, height / 2.0f, z);
		vertices.insert(vertices.end(), postVertices.begin(), postVertices.end());
	}

	return vertices;
}
// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};

GLfloat lightVertices[] =
{
	// Larger coordinates for a bigger cube
	-2.0f, -2.0f,  2.0f,
	-2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f,  2.0f,
	-2.0f,  2.0f,  2.0f,
	-2.0f,  2.0f, -2.0f,
	 2.0f,  2.0f, -2.0f,
	 2.0f,  2.0f,  2.0f
};


GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


GLfloat lightVertices1[] =
{ //     COORDINATES     //
	 0.5f, -1.1f,  0.1f,
	-0.5f, -1.1f, -0.1f,
	 0.5f, -1.1f, -0.1f,
	 0.5f, -1.1f,  0.1f,
	-0.5f,  1.1f,  0.1f,
	-0.5f,  1.1f, -0.1f,
	 0.5f,  1.1f, -0.1f,
	 0.5f,  1.1f,  0.1f
};

GLuint lightIndices1[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};



// Function to read OBJ file
bool loadOBJ(const std::string& filename, std::vector<GLfloat>& vertices) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file" << std::endl;
		return false;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "v") { // Vertex position
			glm::vec3 position;
			iss >> position.x >> position.y >> position.z;
			positions.push_back(position);
		}
		else if (type == "vt") { // Texture coordinates
			glm::vec2 texCoord;
			iss >> texCoord.x >> texCoord.y;
			texCoords.push_back(texCoord);
		}
		else if (type == "vn") { // Normal coordinates
			glm::vec3 normal;
			iss >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (type == "f") { // Face data
			std::string vertexStr;
			while (iss >> vertexStr) {
				std::istringstream viss(vertexStr);
				std::string indexStr;
				std::vector<int> indices;
				while (std::getline(viss, indexStr, '/')) {
					if (!indexStr.empty())
						indices.push_back(std::stoi(indexStr));
				}
				// Assuming OBJ file format is consistent (position/texCoord/normal)
				vertices.push_back(positions[indices[0] - 1].x);
				vertices.push_back(positions[indices[0] - 1].y);
				vertices.push_back(positions[indices[0] - 1].z);
				vertices.push_back(texCoords[indices[1] - 1].x);
				vertices.push_back(texCoords[indices[1] - 1].y);
				vertices.push_back(normals[indices[2] - 1].x); // Add normal x-coordinate
				vertices.push_back(normals[indices[2] - 1].y); // Add normal y-coordinate
				vertices.push_back(normals[indices[2] - 1].z); // Add normal z-coordinate
			}
		}
	}
	return true;
}

std::vector<float> generateStaircaseVertices(float staircaseWidth, float staircaseHeight, float staircaseDepth, int numSteps) {
	std::vector<float> vertices;

	// Calculate dimensions of each step
	float stepWidth = staircaseWidth;
	float stepHeight = staircaseHeight / numSteps;
	float stepDepth = staircaseDepth;

	// Angle for rotating each step
	float angleIncrement = 360.0f / numSteps; // Full circle for one revolution

	// Radius of the cylinder
	float radius1 = 13.6f;

	for (int j = 0; j < 11; ++j) { // Create 3 staircases
		float startY = (j == 0) ? 0.0f : staircaseHeight * (j - 1); // Start from 0 for the first staircase
		for (int i = 0; i < numSteps; ++i)
		{
			// Calculate rotation angle for current step
			float angle = i * angleIncrement;
			float angleRad = angle * M_PI / 180.0f; // Convert to radians

			float radius = radius1 - 0.023f * (j * numSteps + i); // Adjust this factor as needed


			// Calculate coordinates of the center of the step
			float centerX = radius * cos(angleRad);
			float centerY = startY + i * stepHeight + 53.0f;
			float centerZ = radius * sin(angleRad);

			// Calculate coordinates of the four corners of the step
			float x1 = centerX - stepWidth / 2.0f;
			float y1 = centerY;
			float z1 = centerZ;

			float x2 = centerX + stepWidth / 2.0f;
			float y2 = centerY;
			float z2 = centerZ;

			float x3 = centerX + stepWidth / 2.0f;
			float y3 = centerY;
			float z3 = centerZ + stepDepth;

			float x4 = centerX - stepWidth / 2.0f;
			float y4 = centerY;
			float z4 = centerZ + stepDepth;

			float x5 = x1;
			float y5 = centerY + stepHeight;
			float z5 = z1;

			float x6 = x2;
			float y6 = centerY + stepHeight;
			float z6 = z2;

			float x7 = x3;
			float y7 = centerY + stepHeight;
			float z7 = z3;

			float x8 = x4;
			float y8 = centerY + stepHeight;
			float z8 = z4;

			// Add vertices of the step (including all faces) to the vector
			// Front face
			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z3);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z3);

			vertices.push_back(x4);
			vertices.push_back(y4);
			vertices.push_back(z4);

			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			// Left face
			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			vertices.push_back(x4);
			vertices.push_back(y4);
			vertices.push_back(z4);

			vertices.push_back(x8);
			vertices.push_back(y8);
			vertices.push_back(z8);

			vertices.push_back(x8);
			vertices.push_back(y8);
			vertices.push_back(z8);

			vertices.push_back(x5);
			vertices.push_back(y5);
			vertices.push_back(z5);

			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			// Floor
			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);

			vertices.push_back(x6);
			vertices.push_back(y6);
			vertices.push_back(z6);

			vertices.push_back(x6);
			vertices.push_back(y6);
			vertices.push_back(z6);

			vertices.push_back(x5);
			vertices.push_back(y5);
			vertices.push_back(z5);

			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			// Right face
			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z3);

			vertices.push_back(x7);
			vertices.push_back(y7);
			vertices.push_back(z7);

			vertices.push_back(x7);
			vertices.push_back(y7);
			vertices.push_back(z7);

			vertices.push_back(x6);
			vertices.push_back(y6);
			vertices.push_back(z6);

			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);

			// Bottom face
			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			vertices.push_back(x4);
			vertices.push_back(y4);
			vertices.push_back(z4);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z3);

			vertices.push_back(x3);
			vertices.push_back(y3);
			vertices.push_back(z3);

			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);

			vertices.push_back(x1);
			vertices.push_back(y1);
			vertices.push_back(z1);

			// Top face
			vertices.push_back(x5);
			vertices.push_back(y5);
			vertices.push_back(z5);

			vertices.push_back(x6);
			vertices.push_back(y6);
			vertices.push_back(z6);

			vertices.push_back(x7);
			vertices.push_back(y7);
			vertices.push_back(z7);

			vertices.push_back(x7);
			vertices.push_back(y7);
			vertices.push_back(z7);

			vertices.push_back(x8);
			vertices.push_back(y8);
			vertices.push_back(z8);

			vertices.push_back(x5);
			vertices.push_back(y5);
			vertices.push_back(z5);
		}
	}
	return vertices;
}

void scaleVertices(std::vector<float>& vertices, float scaleFactor) {
	for (size_t i = 0; i < vertices.size(); i += 3) {
		vertices[i] *= scaleFactor;      // Scale x-coordinate
		vertices[i + 1] *= scaleFactor;  // Scale y-coordinate
		vertices[i + 2] *= scaleFactor;  // Scale z-coordinate
	}
}


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	std::vector<GLfloat> vertices;
	if (!loadOBJ("obj/actual.obj", vertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> windowvertices;
	if (!loadOBJ("obj/window.obj", windowvertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> lightRoomvertices;
	if (!loadOBJ("obj/top.obj", lightRoomvertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> concreteground;
	if (!loadOBJ("obj/bottom.obj", concreteground)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> insideVertices;
	if (!loadOBJ("obj/test.obj", insideVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> shrubVertices;
	if (!loadOBJ("obj/shrub.obj", shrubVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> bracketVertices;
	if (!loadOBJ("obj/bracket.obj", bracketVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> roofPanelVertices;
	if (!loadOBJ("obj/roof_panel.obj", roofPanelVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> roadVertices;
	if (!loadOBJ("obj/road.obj", roadVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::vector<GLfloat> beaconVertices;
	if (!loadOBJ("obj/light.obj", beaconVertices)) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	Shader shaderProgram1("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	float cylinderRadius = 2.6f;
	float cylinderBaseRadius = 1.5f; // Initial base radius
	float cylinderHeight = 97.5f; // Increased height
	
	int numSegments = 30;
	float groundRadius = 3.0f; //+ 1.0f; // Radius of the circle around the cylinder
	int numGroundSegments = 8; // Increase for smoother circle
	int numPostCircleSegments = 30; // Number of fence posts in the circle

	float smallerCylinderRadius = 1.5f;
	float smallerCylinderHeight = 0.5f;
	int numSmallerSegments = 30;

	float staircaseWidth = 8.0f;
	float staircaseHeight = 24.0f;
	float staircaseDepth = 4.5f;
	int numSteps = 20;

	//std::vector<float> cylinderVertices = CylinderVertices(cylinderRadius, cylinderBaseRadius, cylinderHeight, numSegments);
	
	std::vector<float> roofVertices = generateHemisphereVertices(0.0f, cylinderHeight / 2.0f, 0.0f, 1.5f, 300);

	// platform with railing
	std::vector<float> platform = generatePlatform(0.0f, 0.0f, groundRadius, numGroundSegments, 43.8f, 3.3f, 2.1f, 0.2f);
	
	//railing
	std::vector<float> groundVertices = generateGroundVertices(0.0f, 0.0f, groundRadius, numGroundSegments, 45.0f);

	// railing posts
	std::vector<float> postCircleVertices = generatePostCircleVertices(0.0f, 0.0f, 3.5f, 89.0f, numPostCircleSegments);
	
	// Ground below the 
	//std::vector<float> concreteground = generatePlatform(0.0f, 0.0f, groundRadius, 360, 5.0f, 3.5f, 2.5f, 0.0f);

	//scaleVertices(concreteground, 2.0f);

	// Generate vertices for the staircase
	std::vector<float> staircaseVertices = generateStaircaseVertices(staircaseWidth, staircaseHeight, staircaseDepth, numSteps);

	scaleVertices(staircaseVertices, 2.2f);
	scaleVertices(vertices, 1.5f);
	scaleVertices(windowvertices, 1.5f);
	scaleVertices(roofVertices, 1.5f);
	scaleVertices(platform, 1.5f);
	scaleVertices(lightRoomvertices, 1.5f);
	scaleVertices(groundVertices, 1.5f);
	scaleVertices(postCircleVertices, 1.5f);
	scaleVertices(concreteground, 1.5f);
	scaleVertices(insideVertices, 1.5f);
	scaleVertices(shrubVertices, 1.5f);
	scaleVertices(bracketVertices, 1.5f);
	scaleVertices(roofPanelVertices, 1.5f);
	scaleVertices(roadVertices, 1.5f);
	scaleVertices(beaconVertices, 1.5f);


	glm::mat4 model = glm::mat4(1.0f); // Identity matrix
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // Scale the model by (0.1, 0.1, 0.1)

	// Apply the scaling to each vertex
	for (size_t i = 0; i < staircaseVertices.size(); i += 3) {
		glm::vec4 vertex = glm::vec4(staircaseVertices[i], staircaseVertices[i + 1], staircaseVertices[i + 2], 1.0f); // Convert vertex to vec4
		vertex = model * vertex; // Apply model transformation
		// Update the vertices with the transformed values
		staircaseVertices[i] = vertex.x;
		staircaseVertices[i + 1] = vertex.y;
		staircaseVertices[i + 2] = vertex.z;
	}

	VAO VAO3;
	VAO3.Bind();
	VBO VBO3(staircaseVertices.data(), staircaseVertices.size() * sizeof(float));
	VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO3.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO3.LinkAttrib(VBO3, 2, 2, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO3.Unbind();
	VBO3.Unbind();


	VAO VAO_floor;
	VAO_floor.Bind();
	size_t stride123 = 8 * sizeof(GLfloat); // 6 floats per vertex (position + normal)

	VBO VBO_floor(concreteground.data(), concreteground.size() * sizeof(float));
	VAO_floor.LinkAttrib(VBO_floor, 0, 3, GL_FLOAT, stride123, (void*)0);
	VAO_floor.LinkAttrib(VBO_floor, 1, 3, GL_FLOAT, stride123, (void*)0);
	VAO_floor.LinkAttrib(VBO_floor, 2, 3, GL_FLOAT, stride123, (void*)(3 * sizeof(float)));
	VAO_floor.LinkAttrib(VBO_floor, 3, 3, GL_FLOAT, stride123, (void*)0);
	VAO_floor.Unbind();
	VBO_floor.Unbind();


	VAO VAO4;
	VAO4.Bind();
	VBO VBO4(vertices.data(), vertices.size() * sizeof(GLfloat));
	// Calculate the stride (total size of all attributes per vertex)
	size_t stride = 8 * sizeof(GLfloat); // 6 floats per vertex (position + normal)
	VAO4.LinkAttrib(VBO4, 0, 3, GL_FLOAT, stride, (void*)0);
	VAO4.LinkAttrib(VBO4, 1, 3, GL_FLOAT, stride, (void*)(0 * sizeof(GLfloat)));
	VAO4.LinkAttrib(VBO4, 2, 3, GL_FLOAT, stride, (void*)(3 * sizeof(GLfloat)));
	VAO4.LinkAttrib(VBO4, 3, 3, GL_FLOAT, stride, (void*)(0 * sizeof(GLfloat)));
	VBO4.Unbind();


	size_t stride1 = 8 * sizeof(GLfloat); // 6 floats per vertex (position + normal)

	VAO VAO_bracket;
	VAO_bracket.Bind();
	VBO VBO_bracket(windowvertices.data(), windowvertices.size() * sizeof(float));
	VAO_bracket.LinkAttrib(VBO_bracket, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_bracket.LinkAttrib(VBO_bracket, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_bracket.LinkAttrib(VBO_bracket, 2, 3, GL_FLOAT, stride1, (void*)(3 * sizeof(float)));
	VAO_bracket.LinkAttrib(VBO_bracket, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_bracket.Unbind();
	VAO_bracket.Unbind();


	VAO VAO_lightroom;
	VAO_lightroom.Bind();
	VBO VBO_lightroom(lightRoomvertices.data(), lightRoomvertices.size() * sizeof(float));
	VAO_lightroom.LinkAttrib(VBO_lightroom, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_lightroom.LinkAttrib(VBO_lightroom, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_lightroom.LinkAttrib(VBO_lightroom, 2, 3, GL_FLOAT, stride1, (void*)0);
	VAO_lightroom.LinkAttrib(VBO_lightroom, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_lightroom.Unbind();
	VAO_lightroom.Unbind();

	VAO VAO_Roof;
	VAO_Roof.Bind();
	VBO VBO_Roof(roofVertices.data(), roofVertices.size() * sizeof(float));
	VAO_Roof.LinkAttrib(VBO_Roof, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Roof.LinkAttrib(VBO_Roof, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Roof.LinkAttrib(VBO_Roof, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Roof.LinkAttrib(VBO_Roof, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Roof.Unbind();
	VBO_Roof.Unbind();

	VAO VAO_Platform;
	VAO_Platform.Bind();
	VBO VBO_Platform(platform.data(), platform.size() * sizeof(float));
	VAO_Platform.LinkAttrib(VBO_Platform, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Platform.LinkAttrib(VBO_Platform, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)3);
	VAO_Platform.LinkAttrib(VBO_Platform, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Platform.LinkAttrib(VBO_Platform, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Platform.Unbind();
	VBO_Platform.Unbind();

	VAO VAO2;
	VAO2.Bind();
	std::vector<float> fencePostVertices;
	float fencePostYOffset = 17.0f; // Move fence posts down by 1 unit on the y-axis
	glm::vec3 cylinderCenter(0.0f, 0.0f, 0.0f); // Center of the cylinder
	float circleRadius = cylinderRadius + 10.0f; // Radius of the circle around the cylinder
	int numPosts = 100; // Number of fence posts

	// Define the 8 vertices of the octagon
	std::vector<glm::vec3> octagonVertices;
	for (int i = 0; i < 8; ++i) {
		float angle = 2.0f * M_PI * i / 8;
		float xPosition = cylinderCenter.x + circleRadius * cos(angle);
		float zPosition = cylinderCenter.z + circleRadius * sin(angle);
		octagonVertices.push_back(glm::vec3(xPosition, 0.0f, zPosition));
	}

	// Calculate the number of posts per edge
	int postsPerEdge = numPosts / 8;
	float edgeFraction = 1.0f / postsPerEdge;

	for (int i = 0; i < 8; ++i) {
		glm::vec3 start = octagonVertices[i];
		glm::vec3 end = octagonVertices[(i + 1) % 8];
		for (int j = 0; j < postsPerEdge; ++j) {
			float t = j * edgeFraction;
			float xPosition = start.x + t * (end.x - start.x);
			float zPosition = start.z + t * (end.z - start.z);
			std::vector<float> postVertices = generateFencePostVertices(xPosition, fencePostYOffset, zPosition);
			fencePostVertices.insert(fencePostVertices.end(), postVertices.begin(), postVertices.end());
		}
	}
	VBO VBO2(fencePostVertices.data(), fencePostVertices.size() * sizeof(float));
	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO2.Unbind();
	VBO2.Unbind();

	VAO VAO_Ground;
	VAO_Ground.Bind();
	VBO VBO_Ground(groundVertices.data(), groundVertices.size() * sizeof(float));
	VAO_Ground.LinkAttrib(VBO_Ground, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO_Ground, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO_Ground, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO_Ground, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO_Ground, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.Unbind();
	VBO_Ground.Unbind();

	VAO VAO_PostCircle;
	VAO_PostCircle.Bind();
	VBO VBO_PostCircle(postCircleVertices.data(), postCircleVertices.size() * sizeof(float));
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.Unbind();
	VBO_PostCircle.Unbind();


	VAO VAO_Inside;
	VAO_Inside.Bind();
	VBO VBO_Inside(insideVertices.data(), insideVertices.size() * sizeof(float));
	VAO_Inside.LinkAttrib(VBO_Inside, 0, 3, GL_FLOAT, stride1, (void*)0); // Vertex positions
	VAO_Inside.LinkAttrib(VBO_Inside, 1, 3, GL_FLOAT, stride1, (void*)(0 * sizeof(float))); // Vertex normals
	VAO_Inside.LinkAttrib(VBO_Inside, 2, 3, GL_FLOAT, stride1, (void*)(0 * sizeof(float))); // Vertex texture coordinates
	VAO_Inside.LinkAttrib(VBO_Inside, 3, 3, GL_FLOAT, stride1, (void*)(0 * sizeof(float))); // Vertex colors
	VAO_Inside.Unbind();
	VBO_Inside.Unbind();


	VAO VAO_Shrub;
	VAO_Shrub.Bind();
	VBO VBO_Shrub(shrubVertices.data(), shrubVertices.size() * sizeof(float));
	VAO_Shrub.LinkAttrib(VBO_Shrub, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Shrub.LinkAttrib(VBO_Shrub, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Shrub.LinkAttrib(VBO_Shrub, 2, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Shrub.LinkAttrib(VBO_Shrub, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Shrub.Unbind();
	VBO_Shrub.Unbind();

	VAO VAO_Support;
	VAO_Support.Bind();
	VBO VBO_Support(bracketVertices.data(), bracketVertices.size() * sizeof(float));
	VAO_Support.LinkAttrib(VBO_Support, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Support.LinkAttrib(VBO_Support, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Support.LinkAttrib(VBO_Support, 2, 3, GL_FLOAT, stride1, (void*)(3 * sizeof(float)));
	VAO_Support.LinkAttrib(VBO_Support, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_Support.Unbind();
	VBO_Shrub.Unbind();

	VAO VAO_roof_panel;
	VAO_roof_panel.Bind();
	VBO VBO_roof_panel(roofPanelVertices.data(), roofPanelVertices.size() * sizeof(float));
	VAO_roof_panel.LinkAttrib(VBO_roof_panel, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_roof_panel.LinkAttrib(VBO_roof_panel, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_roof_panel.LinkAttrib(VBO_roof_panel, 2, 3, GL_FLOAT, stride1, (void*)0);
	VAO_roof_panel.LinkAttrib(VBO_roof_panel, 2, 3, GL_FLOAT, stride1, (void*)0);
	VAO_roof_panel.Unbind();
	VBO_roof_panel.Unbind();

	VAO VAO_road;
	VAO_road.Bind();
	VBO VBO_road(roadVertices.data(), roadVertices.size() * sizeof(float));
	VAO_road.LinkAttrib(VBO_road, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_road.LinkAttrib(VBO_road, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_road.LinkAttrib(VBO_road, 2, 3, GL_FLOAT, stride1, (void*)0);
	VAO_road.LinkAttrib(VBO_road, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_road.Unbind();
	VBO_road.Unbind();

	VAO VAO_beacon;
	VAO_beacon.Bind();
	VBO VBO_beacon(beaconVertices.data(), beaconVertices.size() * sizeof(float));
	VAO_beacon.LinkAttrib(VBO_beacon, 0, 3, GL_FLOAT, stride1, (void*)0);
	VAO_beacon.LinkAttrib(VBO_beacon, 1, 3, GL_FLOAT, stride1, (void*)0);
	VAO_beacon.LinkAttrib(VBO_beacon, 2, 3, GL_FLOAT, stride1, (void*)3);
	VAO_beacon.LinkAttrib(VBO_beacon, 3, 3, GL_FLOAT, stride1, (void*)0);
	VAO_beacon.Unbind();
	VBO_beacon.Unbind();

	
	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");
	// Generates Vertex Array Object and binds it

	VAO lightVAO;
	lightVAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	// Generates Element Buffer Object and links it to indices
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	// Links VBO attributes such as coordinates and colors to VAO
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();



	VAO lightVAO1;
	lightVAO1.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO lightVBO1(lightVertices1, sizeof(lightVertices1));
	// Generates Element Buffer Object and links it to indices
	EBO lightEBO1(lightIndices1, sizeof(lightIndices1));
	// Links VBO attributes such as coordinates and colors to VAO
	lightVAO.LinkAttrib(lightVBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO1, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.LinkAttrib(lightVBO1, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	// Unbind all to prevent accidentally modifying them
	lightVAO1.Unbind();
	lightVBO1.Unbind();
	lightEBO1.Unbind();

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(32.0f, 80.5f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec4 secondLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 secondLightPos = glm::vec3(-62.0f, 50.5f, 20.0f);
	glm::mat4 beaconModel = glm::mat4(1.0f);
	beaconModel = glm::translate(beaconModel, secondLightPos);


	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);


	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform4f(glGetUniformLocation(lightShader.ID, "secondLightColor"), secondLightColor.x, secondLightColor.y, secondLightColor.z, secondLightColor.w);

	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "secondLightPos"), secondLightPos.x, secondLightPos.y, secondLightPos.z);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "secondLightColor"), secondLightColor.x, secondLightColor.y, secondLightColor.z, secondLightColor.w);


	/*
	* I'm doing this relative path thing in order to centralize all the resources into one folder and not
	* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
	* folder and then give a relative path from this folder to whatever resource you want to get to.
	* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
	*/
	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string texPath = "/YoutubeOpenGL 9 - Lighting/textures/";

	// Texture
	Texture brickTex((parentDir + texPath + "lighthouse_texture.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	Texture inside((parentDir + texPath + "brick.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	inside.texUnit(shaderProgram, "tex10", 0);

	Texture rail((parentDir + texPath + "railing.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	rail.texUnit(shaderProgram, "tex1", 0);

	Texture glass((parentDir + texPath + "window1.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	glass.texUnit(shaderProgram, "tex2", 0);

	Texture roof((parentDir + texPath + "roof_texture.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	roof.texUnit(shaderProgram, "tex3", 0);

	Texture roof1((parentDir + texPath + "roof.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	roof1.texUnit(shaderProgram, "tex3", 0);

	Texture metal((parentDir + texPath + "metal.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	metal.texUnit(shaderProgram, "tex4", 0);

	Texture concrete((parentDir + texPath + "roof_texture.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	concrete.texUnit(shaderProgram, "tex5", 0);

	Texture stairs((parentDir + texPath + "staircase.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	stairs.texUnit(shaderProgram, "tex6", 0);

	Texture platform_base((parentDir + texPath + "platform.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	platform_base.texUnit(shaderProgram, "tex6", 0);

	Texture shrub((parentDir + texPath + "shrub.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	shrub.texUnit(shaderProgram, "tex7", 0);

	Texture test((parentDir + texPath + "123.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	test.texUnit(shaderProgram, "tex11", 0);

	Texture road((parentDir + texPath + "road.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	road.texUnit(shaderProgram, "tex12", 0);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); // Rotate around y-axis
	float rotationSpeed = 1.5f; // Adjust the speed of rotation
	float rotationAngle = 60.0f; // Initialize rotation angle


	// Main while loop
	while (!glfwWindowShouldClose(window))
{
    // Specify the color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Handles camera inputs
    camera.Inputs(window);
    // Updates and exports the camera matrix to the Vertex Shader
    camera.updateMatrix(45.0f, 0.1f, 1000.0f);

    // Tells OpenGL which Shader Program we want to use
    shaderProgram.Activate();
    // Exports the camera Position to the Fragment Shader for specular lighting
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    // Export the camMatrix to the Vertex Shader of the pyramid
    camera.Matrix(shaderProgram, "camMatrix");
    // Binds texture so that it appears in rendering

    // Set uniforms for the first light
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glUniform4f(glGetUniformLocation(shaderProgram.ID, "secondLightColor"), secondLightColor.x, secondLightColor.y, secondLightColor.z, secondLightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "secondLightPos"), secondLightPos.x, secondLightPos.y, secondLightPos.z);

    // Render objects
	stairs.Bind();
    VAO3.Bind();
   // glDrawArrays(GL_TRIANGLES, 0, staircaseVertices.size() / 3);
    VAO3.Unbind();
	stairs.Unbind();

    VAO2.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, fencePostVertices.size() / 3);
    VAO2.Unbind();

    roof.Bind();
    VAO_Roof.Bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, roofVertices.size() / 3);
    VAO_Roof.Unbind();

	platform_base.Bind();
    VAO_Platform.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, platform.size() / 3);
    VAO_Platform.Unbind();
	platform_base.Unbind();

	rail.Bind();
    VAO_Ground.Bind();
    // Railing
    glDrawArrays(GL_TRIANGLE_STRIP, 0, groundVertices.size() / 3);
    VAO_Ground.Unbind();
	rail.Unbind();

    rail.Bind();
    VAO_PostCircle.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, postCircleVertices.size() / 3);
    VAO_PostCircle.Unbind();
    rail.Unbind();

	brickTex.Bind();
    VAO_bracket.Bind();
    glDrawArrays(GL_TRIANGLES, 0, windowvertices.size() / 3);
    VAO_bracket.Unbind();
	brickTex.Unbind();

	roof.Bind();
	VAO_roof_panel.Bind();
	glDrawArrays(GL_TRIANGLES, 0, roofPanelVertices.size() / 3);
	VAO_roof_panel.Unbind();
	roof.Unbind();

	roof.Bind();
    VAO_lightroom.Bind();
    glDrawArrays(GL_TRIANGLES, 0, lightRoomvertices.size() / 3);
    VAO_lightroom.Unbind();
	roof.Unbind();

    brickTex.Bind();
    VAO4.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
    VAO4.Unbind();

	inside.Bind();
	VAO_Inside.Bind();
	glDrawArrays(GL_TRIANGLES, 0, insideVertices.size() / 8);
	VAO_Inside.Unbind();

	concrete.Bind();
    VAO_floor.Bind();
    glDrawArrays(GL_TRIANGLES, 0, concreteground.size() / 3);
    VAO_floor.Unbind();
	concrete.Unbind();

	shrub.Bind();
	VAO_Shrub.Bind();
	glDrawArrays(GL_TRIANGLES, 0, shrubVertices.size() / 3);
	VAO_Shrub.Unbind();
	shrub.Unbind();

	metal.Bind();
	VAO_Support.Bind();
	glDrawArrays(GL_TRIANGLES, 0, bracketVertices.size() / 3);
	VAO_Support.Unbind();
	metal.Unbind();

	metal.Bind();
	VAO_beacon.Bind();
	glDrawArrays(GL_TRIANGLES, 0, beaconVertices.size() / 3);
	VAO_beacon.Unbind();
	metal.Unbind();

    // Tells OpenGL which Shader Program we want to use for the light source
    lightShader.Activate();
    // Export the camMatrix to the Vertex Shader of the light cube
    camera.Matrix(lightShader, "camMatrix");

	rotationAngle += rotationSpeed;

    // Render the first light source
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    lightVAO.Bind();
    glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

	glm::mat4 beaconModel = glm::mat4(1.0f);
	beaconModel = glm::translate(lightModel, secondLightPos);

	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(beaconModel));
	lightVAO1.Bind();
	glDrawElements(GL_TRIANGLES, sizeof(lightIndices1) / sizeof(int), GL_UNSIGNED_INT, 0);

    // Swap the back buffer with the front buffer
    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
}

	// Delete all the objects we've created
	//EBO1.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();
	lightShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}