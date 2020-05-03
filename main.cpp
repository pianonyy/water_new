#define STB_IMAGE_IMPLEMENTATION

#define X .525731112119133606 
#define Z .850650808352039932

#include<iostream>
#include <stdlib.h>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "stb_image.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include "util.h"
#include "maths.h"
#include "rectangle.h"
#include "water_surface.h"
#include "plane.h"
#include "shader.h"
#include "sphere.h"


GLFWwindow* window;
void input() {
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }


}


void drawCube()
{
    GLfloat vertices[] =
    {
        -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
        1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
        -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
        -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
        -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
    };

    GLfloat colors[] =
    {
        0, 0, 0,   0, 0, 1,   0, 1, 1,   0, 1, 0,
        1, 0, 0,   1, 0, 1,   1, 1, 1,   1, 1, 0,
        0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,
        0, 1, 0,   0, 1, 1,   1, 1, 1,   1, 1, 0,
        0, 0, 0,   0, 1, 0,   1, 1, 0,   1, 0, 0,
        0, 0, 1,   0, 1, 1,   1, 1, 1,   1, 0, 1
    };

    static float alpha = 0;
    //attempt to rotate cube
    glRotatef(alpha, 0, 1, 0);

    /* We have a color array and a vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    /* Send data : 24 vertices */
    glDrawArrays(GL_QUADS, 0, 24);

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    alpha += 1;
}

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    window = glfwCreateWindow(1000, 1000, "water_simulation", NULL, NULL);
    if (window == NULL) { 
        std::cout << "Failed to open window!" << std::endl;
        glfwTerminate(); 
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    vec3 cam_pos(7, 7, 4);
    vec3 cam_targ(0, 0, -2);
    vec3 cam_up(0, 0, 10);

 
    

    GLuint VertexBuffer, VertexArray, ElementBuffer, ShaderProgram;

    GLfloat vertices[] = {
            -0.5, 0.5, 0.5, 1.0, 0.0, 0.0,	// Front Top Left		- Red	- 0
            0.5,  0.5, 0.5, 0.0, 1.0, 0.0,	// Front Top Right		- Green	- 1
            0.5, -0.5, 0.5, 0.0, 0.0, 1.0,	// Front Bottom Right		- Blue	- 2
            -0.5,-0.5, 0.5, 0.0, 1.0, 1.0,	// Front Bottom Left		- Cyan	- 3
            -0.5, 0.5,-0.5, 1.0, 0.0, 1.0,	// Back Top Left		- Pink	- 4
            0.5,  0.5,-0.5, 1.0, 1.0, 0.0,	// Back Top Right		- Yellow- 5
            0.5, -0.5,-0.5, 0.1, 0.1, 0.1,	// Back Bottom Right		- White - 6
            -0.5,-0.5,-0.5, 1.0, 1.0, 1.0,	// Back Bottom Left		- Gray  - 7
    };

    GLuint elements[]{
            0,3,2,  //Front
            2,1,0,
            1,5,6,	//Right
            6,2,1,
            5,4,7,	//Left
            7,6,5,
            4,7,3,	//Back
            3,0,4,
            4,5,1,	//Top
            1,0,4,
            3,2,6,	//Bottom
            6,7,3,
    };


    /*
     * Set up the view and projection matrices
     */
    mat4 view_mat = mat4::look_at(cam_pos, cam_targ, cam_up);
    mat4 inv_view_mat = view_mat.inverse();

    mat4 proj_mat = mat4::perspective_projection(60, 1, 0.1, 100);
    mat4 inv_proj_mat = proj_mat.inverse();

    /*
     * Set up the shaders
     */
    graphics::shader surface_shader("surface.vert", "surface.frag");
    graphics::shader floor_shader("floor.vert", "floor.frag");
    graphics::shader rectangle_shader("rectangle.vert", "rectangle.frag");
    graphics::shader cube_shader("cube.vert", "cube.frag");
  

    /*
     * Set up all the objects we draw
     */
    object3d::rectangle rectangle(vec3(0, 0, 0), vec3(0, 0, 1));
    object3d::water_surface water_surface;
    object3d::plane plane;
    object3d::sphere sphere(2.0,10,10);

    /*
     * Set up the textures
     */
    GLuint floor_texture = util_create_texture("floor2.jpg");
    GLuint cube_texture = util_create_texture("red.jpg");
   
    /*
     * Initialize the shaders
     */
    glUseProgram(surface_shader.program);
    GLuint floor_texture_location = glGetUniformLocation(surface_shader.program, "floor_texture");
    glUniform1i(floor_texture_location, 0);

    
    glUseProgram(0);

    float previous_time = glfwGetTime();

    bool is_drawing_continous = true;
    bool is_Q_key_down = false;

  
   

  

    
    while (!glfwWindowShouldClose(window)) {
      
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        
        /*
         * Check if Q key was pressed and invert whether we are drawing continuously
         */
        int Q_key_state = glfwGetKey(window, GLFW_KEY_Q);
        if (Q_key_state == GLFW_PRESS && !is_Q_key_down) {
            is_Q_key_down = true;
            is_drawing_continous = !is_drawing_continous;
        }
        if (Q_key_state == GLFW_RELEASE) {
            is_Q_key_down = false;
        }
        input();

        /*
         * Figure out how long the last frame was.
         */
        float current_time = glfwGetTime();
        float elapsed_time = current_time - previous_time;
        
        previous_time = current_time;

        /*
         * Update the viewport (Really should not happen every frame, only if the width/height changes)
         */
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        mat4 proj_mat = mat4::perspective_projection(60, 1, 0.1, 100);
        mat4 inv_proj_mat = proj_mat.inverse();
        proj_mat = mat4::perspective_projection(60, width / (float)height, 0.1, 100);
        inv_proj_mat = proj_mat.inverse();

        /*
         * Handle mouse clicks
         */
        {
            static bool is_mouse_down = false;

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            vec3 mouse_world = inv_view_mat * inv_proj_mat * vec3((2.0 * xpos) / width - 1.0, 1.0 - (2.0 * ypos) / height, 1.0);
            mouse_world.make_unit_length();
            vec3 mouse_intersection = cam_pos + (-cam_pos.z / mouse_world.z) * mouse_world;

            int mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
            if (mouse_state == GLFW_PRESS && !is_mouse_down) {
                is_mouse_down = true;

                if (mouse_intersection.x > -4.0 &&
                    mouse_intersection.x < 4.0 &&
                    mouse_intersection.y > -4.0 &&
                    mouse_intersection.y < 4.0) {
                    int i = (mouse_intersection.x + 4.0) / 8.0 * water_surface.width;
                    int j = (mouse_intersection.y + 4.0) / 8.0 * water_surface.height;

                    if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                        water_surface.u[i][j] = 1.2;
                        water_surface.u[i - 1][j - 1] = 0.7;
                        water_surface.u[i - 1][j] = 0.7;
                        water_surface.u[i - 1][j + 1] = 0.7;
                        water_surface.u[i + 1][j - 1] = 0.7;
                        water_surface.u[i + 1][j] = 0.7;
                        water_surface.u[i + 1][j + 1] = 0.7;
                        water_surface.u[i][j + 1] = 0.7;
                        water_surface.u[i][j - 1] = 0.5;
                    }
                }
            }
            else if (mouse_state == GLFW_RELEASE && is_mouse_down) {
                is_mouse_down = false;
            }
        }
        

        
        /*
          Draw sphere inside the water
        */
        /*
        {   
            
            GLfloat x, y, z, alpha, beta;
            GLfloat radius = 60.0f;
            int gradation = 20;
            for (alpha = 0.0; alpha < M_PI; alpha += M_PI / gradation)
            {
                
                glDrawArrays(GL_TRIANGLES, 0, 18);
                glColor3f(0.4, 0.5, 0.3);
                for (beta = 0.0; beta < 2.01 * M_PI; beta += M_PI / gradation)
                {
                    x = radius * cos(beta) * sin(alpha);
                    y = radius * sin(beta) * sin(alpha);
                    z = radius * cos(alpha);
                   
                    glVertex3f(x, y, z);
                    x = radius * cos(beta) * sin(alpha + M_PI / gradation);
                    y = radius * sin(beta) * sin(alpha + M_PI / gradation);
                    z = radius * cos(alpha + M_PI / gradation);
                    glVertex3f(x, y, z);
                }
               
            }

            

        }
        */


        
        {


            glMatrixMode(GL_PROJECTION_MATRIX);
            glLoadIdentity();


            glMatrixMode(GL_MODELVIEW_MATRIX);
            glTranslatef(0, 0, -5);
            drawCube();
        }
        
        

        /*
         * Draw the box which contains the water
         */
         
        {
            glUseProgram(floor_shader.program);

            // Set up uniforms for floor of water
            mat4 model_mat = mat4::translation(vec3(0, 0, -3)) * mat4::scale(vec3(3, 3, 1)) * mat4::rotation_z(M_PI / 2.0);
            glUniformMatrix4fv(floor_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
            glUniformMatrix4fv(floor_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m); //
            glUniformMatrix4fv(floor_shader.model_mat_location, 1, GL_TRUE, model_mat.m); // down
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floor_texture);

            // Draw floor of water
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            // Set up uniforms for side 1 of water
            model_mat = mat4::translation(vec3(0, -3, -1)) * mat4::scale(vec3(3, 1, 2)) * mat4::rotation_x(M_PI / 2.0);
            glUniformMatrix4fv(floor_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floor_texture);

            // Draw side 1 of water left
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            // Set up uniforms for side 2 of water
            model_mat = mat4::translation(vec3(-3, 0, -1)) * mat4::scale(vec3(1, 3, 2)) * mat4::rotation_y(M_PI / 2.0);
            glUniformMatrix4fv(floor_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floor_texture);

            // Draw side 2 of water right
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            glUseProgram(0);

          
        }

        {


            
       


            glUseProgram(cube_shader.program);

            mat4 model_mat = mat4::translation(vec3(-0.9, -0.9, -1.0)) * mat4::scale(vec3(2.1, 2.1, 0.1)) * mat4::rotation_z(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
            glUniformMatrix4fv(cube_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m); //
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m); // down
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);

            // Draw floor of water
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 30);
            glBindVertexArray(0);

            // Set up uniforms for side 1 of water
            model_mat = mat4::translation(vec3(-0.9, -3, 0.1)) * mat4::scale(vec3(2.1, 0.1, 1.1)) * mat4::rotation_x(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);

            // Draw side 1 of water left far
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            //// Set up uniforms for side 2 of water
            model_mat = mat4::translation(vec3(-3.0, -0.9, 0.1)) * mat4::scale(vec3(0.1, 2.1, 1.1)) * mat4::rotation_y(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);

            // Draw side 2 of water right far
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            //// Set up uniforms for side 2 of water
            model_mat = mat4::translation(vec3(1.2, -0.9, 0.1)) * mat4::scale(vec3(0.1, 2.1, 1.1)) * mat4::rotation_y(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);

            // Draw side 2 of water right near
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            // Set up uniforms for side 1 of water
            model_mat = mat4::translation(vec3(-0.9, 1.2, 0.1)) * mat4::scale(vec3(2.1, 0.1, 1.1)) * mat4::rotation_x(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);


            // Draw side 1 of water left near
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            model_mat = mat4::translation(vec3(-0.9, -0.9, 1.2)) * mat4::scale(vec3(2.1, 2.1, 0.1)) * mat4::rotation_z(M_PI / 2.0);
            glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube_texture);


            // Draw side 1 of water high
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            glUseProgram(0);
        }

        water_surface.update(elapsed_time);

        /*
         * Draw a discrete representation of the water surface
         */
        if (!is_drawing_continous) {
            glUseProgram(rectangle_shader.program);

            for (int i = 0; i < water_surface.width; i++) {
                for (int j = 0; j < water_surface.height; j++) {
                    float x = 3 - 6 * (1 - (i / (float)water_surface.width));
                    float y = 3 - 6 * (1 - (j / (float)water_surface.height));

                    vec3 position(x, y, 0);
                    vec3 scale(2.0, 2.0, water_surface.u[i][j]);

                    mat4 model_mat = mat4::translation(position) * mat4::scale(scale);

                    glUniform3f(rectangle_shader.color_location, 0.001, 0.943, 0.123);
                    glUniformMatrix4fv(rectangle_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
                    glUniformMatrix4fv(rectangle_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
                    glUniformMatrix4fv(rectangle_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);

                    glBindVertexArray(rectangle.vao);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
                    glBindVertexArray(0);
                }
            }

            glUseProgram(0);
        }
        

        /*
         * Draw a continous representation of the water surface
         */
        if (is_drawing_continous) {
            glUseProgram(surface_shader.program);

            // Set up uniforms for water surface
            mat4 model_mat = mat4::identity();
            glUniformMatrix4fv(surface_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
            glUniformMatrix4fv(surface_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
            glUniformMatrix4fv(surface_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
            glUniform3f(surface_shader.color_location, 0.527, 0.843, 0.898);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floor_texture);
            glActiveTexture(GL_TEXTURE1);
           

            // Draw the water surface
            glBindVertexArray(water_surface.vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, water_surface.elements_vbo);
            glDrawElements(GL_TRIANGLES, (water_surface.N - 1) * (water_surface.N - 1) * 3 * 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUseProgram(0);
        }
        

    

        
        

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
