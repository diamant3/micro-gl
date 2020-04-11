#pragma once
#define DEBUG_PLANAR false
#define MAX_ITERATIONS 200
#define APPLY_MERGE true

#include <microgl/tesselation/half_edge.h>
#include <microgl/chunker.h>
#if DEBUG_PLANAR==true
#include <stdexcept>
#endif

namespace microgl {

    namespace tessellation {
#define abs(a) ((a)<0 ? -(a) : (a))

        enum class fill_rule {
            non_zero, even_odd
        };

        template <typename number>
        class planarize_division {
        private:
#define min__(a, b) ((a)<(b) ? (a) : (b))
#define max__(a, b) ((a)>(b) ? (a) : (b))
            using vertex = microgl::vec2<number>;
            using index = unsigned int;
            using half_edge = half_edge_t<number>;
            using half_edge_vertex = half_edge_vertex_t<number>;
            using half_edge_face = half_edge_face_t<number>;
            using conflict = conflict_node_t<number>;

            struct static_pool {
            private:
                half_edge_vertex * _vertices = nullptr;
                half_edge * _edges = nullptr;
                half_edge_face * _faces = nullptr;
                conflict * _conflicts = nullptr;
                index _curr_v = 0;
                index _curr_e = 0;
                index _curr_c = 0;

            public:
                half_edge_vertex * get_vertex() {
                    return &_vertices[_curr_v++];
                }
                half_edge * get_edge() {
                    return &_edges[_curr_e++];
                }
                conflict * get_conflict_node() {
                    return &_conflicts[_curr_c++];
                }

                static_pool(const int v, const int e, const int c) {
                    _vertices = new half_edge_vertex[v];
                    _edges = new half_edge[e];
                    _conflicts = new conflict[c];
                }

                ~static_pool() {
                    delete [] _vertices;
                    delete [] _edges;
                    delete [] _conflicts;

                    _vertices = nullptr;
                    _edges = nullptr;
                    _conflicts = nullptr;
                }
            };

            struct dynamic_pool {
            private:
                index t = 1;
                dynamic_array<half_edge_vertex *> _vertices;
                dynamic_array<half_edge *> _edges;
                dynamic_array<half_edge_face *> _faces;

            public:
                ~dynamic_pool() {
                    const auto size_v = _vertices.size();
                    const auto size_e = _edges.size();
                    const auto size_f = _faces.size();
                    for (index ix = 0; ix < size_v; ++ix)
                        delete _vertices.data()[ix];
                    for (index ix = 0; ix < size_e; ++ix)
                        delete _edges.data()[ix];
                    for (index ix = 0; ix < size_f; ++ix)
                        delete _faces.data()[ix];
                };

                auto create_vertex(const vertex &coords) -> half_edge_vertex * {
                    auto * v = new half_edge_vertex();
                    v->coords = coords;
                    _vertices.push_back(v);
                    return v;
                }

                auto create_edge() -> half_edge * {
                    auto * v = new half_edge();
                    _edges.push_back(v);
                    return v;
                }

                auto create_face() -> half_edge_face * {
                    auto * v = new half_edge_face();
                    v->index=t++;
                    _faces.push_back(v);
                    return v;
                }

                auto getFaces() -> dynamic_array<half_edge_face *> & {
                    return _faces;
                }

            };

            struct trapeze_t {
                // in ccw order
                half_edge *left_top=nullptr;
                half_edge *left_bottom=nullptr;
                half_edge *right_bottom=nullptr;
                half_edge *right_top=nullptr;
            };

            enum class point_class_with_trapeze {
                // all of the walls enums are actually strictly inside excluding endpoints, open segments
                strictly_inside, left_wall, right_wall, top_wall, bottom_wall, boundary_vertex, outside, unknown
            };

            enum class intersection_status {
                intersect, none, parallel, degenerate_line
            };

            struct conflicting_edge_intersection_status {
                vertex point_of_interest;
                point_class_with_trapeze class_of_interest_point;
            };

            struct vertical_face_cut_result {
                // true/false if was split into two
                bool face_was_split = false;
                // if a vertical split occurs, vertex_a_edge_split_edge is oriented from bottom->top
                // and has left_trapeze to it's left
                half_edge * vertex_a_edge_split_edge = nullptr;
                trapeze_t left_trapeze;
                trapeze_t right_trapeze;
            };

            struct location_codes {
                int left_wall, bottom_wall, right_wall, top_wall;
            };

        public:
            static
            void compute(const chunker<vertex> & pieces);

            static
            void compute_DEBUG(const chunker<vertex> &pieces,
                               const fill_rule &rule,
                               dynamic_array<vertex> &output_vertices,
                               dynamic_array<index> &output_indices,
                               dynamic_array<microgl::triangles::boundary_info> *boundary_buffer,
                               triangles::indices & output_indices_type,
                               dynamic_array<vertex> &debug_trapezes);

        private:
            static
            void face_to_trapeze_vertices(half_edge_face * face, dynamic_array<vertex> &vertices) {
                if(face->edge== nullptr)
                    return;
                auto trapeze = infer_trapeze(face);
                vertices.push_back(trapeze.left_top->origin->coords);
                vertices.push_back(trapeze.right_top->origin->coords);
                vertices.push_back(trapeze.right_bottom->origin->coords);
                vertices.push_back(trapeze.left_bottom->origin->coords);
            }

            static
            auto create_frame(const chunker<vertex> &pieces, static_pool & static_pool, dynamic_pool & dynamic_pool) -> half_edge_face *;

            static
            auto build_edges_and_conflicts(const chunker<vertex> &pieces, half_edge_face & main_frame,
                    static_pool & pool) -> half_edge** ;

            static
            void insert_edge(half_edge *edge, index idx, dynamic_pool &pool);

            static
            int classify_point(const vertex &point, const vertex &a, const vertex &b);

            static
            intersection_status finite_segment_intersection_test(const vertex &a, const vertex &b,
                                                                 const vertex &c, const vertex &d,
                                                                 vertex &intersection,
                                                                 number &alpha, number &alpha1);

            static
            auto infer_trapeze(const half_edge_face *face) -> trapeze_t;

            static
            auto handle_vertical_face_cut(const trapeze_t &trapeze,
                                          vertex & a,
                                          const point_class_with_trapeze &a_classs,
                                          dynamic_pool &pool) -> vertical_face_cut_result;

            static
            auto try_split_edge_at(const vertex& point, half_edge *edge, dynamic_pool &pool) -> half_edge * ;

            static half_edge *
            try_insert_vertex_on_trapeze_boundary_at(const vertex &v, const trapeze_t &trapeze,
                                                     point_class_with_trapeze where_boundary, dynamic_pool &pool);

            static
            number evaluate_line_at_x(number x, const vertex &a, const vertex &b);

            static
            auto
            insert_edge_between_non_co_linear_vertices(half_edge *vertex_a_edge, half_edge *vertex_b_edge,
                    dynamic_pool &dynamic_pool) -> half_edge *;

            static
            auto classify_conflict_against_two_faces(const half_edge *face_separator, const half_edge *edge)->half_edge_face *;

            static
            void re_distribute_conflicts_of_split_face(conflict *conflict_list, const half_edge *face_separator);

            static
            void walk_and_update_edges_face(half_edge *edge_start, half_edge_face *face);

            static
            void clamp(number &val, const number &a, const number &b);

            static
            void clamp_vertex(vertex &v, vertex a, vertex b);
            static
            void clamp_vertex_horizontally(vertex &v, vertex a, vertex b);
            static
            void clamp_vertex_vertically(vertex &v, vertex a, vertex b);

            static
            point_class_with_trapeze classify_arbitrary_point_with_trapeze(const vertex &point, const trapeze_t &trapeze);
            static
            point_class_with_trapeze classify_from_location_codes(const location_codes &codes);

            static
            location_codes compute_location_codes(const vertex &point, const trapeze_t &trapeze);

            static
            auto
            compute_conflicting_edge_intersection_against_trapeze(const trapeze_t &trapeze,
                                                                  vertex &a, vertex b, const point_class_with_trapeze & a_class)
                                                                  -> conflicting_edge_intersection_status;

            static
            bool is_trapeze_degenerate(const trapeze_t &trapeze);

            static
            bool do_a_b_lies_on_same_trapeze_wall(const trapeze_t &trapeze, const vertex &a, const vertex &b,
                                                  const point_class_with_trapeze &a_class,
                                                  const point_class_with_trapeze &b_class,
                                                  point_class_with_trapeze &resulting_wall);

            static
            int infer_edge_winding(const vertex &a, const vertex &b);

            static
            bool is_a_before_or_equal_b_on_same_boundary(const vertex &a, const vertex &b,
                                                         const point_class_with_trapeze &wall);

            static
            void handle_co_linear_edge_with_trapeze(const trapeze_t &trapeze, const vertex &a, const vertex &b,
                                                    const point_class_with_trapeze &wall_class,
                                                    half_edge **result_edge_a,
                                                    half_edge **result_edge_b, dynamic_pool &pool);

            static
            point_class_with_trapeze
            locate_and_classify_point_that_is_already_on_trapeze(const vertex &point, const trapeze_t &trapeze);

            static bool
            handle_face_split(const trapeze_t &trapeze, vertex &a, vertex &b,
                              const point_class_with_trapeze &a_class,
                              const point_class_with_trapeze &b_class,
                              half_edge ** result_edge_a,
                              half_edge ** result_edge_b,
                              dynamic_pool &dynamic_pool);

            static
            half_edge *locate_half_edge_of_face_rooted_at_vertex(const half_edge_vertex *root, const half_edge_face *face);

            static
            auto locate_face_of_a_b(const half_edge_vertex &a, const vertex &b) -> half_edge *;

            static
            bool is_distance_to_line_less_than_epsilon(const vertex &v, const vertex &a, const vertex &b, number epsilon);

            static
            void remove_edge(half_edge *edge);

            static
            void handle_face_merge(const half_edge_vertex *vertex_on_vertical_wall);

            static
            point_class_with_trapeze
            round_vertex_to_trapeze(vertex &point, const trapeze_t &trapeze);

            static
            point_class_with_trapeze
            round_edge_to_trapeze(const vertex &a, vertex &b,
                                  const point_class_with_trapeze &class_a, const trapeze_t &trapeze);

            static
            int compute_face_windings(half_edge_face *face);

            static
            bool infer_fill(int winding, const fill_rule &rule);
        };

    }

}

#include "planarize_division.cpp"