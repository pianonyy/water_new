#define STB_IMAGE_IMPLEMENTATION



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

    graphics::shader water_shader("water.vert", "water.frag");
   
    



    /*
     * Set up all the objects we draw
     */
    object3d::rectangle rectangle(vec3(0, 0, 0), vec3(0, 0, 1));
    object3d::water_surface water_surface;
    object3d::plane plane;
    

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    /*
     * Set up the textures
     */
    GLuint floor_texture = util_create_texture("floor2.jpg");
    GLuint cube_texture = util_create_texture("red.jpg");
    GLuint flag_texture = util_create_texture("flag.jpg");
    
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
    bool is_right_key_down = false;
    bool is_left_key_down = false;
    bool is_up_key_down = false;
    bool is_down_key_down = false;

    



    float dx  = 0.0000001;
    float dy  = 0.0000001;


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

        
        
        //core of boat
        glUseProgram(rectangle_shader.program);

        


        vec3 position(2.58 + dx, 1 + dy, 0.11);
        vec3 scale(7.0, 10.0, 0.1);

        mat4 model_mat = mat4::translation(position) * mat4::scale(scale) * mat4::rotation_z(M_PI / 4);

        glUniform3f(rectangle_shader.color_location, 0.001, 0.943, 0.123);
        glUniformMatrix4fv(rectangle_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        glUniformMatrix4fv(rectangle_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
        glUniformMatrix4fv(rectangle_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boat_texture);

        glBindVertexArray(rectangle.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 100);
        glBindVertexArray(0);



        glUseProgram(0);


        //flag))
        glUseProgram(cube_shader.program);
        mat4 model_mat_boat1 = mat4::translation(vec3(2.54+dx, 0.6+dy, 0.86)) * mat4::scale(vec3(0.3, 0.3, 0.1)) * mat4::rotation_y(M_PI / 2) *mat4::rotation_z(M_PI / 2);
        glUniformMatrix4fv(rectangle_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
        glUniformMatrix4fv(rectangle_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glUniformMatrix4fv(rectangle_shader.model_mat_location, 1, GL_TRUE, model_mat_boat1.m);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flag_texture);

        
        glBindVertexArray(plane.vao);
        glDrawArrays(GL_TRIANGLES, 0, 100);
        glBindVertexArray(0);




        glUseProgram(0);
                
        //parus
        glUseProgram(rectangle_shader.program);




        vec3 position_2(2.58 + dx, 1 + dy, 0.11);
        vec3 scale_2(0.6, 0.6, 0.8);

        mat4 model_mat_boat2 = mat4::translation(position_2) * mat4::scale(scale_2);

        glUniform3f(rectangle_shader.color_location, 0.001, 0.943, 0.123);
        glUniformMatrix4fv(rectangle_shader.model_mat_location, 1, GL_TRUE, model_mat_boat2.m);
        glUniformMatrix4fv(rectangle_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
        glUniformMatrix4fv(rectangle_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);

        glBindVertexArray(rectangle.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 100);
        glBindVertexArray(0);
        
        glUseProgram(0);
        


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
            static bool is_mouse_down = true;

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            vec3 mouse_world = inv_view_mat * inv_proj_mat * vec3((2.0 * xpos) / width - 1.0, 1.0 - (2.0 * ypos) / height, 1.0);
            mouse_world.make_unit_length();
            vec3 mouse_intersection = cam_pos + (-cam_pos.z / mouse_world.z) * mouse_world;

            int mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
            if (mouse_state == GLFW_PRESS && !is_mouse_down) {
                is_mouse_down = true;

                if (mouse_intersection.x > -3.0 &&
                    mouse_intersection.x < 3.0 &&
                    mouse_intersection.y > -3.0 &&
                    mouse_intersection.y < 3.0) {
                    int i = (mouse_intersection.x + 3.0) / 6.0 * water_surface.width;
                    int j = (mouse_intersection.y + 3.0) / 6.0 * water_surface.height;
                    
                        
                    
                    if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                        water_surface.u[i][j] = 0.6;// 1.2;
                        water_surface.u[i - 1][j - 1] = 0.3; //0.7
                        water_surface.u[i - 1][j] = 0.3; //0.7
                        water_surface.u[i - 1][j + 1] = 0.3; //0.7
                        water_surface.u[i + 1][j - 1] = 0.3; //0.7
                        water_surface.u[i + 1][j] = 0.3; // 0.7
                        water_surface.u[i + 1][j + 1] = 0.3; // 0.7
                        water_surface.u[i][j + 1] = 0.3; // 0.7
                        water_surface.u[i][j - 1] = 0.3; // 0.5
                    }
                }
            }
            else if (mouse_state == GLFW_RELEASE && is_mouse_down) {
                is_mouse_down = false;
            }
        }

       /* {
                glUseProgram(rectangle_shader.program);

                

                        vec3 position(2.58, 1, -1);
                        vec3 scale(7.0, 10.0, 0.1);

                        mat4 model_mat = mat4::translation(position) * mat4::scale(scale);

                        glUniform3f(rectangle_shader.color_location, 0.001, 0.943, 0.123);
                        glUniformMatrix4fv(rectangle_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
                        glUniformMatrix4fv(rectangle_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
                        glUniformMatrix4fv(rectangle_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);

                        glBindVertexArray(rectangle.vao);
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 100);
                        glBindVertexArray(0);
                    
                

                glUseProgram(0);
        }*/

        

        
        
        /*
          Draw sphere inside the water
        */
        
        //{   glUseProgram(sphere_shader.program);
        //   
        //    glUniformMatrix4fv(floor_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
        //    glUniformMatrix4fv(floor_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m); //
        //    glUniformMatrix4fv(floor_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    GLfloat x, y, z, alpha, beta;
        //    GLfloat radius = 60.0f;
        //    int gradation = 20;
        //    for (alpha = 0.0; alpha < M_PI; alpha += M_PI / gradation)
        //    {
        //        
        //        
        //        
        //        for (beta = 0.0; beta < 2.01 * M_PI; beta += M_PI / gradation)
        //        {
        //            x = radius * cos(beta) * sin(alpha);
        //            y = radius * sin(beta) * sin(alpha);
        //            z = radius * cos(alpha);
        //           
        //            glBindVertexArray(x,y,z)
        //            glVertex3f(x, y, z);
        //            x = radius * cos(beta) * sin(alpha + M_PI / gradation);
        //            y = radius * sin(beta) * sin(alpha + M_PI / gradation);
        //            z = radius * cos(alpha + M_PI / gradation);
        //            glVertex3f(x, y, z);
        //            glDrawArrays(GL_TRIANGLES, 0, 18);
        //        }
        //       
        //    }
        //    glUseProgram(0);

        //    

        //}
        


        //

        //
        //

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

        //{


        //    
       


        //    glUseProgram(cube_shader.program);

        //    mat4 model_mat = mat4::translation(vec3(-0.9, -0.9, -1.0)) * mat4::scale(vec3(2.1, 2.1, 0.1)) * mat4::rotation_z(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
        //    glUniformMatrix4fv(cube_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m); //
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m); // down
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);

        //    // Draw floor of water
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 30);
        //    glBindVertexArray(0);

        //    // Set up uniforms for side 1 of water
        //    model_mat = mat4::translation(vec3(-0.9, -3, 0.1)) * mat4::scale(vec3(2.1, 0.1, 1.1)) * mat4::rotation_x(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);

        //    // Draw side 1 of water left far
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //    glBindVertexArray(0);

        //    //// Set up uniforms for side 2 of water
        //    model_mat = mat4::translation(vec3(-3.0, -0.9, 0.1)) * mat4::scale(vec3(0.1, 2.1, 1.1)) * mat4::rotation_y(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);

        //    // Draw side 2 of water right far
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //    glBindVertexArray(0);

        //    //// Set up uniforms for side 2 of water
        //    model_mat = mat4::translation(vec3(1.2, -0.9, 0.1)) * mat4::scale(vec3(0.1, 2.1, 1.1)) * mat4::rotation_y(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);

        //    // Draw side 2 of water right near
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //    glBindVertexArray(0);

        //    // Set up uniforms for side 1 of water
        //    model_mat = mat4::translation(vec3(-0.9, 1.2, 0.1)) * mat4::scale(vec3(2.1, 0.1, 1.1)) * mat4::rotation_x(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);


        //    // Draw side 1 of water left near
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //    glBindVertexArray(0);

        //    model_mat = mat4::translation(vec3(-0.9, -0.9, 1.2)) * mat4::scale(vec3(2.1, 2.1, 0.1)) * mat4::rotation_z(M_PI / 2.0);
        //    glUniformMatrix4fv(cube_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, cube_texture);


        //    // Draw side 1 of water high
        //    glBindVertexArray(plane.vao);
        //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //    glBindVertexArray(0);

        //    glUseProgram(0);
        //}

        {
            glUseProgram(water_shader.program);

          

            // Set up uniforms for side 1 of water
            mat4 model_mat1 = mat4::translation(vec3(0, 3, -1.5)) * mat4::scale(vec3(3, 1, 1.5)) * mat4::rotation_x(M_PI / 2.0);
            
            glUniformMatrix4fv(water_shader.view_mat_location, 1, GL_TRUE, view_mat.m);
            glUniformMatrix4fv(water_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m); //
            glUniformMatrix4fv(water_shader.model_mat_location, 1, GL_TRUE, model_mat1.m);

            // Draw side 1 of water left
            glBindVertexArray(plane.vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glBindVertexArray(0);

            // Set up uniforms for side 2 of water
            model_mat1 = mat4::translation(vec3(3, 0, -1.5)) * mat4::scale(vec3(1, 3, 1.5)) * mat4::rotation_y(M_PI / 2.0);
            glUniformMatrix4fv(water_shader.model_mat_location, 1, GL_TRUE, model_mat1.m);


            // Draw side 2 of water right
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
        

    

        //int right_key_state = glfwGetKey(window, GLFW_KEY_RIGHT);
       
        //if (right_key_state == GLFW_PRESS && !is_right_key_down) {
        //    is_right_key_down = true;

        //    int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
        //    int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



        //    if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
        //        water_surface.u[i][j] = 0.2;// 1.2;
        //        water_surface.u[i - 1][j - 1] = 0.1; //0.7
        //        water_surface.u[i - 1][j] = 0.1; //0.7
        //        water_surface.u[i - 1][j + 1] = 0.1; //0.7
        //        water_surface.u[i + 1][j - 1] = 0.1; //0.7
        //        water_surface.u[i + 1][j] = 0.1; // 0.7
        //        water_surface.u[i + 1][j + 1] = 0.1; // 0.7
        //        water_surface.u[i][j + 1] = 0.1; // 0.7
        //        water_surface.u[i][j - 1] = 0.1; // 0.5
        //    }
        //    dy += 0.09;
        //}
        //if (right_key_state == GLFW_RELEASE) {
        //    is_right_key_down = false;
        //}
        
        int state_l = glfwGetKey(window, GLFW_KEY_LEFT); 
        if (state_l == GLFW_PRESS) {
            
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                dy -= 0.004;
                water_surface.u[i][j] = 0.2;// 1.2;
                water_surface.u[i - 1][j - 1] = 0.1; //0.7
                water_surface.u[i - 1][j] = 0.1; //0.7
                water_surface.u[i - 1][j + 1] = 0.1; //0.7
                water_surface.u[i + 1][j - 1] = 0.1; //0.7
                water_surface.u[i + 1][j] = 0.1; // 0.7
                water_surface.u[i + 1][j + 1] = 0.1; // 0.7
                water_surface.u[i][j + 1] = 0.1; // 0.7
                water_surface.u[i][j - 1] = 0.1; // 0.5
            }
            else {
                dy += 0.1;
            }
        }

        int state_r = glfwGetKey(window, GLFW_KEY_RIGHT);
        if (state_r == GLFW_PRESS) {
            
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                dy += 0.004;
                water_surface.u[i][j] = 0.2;// 1.2;
                water_surface.u[i - 1][j - 1] = 0.1; //0.7
                water_surface.u[i - 1][j] = 0.1; //0.7
                water_surface.u[i - 1][j + 1] = 0.1; //0.7
                water_surface.u[i + 1][j - 1] = 0.1; //0.7
                water_surface.u[i + 1][j] = 0.1; // 0.7
                water_surface.u[i + 1][j + 1] = 0.1; // 0.7
                water_surface.u[i][j + 1] = 0.1; // 0.7
                water_surface.u[i][j - 1] = 0.1; // 0.5
            }
            else {
                dy -= 0.1;
            }
        }

        int state_u = glfwGetKey(window, GLFW_KEY_UP);
        if (state_u == GLFW_PRESS) {
            
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                dx -= 0.004;
                water_surface.u[i][j] = 0.2;// 1.2;
                water_surface.u[i - 1][j - 1] = 0.2; //0.7
                water_surface.u[i - 1][j] = 0.2; //0.7
                water_surface.u[i - 1][j + 1] = 0.2; //0.7
                water_surface.u[i + 1][j - 1] = 0.2; //0.7
                water_surface.u[i + 1][j] = 0.2; // 0.7
                water_surface.u[i + 1][j + 1] = 0.2; // 0.7
                water_surface.u[i][j + 1] = 0.2; // 0.7
                water_surface.u[i][j - 1] = 0.1; // 0.5
            }
            else {
                dx += 0.1;
            }
            
        }

        int state_d = glfwGetKey(window, GLFW_KEY_DOWN);
        if (state_d == GLFW_PRESS) {
            
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                dx += 0.004;
                water_surface.u[i][j] = 0.2;// 1.2;
                water_surface.u[i - 1][j - 1] = 0.1; //0.7
                water_surface.u[i - 1][j] = 0.1; //0.7
                water_surface.u[i - 1][j + 1] = 0.1; //0.7
                water_surface.u[i + 1][j - 1] = 0.1; //0.7
                water_surface.u[i + 1][j] = 0.1; // 0.7
                water_surface.u[i + 1][j + 1] = 0.1; // 0.7
                water_surface.u[i][j + 1] = 0.1; // 0.7
                water_surface.u[i][j - 1] = 0.1; // 0.5
            }
            else {
                dx -= 0.1;
            }
        }

        
            

        /*int up_key_state = glfwGetKey(window, GLFW_KEY_UP);
        if (up_key_state == GLFW_PRESS && !is_up_key_down) {
            is_up_key_down = true;
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                water_surface.u[i][j] = 0.2;
                water_surface.u[i - 1][j - 1] = 0.1; 
                water_surface.u[i - 1][j] = 0.1; 
                water_surface.u[i - 1][j + 1] = 0.1; 
                water_surface.u[i + 1][j - 1] = 0.1; 
                water_surface.u[i + 1][j] = 0.1; 
                water_surface.u[i + 1][j + 1] = 0.1; 
                water_surface.u[i][j + 1] = 0.1; 
                water_surface.u[i][j - 1] = 0.1; 
            }
            dx -= 0.09;
        }
        if (up_key_state == GLFW_RELEASE) {
            is_up_key_down = false;
        }*/
        
        /*int down_key_state = glfwGetKey(window, GLFW_KEY_DOWN);
        if (down_key_state == GLFW_PRESS && !is_down_key_down) {
            is_down_key_down = true;
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                water_surface.u[i][j] = 0.2;
                water_surface.u[i - 1][j - 1] = 0.1;
                water_surface.u[i - 1][j] = 0.1;
                water_surface.u[i - 1][j + 1] = 0.1;
                water_surface.u[i + 1][j - 1] = 0.1;
                water_surface.u[i + 1][j] = 0.1;
                water_surface.u[i + 1][j + 1] = 0.1;
                water_surface.u[i][j + 1] = 0.1;
                water_surface.u[i][j - 1] = 0.1;
            }
            dx += 0.09;
        }
        if (down_key_state == GLFW_RELEASE) {
            is_down_key_down = false;
        }*/

        /*int left_key_state = glfwGetKey(window, GLFW_KEY_LEFT);
        if (left_key_state == GLFW_PRESS && !is_left_key_down) {
            is_left_key_down = true;
            int i = (2.58 + dx + 3.0) / 6.0 * water_surface.width;
            int j = (1.0 + dy + 3.0) / 6.0 * water_surface.height;



            if (i > 0 && j > 0 && i < water_surface.width - 1 && j < water_surface.height - 1) {
                water_surface.u[i][j] = 0.2;
                water_surface.u[i - 1][j - 1] = 0.1;
                water_surface.u[i - 1][j] = 0.1;
                water_surface.u[i - 1][j + 1] = 0.1;
                water_surface.u[i + 1][j - 1] = 0.1;
                water_surface.u[i + 1][j] = 0.1;
                water_surface.u[i + 1][j + 1] = 0.1;
                water_surface.u[i][j + 1] = 0.1;
                water_surface.u[i][j - 1] = 0.1;
            }
            dy -= 0.09;
        }
        if (left_key_state == GLFW_RELEASE) {
            is_left_key_down = false;
        }*/
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
