#pragma once
#include <Eigen/Geometry>

namespace point {

struct point : public Eigen::Vector3i {
    using Eigen::Vector3i::Vector3i;
};

std::vector<point> points_in_range(point position, point size);
std::vector<point> points_on_faces(point position);

struct point_hasher {
    size_t operator()(const point& point_) const;
};

}
