#pragma once

#include <microgl/triangles.h>
#include <microgl/chunker.h>

namespace tessellation {

    using index = unsigned int;

    template <typename number>
    class complex_poly_tess {
    public:
        using vertex = microgl::vec2<number>;

        enum class fill_rule {
            non_zero, even_odd
        };

        static void compute(chunker<vertex> & pieces,
                            dynamic_array<vertex> & result,
                            dynamic_array<index> & indices,
                            const fill_rule & rule = fill_rule::non_zero,
                            const microgl::triangles::TrianglesIndices &requested = microgl::triangles::TrianglesIndices::TRIANGLES,
                            dynamic_array<microgl::triangles::boundary_info> * boundary_buffer = nullptr
                            );

    private:

    };


}

#include "../../src/complex_poly_tess.cpp"