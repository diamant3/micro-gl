#include <microgl/tesselation/simplifier.h>

namespace tessellation {

//    struct node {
//        // may have missing null children, that were removed
//        std::vector<node *> children;
//        int index_poly = -1;
//    };
//
//    // Use the sign of the determinant of vectors (AB,AM), where M(X,Y) is the query point:
//    // position = sign((Bx - Ax) * (Y - Ay) - (By - Ay) * (X - Ax))
//
//    //    Input:  three points p, a, b
//    //    Return: >0 for P2 left of the line through a and b
//    //            =0 for P2  on the line
//    //            <0 for P2  right of the line
//    //    See: Algorithm 1 "Area of Triangles and Polygons"
//    inline int
//    classify_point(const Vertex & point, const Vertex &a, const Vertex & b)
//    {
//        return int((b.x - a.x) * (point.y - a.y)
//                   - (point.x - a.x) * (b.y - a.y) );
//    }
//
//    inline int
//    isLeft(const Vertex & point, const Vertex &a, const Vertex & b)
//    {
//        return classify_point(point, a, b) > 0;
//    }
//
//    bool point_in_simple_poly(const Vertex &point,
//                              const Vertex * poly,
//                              int size,
//                              bool CCW = true) {
//        bool fails = true;
//        int direction = CCW ? 1 : -1;
//
//        for (int ix = 0; ix < size; ++ix) {
//            int a = ix;
//            int b = (ix+1)==size ? 0 : ix+1;
//
//            // handle a common degenerate case
//            if(poly[a]==poly[b])
//                continue;
//
//            int classify = classify_point(point, poly[a], poly[b]);
//            fails = classify * direction > 0;
//
//            if(fails)
//                return false;
//        }
//
//        return true;
//    }
//
//    /**
//     * since polygons are non-intersecting, then it is enough to test one point inclusion.
//     * if one point is inside a polygon, then the entire is polygon inside, otherwise
//     * it will contradict the fast that they are non-intersecting.
//     * polygons may touch one another. this is a nice and fast algorithm
//     *
//     * 1=poly 1 inside poly 2
//     * -1=poly 2 inside poly 1
//     * 0=poly 1 and poly 2 have zero intersection
//     */
//    int compare_simple_non_intersecting_polygons(const Vertex * poly_1, int size_1, bool poly_1_CCW,
//                                                 const Vertex * poly_2, int size_2, bool poly_2_CCW) {
//        bool poly_1_inside_2 = point_in_simple_poly(poly_1[0], poly_2, size_2, poly_2_CCW);
//
//        if(poly_1_inside_2)
//            return 1;
//
//        bool poly_2_inside_1 = point_in_simple_poly(poly_2[0], poly_1, size_1, poly_1_CCW);
//
//        if(poly_2_inside_1)
//            return -1;
//
//        return 0;
//    }
//
//    void compute_component_tree_recurse(node * root,
//                                        node * current,
//                                        const vector<Poly> &poly_list,
//                                        const vector<int> &winding,
//                                        const vector<int> &directions) {
//        int children_count = root->children.size();
//        int compare;
//
//        auto & poly_current = poly_list[current->index_poly].vtxList;
//
//        if(root->index_poly != -1) {
//            auto & poly_root = poly_list[root->index_poly].vtxList;
//
//            compare = compare_simple_non_intersecting_polygons(
//                    poly_current.data(), poly_current.size(), directions[current->index_poly],
//                    poly_root.data(), poly_root.size(), directions[root->index_poly]);
//
//            // compare against the root polygon
//
//            switch (compare) {
//                case 1: // current inside the root node
//                    // we need to find a suitable place for it among children later
//                    break;
//                case -1: // root inside current node
//                    // attach root to current
////                compute_component_tree_recurse(current, root, poly_list, winding, directions);
//                    throw std::runtime_error("weird !!!");
//                    break;
//                case 0: // complete strangers, let's exit
//                    return;
//            }
//
//        }
//
//        // lets' go over the root's children
//        for (int ix = 0; ix < children_count; ++ix) {
//            auto * child_node = root->children[ix];
//
//            if(child_node== nullptr)
//                continue;
//
//            int child_poly_index = child_node->index_poly;
//            auto & child_poly = poly_list[child_poly_index].vtxList;
//
//            compare = compare_simple_non_intersecting_polygons(
//                    poly_current.data(), poly_current.size(), directions[current->index_poly],
//                    child_poly.data(), child_poly.size(), directions[child_poly_index]);
//
//            switch (compare) {
//                case 1: // current inside the child, bubble it down, and exit
//                    compute_component_tree_recurse(child_node, current, poly_list, winding, directions);
//                    return;
//                case -1: // child inside current node, so add it to the current node
//                    // and also, disconnect the child from the root, so it will have one parent
//                    // this is why I need a linked list
//                    current->children.push_back(child_node);
//                    root->children[ix] = nullptr;
//                    break;
//                case 0: // complete strangers
//                    break;
//            }
//
//        }
//
//        // if we made it here, it means current node is direct child of this root
//        root->children.push_back(current);
//    }
//
//    void compute_component_tree(const MultiPoly &multi,
//                                const vector<int> &winding,
//                                const vector<int> &directions) {
//        auto & poly_list = multi.m_polyList;
//
//        node nodes[multi.m_polyList.size()];
//        node root{};
//
//        for (unsigned long ix = 0; ix < poly_list.size(); ++ix)
//        {
//            auto * current = &(nodes[ix]);
//            current->index_poly = int(ix);
//
//            // now start hustling
//            compute_component_tree_recurse(
//                    &root,
//                    current,
//                    poly_list,
//                    winding,
//                    directions);
//
//        }
//
//        int a = 0;
//    }

    void simplifier::compute(chunker<vec2_f> & pieces,
                             chunker<vec2_f> & result,
                             vector<int> &winding) {

        vector<int> directions;

        simplify_components::compute(
                pieces,
                result,
                winding,
                directions);

        /*
        multi.findMonotone(result_multi, winding, directions);
//        multi.findMonotone(result_multi, winding);

        auto & poly_list = result_multi.m_polyList;
        int ref_location = 0;//polygons_result.size();

        // record out polygons
        for (auto &p : poly_list) {
            for (auto &v : p.vtxList) {
                polygons_result.push_back({v.x, v.y});
            }
            // record the polygons
            polygons_locations.push_back(ref_location);
            ref_location += p.vtxList.size();
        }
        // the last one
        polygons_locations.push_back(ref_location);
*/

        // experiment
//        compute_component_tree(result_multi, winding, directions);

        int a =0;
    }

//    void simplifier::compute(vec2_32i *$pts,
//                                            index size) {
//
//    }
}