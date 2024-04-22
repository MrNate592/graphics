#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

#include <vector>
#include <cmath>

//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------



const unsigned int width = 800;
const unsigned int height = 800;
#define M_PI 3.14159265359

// Function to generate vertices for a cylinder with varying radius
std::vector<float> generateCylinderVertices(float bottomRadius, float topRadius, float height, int numSegments) {
    std::vector<float> vertices;

    // Generate vertices for the bottom circle
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2 * M_PI * i / numSegments;
        float x = bottomRadius * cos(theta);
        float z = bottomRadius * sin(theta);

        vertices.push_back(x);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);
    }

    // Generate vertices for the top circle
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2 * M_PI * i / numSegments;
        float x = topRadius * cos(theta);
        float z = topRadius * sin(theta);

        vertices.push_back(x);
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);
    }

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

// Function to generate vertices for a fence post
std::vector<float> generateFencePostVertices(float xPosition, float yPosition, float zPosition) {
    // Define fence post dimensions
    float width = 0.2f;
    float height = 2.0f;
    float depth = 0.1f;

    // Calculate vertices for the fence post
    std::vector<float> vertices = {
        // Front face
        -width / 2.0f + xPosition, -height / 2.0f + yPosition,  depth / 2.0f + zPosition,  // Bottom left
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


//std::vector<float> generateGroundVertices(float width, float depth) {
//    std::vector<float> vertices = {
//        // Front face
//        -width / 2.0f, -10.0f,  depth / 2.0f,  // Bottom left
//         width / 2.0f, -10.0f,  depth / 2.0f,  // Bottom right
//         width / 2.0f, -10.0f, -depth / 2.0f,  // Top right
//        -width / 2.0f, -10.0f, -depth / 2.0f   // Top left
//    };
//    return vertices;
//}

std::vector<float> generateGroundVertices(float centerX, float centerY, float radius, int numSegments) {
    std::vector<float> vertices;

    // Define the thickness of the ground
    float thickness = 0.5f;

    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments; // Angle for placing points evenly around the center
        float x = centerX + radius * cos(angle);
        float z = centerY + radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(8.0f - thickness / 2.0f); // Set the y-coordinate to the bottom of the ground
        vertices.push_back(z);

        vertices.push_back(x);
        vertices.push_back(8.0f + thickness / 2.0f); // Set the y-coordinate to the top of the ground
        vertices.push_back(z);

        // Repeat the first vertex to close the circle smoothly
        if (i == numSegments - 1) {
            vertices.push_back(vertices[0]);
            vertices.push_back(vertices[1]);
            vertices.push_back(vertices[2]);
            vertices.push_back(vertices[3]);
            vertices.push_back(vertices[4]);
            vertices.push_back(vertices[5]);
        }
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
            vertices.push_back(y1);
            vertices.push_back(z1);

            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
        }
    }

    return vertices;
}

std::vector<float> generatePostCircleVertices(float centerX, float centerY, float radius, float height, int numPosts) {
    std::vector<float> vertices;

    for (int i = 0; i < numPosts; ++i) {
        float angle = 2.0f * M_PI * i / numPosts; // Angle for placing posts evenly around the circle
        float x = centerX + radius * cos(angle);
        float z = centerY + radius * sin(angle);

   
        // Calculate vertices for the post at the current angle
        std::vector<float> postVertices = generateFencePostVertices(x, height / 2.0f, z);
        vertices.insert(vertices.end(), postVertices.begin(), postVertices.end());
    }

    return vertices;
}

#include <cmath> // Include cmath for trigonometric functions

std::vector<float> generateStaircaseVertices(float staircaseWidth, float staircaseHeight, float staircaseDepth, int numSteps) {
    std::vector<float> vertices;

    // Calculate dimensions of each step
    float stepWidth = staircaseWidth;
    float stepHeight = staircaseHeight / numSteps;
    float stepDepth = staircaseDepth;

    // Angle for rotating each step
    float angleIncrement = 360.0f / numSteps; // Full circle for one revolution

    // Radius of the cylinder
    float radius = 5.5f;

    // Generate vertices for each step, including all faces
    for (int i = 0; i < numSteps; ++i) {
        // Calculate rotation angle for current step
        float angle = i * angleIncrement;
        float angleRad = angle * M_PI / 180.0f; // Convert to radians

        // Calculate coordinates of the center of the step
        float centerX = radius * cos(angleRad);
        float centerY = i * stepHeight;
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

    return vertices;
}


int main() {
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
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, width, height);

    // Generates Shader object using shaders default.vert and default.frag
    Shader shaderProgram("default.vert", "default.frag");

    // Define cylinder parameters
    float cylinderRadius = 2.6f;
    float cylinderBaseRadius = 1.5f; // Initial base radius
    float cylinderHeight = 22.0f; // Increased height
    int numSegments = 30;
    float postHeight = 0.1f;
    int numPostCircleSegments = 50; // Number of fence posts in the circle
    
    float staircaseWidth = 3.0f;
    float staircaseHeight = 20.0f;
    float staircaseDepth = 5.0f;
    int numSteps = 30;

    // Generate vertices for the staircase
    std::vector<float> staircaseVertices = generateStaircaseVertices(staircaseWidth, staircaseHeight, staircaseDepth, numSteps);

    // Generate vertices for the staircase
    //std::vector<float> staircaseVertices = generateStaircaseVertices(5.0f, cylinderBaseRadius, staircaseHeight, numSteps);

    // Generate vertices for the cylinder
    std::vector<float> cylinderVertices = generateCylinderVertices(cylinderRadius, cylinderBaseRadius, cylinderHeight, numSegments);
    float groundRadius = cylinderRadius + 1.0f; // Radius of the circle around the cylinder
    int numGroundSegments = 1000; // Increase for smoother circle
    std::vector<float> groundVertices = generateGroundVertices(0.0f, 0.0f, groundRadius, numGroundSegments);
    std::vector<float> roofVertices = generateHemisphereVertices(0.0f, cylinderHeight / 2.0f, 0.0f, cylinderRadius, numSegments);
    //std::vector<float> platform = generatePostCircleVertices(0.0f, 0.0f, groundRadius, numGroundSegments);

    // Create VAO for the cylinder
    VAO VAO1;
    VAO1.Bind();
    VBO VBO1(cylinderVertices.data(), cylinderVertices.size() * sizeof(float));
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO1.Unbind();
    VBO1.Unbind();

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
    std::string texPath = "/Resources/YoutubeOpenGL 7 - Going 3D/";

    // Texture
    Texture brickTex((parentDir + texPath + "img.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    brickTex.texUnit(shaderProgram, "tex0", 0);

    Texture stair((parentDir + texPath + "brick.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    stair.texUnit(shaderProgram, "tex1", 0);


    // Create VAO for fence posts
    VAO VAO2;
    VAO2.Bind();
    std::vector<float> fencePostVertices;
    float fencePostYOffset = -9.0f; // Move fence posts down by 1 unit on the y-axis
    glm::vec3 cylinderCenter(0.0f, 0.0f, 0.0f); // Center of the cylinder
    float circleRadius = cylinderRadius + 5.0f; // Radius of the circle around the cylinder
    int numPosts = 18; // Number of fence posts
    for (int i = 0; i < numPosts; ++i) {
        float angle = 2.0f * M_PI * i / numPosts; // Angle for placing posts evenly around the cylinder
        float xPosition = cylinderCenter.x + circleRadius * cos(angle);
        float zPosition = cylinderCenter.z + circleRadius * sin(angle);
        std::vector<float> postVertices = generateFencePostVertices(xPosition, fencePostYOffset, zPosition);
        fencePostVertices.insert(fencePostVertices.end(), postVertices.begin(), postVertices.end());
    }
    VBO VBO2(fencePostVertices.data(), fencePostVertices.size() * sizeof(float));
    VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO2.Unbind();
    VBO2.Unbind();


    // Create VAO for the ground
    VAO VAO_Ground;
    VAO_Ground.Bind();
    // Generate VBO for ground and link vertices
    VBO VBO_Ground(groundVertices.data(), groundVertices.size() * sizeof(float));
    // Link VBO attributes to VAO
    VAO_Ground.LinkAttrib(VBO_Ground, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // Unbind VAO and VBO after setup
    VAO_Ground.Unbind();
    VBO_Ground.Unbind();


    VAO VAO_Roof;
    VAO_Roof.Bind();
    // Generate VBO for the roof and link vertices
    VBO VBO_Roof(roofVertices.data(), roofVertices.size() * sizeof(float));
    // Link VBO attributes to VAO
    VAO_Roof.LinkAttrib(VBO_Roof, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // Unbind VAO and VBO after setup
    VAO_Roof.Unbind();
    VBO_Roof.Unbind();

    // Generate vertices for fence posts on the ground circle
    std::vector<float> postCircleVertices = generatePostCircleVertices(0.0f, 0.0f, groundRadius, 18.0f, numPostCircleSegments);

    // Create VAO for fence posts on the ground circle
    VAO VAO_PostCircle;
    VAO_PostCircle.Bind();
    // Generate VBO for fence posts and link vertices
    VBO VBO_PostCircle(postCircleVertices.data(), postCircleVertices.size() * sizeof(float));
    // Link VBO attributes to VAO
    VAO_PostCircle.LinkAttrib(VBO_PostCircle, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // Unbind VAO and VBO after setup
    VAO_PostCircle.Unbind();
    VBO_PostCircle.Unbind();

    VAO VAO3;
    VAO3.Bind();
    VBO VBO3(staircaseVertices.data(), staircaseVertices.size() * sizeof(float));
    VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO3, 2, 2, GL_FLOAT, 3 * sizeof(float), (void*)0);
    VAO3.Unbind();
    VBO3.Unbind();


    // Enable the Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Create a camera object
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        // Specify the color of the background
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Clear the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use the shader program
        shaderProgram.Activate();

        // Handle camera inputs
        camera.Inputs(window);
        // Update and export the camera matrix to the Vertex Shader
        camera.Matrix(30.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
        glUniform1f(uniID, 10.5f);
        brickTex.Bind();

        // Draw the cylinder
        VAO1.Bind();
        glDrawArrays(GL_TRIANGLES, 0, cylinderVertices.size() / 3);
        VAO1.Unbind();

        // Draw the fence posts
        VAO2.Bind();
       // glDrawArrays(GL_TRIANGLES, 0, fencePostVertices.size() / 3);
        VAO2.Unbind();

        VAO_Ground.Bind();
        //glDrawArrays(GL_TRIANGLE_FAN, 0, groundVertices.size() / 3);
        VAO_Ground.Unbind();

        VAO_Roof.Bind();
        //glDrawArrays(GL_TRIANGLE_FAN, 0, roofVertices.size() / 3);
        VAO_Roof.Unbind();

        VAO_PostCircle.Bind();
        //glDrawArrays(GL_TRIANGLES, 0, postCircleVertices.size() / 3);
        VAO_PostCircle.Unbind();

        stair.Bind();

        VAO3.Bind();
        glDrawArrays(GL_TRIANGLES, 0, staircaseVertices.size() / 3);
        VAO3.Unbind();

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Handle all GLFW events
        glfwPollEvents();
    }

    // Delete all the objects we've created
    VAO1.Delete();
    VBO1.Delete();
    VAO2.Delete();
    VBO2.Delete();
    VAO_Ground.Delete();
    VBO_Ground.Delete();
    VAO_Roof.Delete();
    VBO_Roof.Delete();
    VAO_PostCircle.Delete();
    VBO_PostCircle.Delete();
    VAO3.Delete();
    VBO3.Delete();
    shaderProgram.Delete();
    // Delete the window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}
