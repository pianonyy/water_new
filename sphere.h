#pragma once
#ifndef OBJECTS_SPHERE_H
#define OBJECTS_PLANE_H

#include <GL/glew.h>

namespace object3d {

    class sphere {
    private:
        const float points_buffer[18] =
        {
            -1, 1, 0,
            1, 1, 0,
            -1, -1, 0,

            1, 1, 0,
            1, -1, 0,
            -1, -1, 0
        };

        const float normals_buffer[18] =
        {
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,

            0, 0, 1,
            0, 0, 1,
            0, 0, 1
        };

    public:
        GLuint vao;

        sphere(double r, int nx, int ny);
    };

};

#endif
