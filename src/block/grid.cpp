#include "grid.hpp"

namespace block {

grid::grid(){};

bool grid::contains_point(point::point position) {
    for (block block_ : blocks) {
        if (block_.position == position) return true;
    }
    return false;
}

bool grid::add_point(point::point position) {
    if (contains_point(position)) return false;
    blocks.push_back(block());
    return true;
}

void grid::add_box(point::point position, point::point size) {
    for (point::point position_ : point::points_in_range(position, size)) {
        add_point(position_);
    }
}

void grid::add_cylinder(point::point position, int radius, int height) {
    for (point::point position_ : point::points_in_range(position, point::point(radius, radius, -height))) {
        point::point relative_position = position_ - position;
        relative_position.z() = 0;
        if (relative_position.norm() < radius)
            add_point(position_);
    }
}

}