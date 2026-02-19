#include "point.hpp"

namespace point {

std::vector<point> points_in_range(point position, point size) {
    std::vector<point> output;
    for (int x = 0; x < size.x(); x++) {
        for (int y = 0; y < size.y(); y++) {
            for (int z = 0; z < size.z(); z++) {
                output.push_back(position + Eigen::Vector3i(x, y, z));
            }
        }
    }
    return output;
}
std::vector<point> points_on_faces(point position) {
    return {
        position + point( 1, 0, 0),
        position + point( 0, 1, 0),
        position + point( 0, 0, 1),
        position + point(-1, 0, 0),
        position + point( 0,-1, 0),
        position + point( 0, 0,-1),
    };
}
size_t point_hasher::operator()(const point& point_) const
{
    return std::hash<int>()(point_.x()) ^ (std::hash<int>()(point_.y()) << 1) ^ (std::hash<int>()(point_.z()) << 2);
}

}
