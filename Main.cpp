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
#define M_PI 3.14159265359


const unsigned int width = 800;
const unsigned int height = 800;


std::vector<float> CylinderVertices(float bottomRadius, float topRadius, float height, int numSegments) {
	std::vector<float> vertices;

	// Generate vertices for the sides of the cylinder
	for (int i = 0; i < 500; ++i) {
		float theta1 = 2.5 * M_PI * i / 500;
		float theta2 = 2.5 * M_PI * (i + 1) / 500;  //numsegments

		float circle = 2.0 * M_PI * (1.5 * 1.5);

		float x1_bottom = bottomRadius * cos(theta1);
		float z1_bottom = bottomRadius * sin(theta1);
		float x2_bottom = bottomRadius * cos(theta2);
		float z2_bottom = bottomRadius * sin(theta2);

		float x1_top = topRadius * cos(theta1);
		float z1_top = topRadius * sin(theta1);
		float x2_top = topRadius * cos(theta2);
		float z2_top = topRadius * sin(theta2);

		// Vertices for the top part
		//vertices.push_back(x1_top);
		//vertices.push_back(height / 2.0f);
		//vertices.push_back(z1_top);

		//vertices.push_back(0.0f); // Top center
		//vertices.push_back(height / 2.0f);
		//vertices.push_back(0.0f);

		//vertices.push_back(x2_top);
		//vertices.push_back(height / 2.0f);
		//vertices.push_back(z2_top);

		// Vertices for the first triangle of the side quad
		vertices.push_back(x1_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z1_bottom);
		vertices.push_back(x1_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z1_top);
		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z2_bottom);

		// Vertices for the second triangle of the side quad
		vertices.push_back(x2_bottom);
		vertices.push_back(-height / 2.0f);
		vertices.push_back(z2_bottom);
		vertices.push_back(x1_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z1_top);
		vertices.push_back(x2_top);
		vertices.push_back(height / 2.0f);
		vertices.push_back(z2_top);
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

std::vector<float> generatePlatform(float centerX, float centerY, float radius, int numSegments, float yaxis) {
	std::vector<float> vertices;
	float outerRadius = 2.5f;
	float innerRadius = 1.7f;
	// Define the thickness of the platform
	float thickness = 0.1f;
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
		vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Top vertices
		vertices.push_back(xOuter);
		vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);

		vertices.push_back(xInner);
		vertices.push_back(8.0f - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(8.0f - innerThickness / 2.0f); // Set the inner y-coordinate to the bottom of the ground
		vertices.push_back(nextZInner);

		// Top vertices
		vertices.push_back(xInner);
		vertices.push_back(8.0f + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(zInner);
		vertices.push_back(nextXInner);
		vertices.push_back(8.0f + innerThickness / 2.0f); // Set the inner y-coordinate to the top of the ground
		vertices.push_back(nextZInner);

		// Side faces
		// First triangle
		vertices.push_back(xOuter);
		vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(zOuter);
		vertices.push_back(xOuter);
		vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);

		// Second triangle
		vertices.push_back(nextXOuter);
		vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
		vertices.push_back(nextZOuter);
		vertices.push_back(xOuter);
		vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(zOuter);
		vertices.push_back(nextXOuter);
		vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
		vertices.push_back(nextZOuter);


	}

	return vertices;
}

std::vector<float> createBracket(float centerX, float centerY, float height, float width, float thickness) {
	std::vector<float> vertices;

	// Vertical leg of the bracket
	float vTopY = centerY + height / 2.0f;
	float vBottomY = centerY - height / 2.0f;
	float vLeftX = centerX - width / 2.0f;
	float vRightX = centerX + width / 2.0f;
	float vFrontZ = thickness / 2.0f;
	float vBackZ = -thickness / 2.0f;

	// Vertical face
	// Front face
	vertices.push_back(vLeftX); vertices.push_back(vTopY); vertices.push_back(vFrontZ);
	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vFrontZ);
	vertices.push_back(vRightX); vertices.push_back(vBottomY); vertices.push_back(vFrontZ);

	vertices.push_back(vLeftX); vertices.push_back(vTopY); vertices.push_back(vFrontZ);
	vertices.push_back(vRightX); vertices.push_back(vBottomY); vertices.push_back(vFrontZ);
	vertices.push_back(vLeftX); vertices.push_back(vBottomY); vertices.push_back(vFrontZ);

	// Back face
	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vBackZ);
	vertices.push_back(vLeftX); vertices.push_back(vTopY); vertices.push_back(vBackZ);
	vertices.push_back(vLeftX); vertices.push_back(vBottomY); vertices.push_back(vBackZ);

	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vBackZ);
	vertices.push_back(vLeftX); vertices.push_back(vBottomY); vertices.push_back(vBackZ);
	vertices.push_back(vRightX); vertices.push_back(vBottomY); vertices.push_back(vBackZ);

	// Horizontal arm of the bracket
	float hTopY = vTopY + thickness;
	float hBottomY = centerY + height / 2.0f;
	float hLeftX = centerX + width / 2.0f;
	float hRightX = hLeftX + height / 2.0f;  // Adjust the width of the arm

	// Front face
	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vFrontZ);
	vertices.push_back(hLeftX); vertices.push_back(hTopY); vertices.push_back(vFrontZ);
	vertices.push_back(hRightX); vertices.push_back(hBottomY); vertices.push_back(vFrontZ);

	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vFrontZ);
	vertices.push_back(hRightX); vertices.push_back(hBottomY); vertices.push_back(vFrontZ);
	vertices.push_back(vRightX); vertices.push_back(vBottomY); vertices.push_back(vFrontZ);

	// Back face
	vertices.push_back(hRightX); vertices.push_back(hBottomY); vertices.push_back(vBackZ);
	vertices.push_back(hLeftX); vertices.push_back(hTopY); vertices.push_back(vBackZ);
	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vBackZ);

	vertices.push_back(vRightX); vertices.push_back(vTopY); vertices.push_back(vBackZ);
	vertices.push_back(hRightX); vertices.push_back(hBottomY); vertices.push_back(vBackZ);
	vertices.push_back(vRightX); vertices.push_back(vBottomY); vertices.push_back(vBackZ);

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
		float yOffset = 11.5f;

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
	float height = 0.8;
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
	float outerRadius = 2.5f;
	float innerRadius = 2.45f;
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
	angleIncrement *= 8.0f / 50;

	// Generate vertices for each post
	for (int i = 0; i < 50; ++i) {
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
	-0.5f, -1.1f,  0.1f,
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

std::vector<float> generateRectangleVertices(float size, float width, float depth, float offsetX = -2.1f) {
	std::vector<float> vertices;

	// Define half size for cleaner calculations
	float halfSize = size / 2.0f;

	// Define the vertices of the square
	vertices.push_back(offsetX); // Top left
	vertices.push_back(halfSize); // Y-coordinate (unchanged)
	vertices.push_back(-halfSize); // Z-coordinate

	vertices.push_back(offsetX); // Top right
	vertices.push_back(halfSize); // Y-coordinate (unchanged)
	vertices.push_back(halfSize); // Z-coordinate

	vertices.push_back(offsetX); // Bottom right
	vertices.push_back(-halfSize); // Y-coordinate (unchanged)
	vertices.push_back(halfSize); // Z-coordinate

	vertices.push_back(offsetX); // Bottom left
	vertices.push_back(-halfSize); // Y-coordinate (unchanged)
	vertices.push_back(-halfSize); // Z-coordinate

	return vertices;
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



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	Shader shaderProgram1("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	float cylinderRadius = 2.6f;
	float cylinderBaseRadius = 1.5f; // Initial base radius
	float cylinderHeight = 22.0f; // Increased height
	int numSegments = 30;
	float groundRadius = 2.5f; //+ 1.0f; // Radius of the circle around the cylinder
	int numGroundSegments = 8; // Increase for smoother circle
	int numPostCircleSegments = 30; // Number of fence posts in the circle

	float smallerCylinderRadius = 1.2f;
	float smallerCylinderHeight = 0.5f;
	int numSmallerSegments = 30;

	std::vector<float> cylinderVertices = CylinderVertices(cylinderRadius, cylinderBaseRadius, cylinderHeight, numSegments);
	std::vector<float> roofVertices = generateHemisphereVertices(0.0f, cylinderHeight / 2.0f, 0.0f, 1.1f, 300);
	std::vector<float> platform = generatePlatform(0.0f, 0.0f, groundRadius, numGroundSegments, 8.0f);
	std::vector<float> smallerCylinderVertices = generateCylinderVertices1(smallerCylinderRadius, smallerCylinderRadius, smallerCylinderHeight, numSmallerSegments);
	std::vector<float> groundVertices = generateGroundVertices(0.0f, 0.0f, groundRadius, numGroundSegments, 9.0f);
	std::vector<float> postCircleVertices = generatePostCircleVertices(0.0f, 0.0f, groundRadius, 17.0f, numPostCircleSegments);
	std::vector<float> bracket = createBracket(2.0f, 7.5f, 0.5f, 0.5f, 0.1f);
	std::vector<float> rectangle = generateRectangleVertices(3.0f, 1.0f, 1.0f);

	VAO VAO4;
	VAO4.Bind();

	// Create VBO for railing vertices
	VBO VBO4(rectangle.data(), rectangle.size() * sizeof(float));

	// Link vertex attributes
	VAO4.LinkAttrib(VBO4, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO4.LinkAttrib(VBO4, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO4.LinkAttrib(VBO4, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO4.LinkAttrib(VBO4, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	VBO4.Unbind();

	VAO VAO_bracket;
	VAO_bracket.Bind();
	VBO VBO_bracket(bracket.data(), bracket.size() * sizeof(float));
	VAO_bracket.LinkAttrib(VBO_bracket, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_bracket.LinkAttrib(VBO_bracket, 4, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_bracket.LinkAttrib(VBO_bracket, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_bracket.LinkAttrib(VBO_bracket, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_bracket.Unbind();
	VAO_bracket.Unbind();


	VAO VAO1;
	VAO1.Bind();
	VBO VBO1(cylinderVertices.data(), cylinderVertices.size() * sizeof(float));
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO1.Unbind();
	VBO1.Unbind();

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
	VAO_Platform.LinkAttrib(VBO_Platform, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Platform.LinkAttrib(VBO_Platform, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Platform.LinkAttrib(VBO_Platform, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	VAO_Platform.Unbind();
	VBO_Platform.Unbind();

	VAO VAO_SmallerCylinder;
	VAO_SmallerCylinder.Bind();
	VBO VBO_SmallerCylinder(smallerCylinderVertices.data(), smallerCylinderVertices.size() * sizeof(float));
	VAO_SmallerCylinder.LinkAttrib(VBO_SmallerCylinder, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_SmallerCylinder.LinkAttrib(VBO_SmallerCylinder, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_SmallerCylinder.LinkAttrib(VBO_SmallerCylinder, 2, 2, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_SmallerCylinder.LinkAttrib(VBO_SmallerCylinder, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_SmallerCylinder.Unbind();
	VBO_SmallerCylinder.Unbind();

	VAO VAO2;
	VAO2.Bind();
	std::vector<float> fencePostVertices;
	float fencePostYOffset = -9.0f; // Move fence posts down by 1 unit on the y-axis
	glm::vec3 cylinderCenter(0.0f, 0.0f, 0.0f); // Center of the cylinder
	float circleRadius = cylinderRadius + 5.0f; // Radius of the circle around the cylinder
	int numPosts = 100; // Number of fence posts
	for (int i = 0; i < numPosts; ++i) {
		float angle = 2.0f * M_PI * i / numPosts; // Angle for placing posts evenly around the cylinder
		float xPosition = cylinderCenter.x + circleRadius * cos(angle);
		float zPosition = cylinderCenter.z + circleRadius * sin(angle);
		std::cout << "xPosition: " << xPosition << std::endl;
		std::cout << "zPosition: " << zPosition << std::endl;
		std::vector<float> postVertices = generateFencePostVertices(xPosition, fencePostYOffset, zPosition);
		fencePostVertices.insert(fencePostVertices.end(), postVertices.begin(), postVertices.end());

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
	VAO_Ground.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	VAO_Ground.Unbind();
	VBO_Ground.Unbind();


	VAO VAO_PostCircle;
	VAO_PostCircle.Bind();
	VBO VBO_PostCircle(postCircleVertices.data(), postCircleVertices.size() * sizeof(float));
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.LinkAttrib(VBO_PostCircle, 3, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_PostCircle.Unbind();
	VBO_PostCircle.Unbind();

	/*VAO VAO_Ground;
	VAO_Ground.Bind();
	VBO VBO_Ground(groundVertices.data(), groundVertices.size() * sizeof(float));
	VAO_Ground.LinkAttrib(VBO_Ground, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO_Ground.Unbind();
	VBO_Ground.Unbind();*/
	
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
	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();


	//Shader lightShader1("light.vert", "light.frag");
	//// Generates Vertex Array Object and binds it
	//VAO lightVAO1;
	//lightVAO1.Bind();
	//// Generates Vertex Buffer Object and links it to vertices
	//VBO lightVBO1(lightVertices1, sizeof(lightVertices1));
	//// Generates Element Buffer Object and links it to indices
	//EBO lightEBO1(lightIndices1, sizeof(lightIndices1));
	//// Links VBO attributes such as coordinates and colors to VAO
	//lightVAO1.LinkAttrib(lightVBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	//// Unbind all to prevent accidentally modifying them
	//lightVAO1.Unbind();
	//lightVBO1.Unbind();
	//lightEBO1.Unbind();


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(22.5f, 15.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	//glm::vec3 lightPos1 = glm::vec3(-10.5f, 15.5f, 0.5f);
	//glm::mat4 lightModel1 = glm::mat4(1.0f);
	//lightModel1 = glm::translate(lightModel1, lightPos1);

	//glm::vec3 pyramidPos1 = glm::vec3(-3.0f, 0.0f, 0.0f);
	//glm::mat4 pyramidModel1 = glm::mat4(1.0f);
	//pyramidModel1 = glm::translate(pyramidModel1, pyramidPos1);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	/*lightShader1.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader1.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel1));
	glUniform4f(glGetUniformLocation(lightShader1.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram1.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram1.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel1));
	glUniform4f(glGetUniformLocation(shaderProgram1.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram1.ID, "lightPos"), lightPos1.x, lightPos1.y, lightPos1.z);*/




	/*
	* I'm doing this relative path thing in order to centralize all the resources into one folder and not
	* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
	* folder and then give a relative path from this folder to whatever resource you want to get to.
	* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
	*/
	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string texPath = "/Resources/YoutubeOpenGL 7 - Going 3D/";

	// Texture
	Texture brickTex((parentDir + texPath + "img2.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	Texture rail((parentDir + texPath + "rai.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	rail.texUnit(shaderProgram, "tex1", 0);

	Texture glass((parentDir + texPath + "window1.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	glass.texUnit(shaderProgram, "tex2", 0);

	Texture roof((parentDir + texPath + "roof.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	roof.texUnit(shaderProgram, "tex3", 0);

	Texture metal((parentDir + texPath + "metal.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	metal.texUnit(shaderProgram, "tex4", 0);
	// Original code from the tutorial
	/*Texture brickTex("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);*/



	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

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
		camera.updateMatrix(45.0f, 0.1f, 100.0f);


		// Tells OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Exports the camera Position to the Fragment Shader for specular lighting
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		// Export the camMatrix to the Vertex Shader of the pyramid
		camera.Matrix(shaderProgram, "camMatrix");
		// Binds texture so that is appears in rendering
		brickTex.Bind();

		VAO1.Bind();
		glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size() / 3);
		VAO1.Unbind();

		VAO2.Bind();
		glDrawArrays(GL_TRIANGLES, 0, fencePostVertices.size() / 3);
		VAO2.Unbind();

		roof.Bind();
		VAO_Roof.Bind();
		glDrawArrays(GL_TRIANGLE_FAN, 0, roofVertices.size() / 3);
		VAO_Roof.Unbind();

		rail.Bind();
		VAO_Platform.Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, platform.size() / 3);
		VAO_Platform.Unbind();

		glass.Bind();
		VAO_SmallerCylinder.Bind();
		glDrawArrays(GL_TRIANGLES, 0, smallerCylinderVertices.size() / 3);
		VAO_SmallerCylinder.Unbind();

		VAO_Ground.Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, groundVertices.size() / 3);
		VAO_Ground.Unbind();

		rail.Bind();
		VAO_PostCircle.Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, postCircleVertices.size() / 3);
		VAO_PostCircle.Unbind();
		rail.Unbind();

		VAO_bracket.Bind();
		glDrawArrays(GL_TRIANGLES, 0, bracket.size() / 3);
		VAO_bracket.Unbind();

		glass.Bind();
		VAO4.Bind();
		glDrawArrays(GL_TRIANGLE_FAN, 0, rectangle.size() / 3);
		VAO4.Unbind();
		glass.Unbind();

		// Tells OpenGL which Shader Program we want to use
		lightShader.Activate();
		// Export the camMatrix to the Vertex Shader of the light cube
		camera.Matrix(lightShader, "camMatrix");
		// Bind the VAO so OpenGL knows to use it
		lightVAO.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		//lightVAO1.Bind();
		//// Draw primitives, number of indices, datatype of indices, index of indices
		//glDrawElements(GL_TRIANGLES, sizeof(lightIndices1) / sizeof(int), GL_UNSIGNED_INT, 0);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
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