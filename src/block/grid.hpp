#pragma once
#include "block.hpp"
#include <Eigen/Geometry>

namespace block {

struct grid {
    std::vector<block> blocks;
    grid();
    bool contains_point(point::point position);

    // add_point returns false if there was any overlap with existing points
    bool add_point(point::point position);
    void add_box(point::point position, point::point size);
    void add_cylinder(point::point position, int radius, int height); // from bottom center
};

}