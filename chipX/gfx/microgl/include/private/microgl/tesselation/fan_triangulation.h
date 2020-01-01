#pragma once

#include <microgl/vec2.h>
#include <microgl/triangles.h>
#include <microgl/dynamic_array.h>

namespace tessellation {

#define abs(a) ((a)<0 ? -(a) : (a))
    using namespace microgl;

    template <typename number>
    class fan_triangulation {
        using index = unsigned int;
        using vertex = vec2<number>;
    public:

        static
        void compute(vertex * points,
                     index size,
                     dynamic_array<index> & indices_buffer_triangulation,
                     dynamic_array<triangles::boundary_info> * boundary_buffer,
                     const triangles::TrianglesIndices &requested =
                                triangles::TrianglesIndices::TRIANGLES_FAN
                        );

    };


}

#include "fan_triangulation.cpp"