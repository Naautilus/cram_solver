#pragma once
#include <vector>
#include "../face/face.hpp"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "../point/point.hpp"

/* cube faces are in order:
 *
 * 0: +x
 * 1: +y
 * 2: +z
 * 3: -x
 * 4: -y
 * 5: -z
 *
 */


namespace block {

extern std::vector<point::point> block_face_positions;

enum type {
    air,
    mantlet,
    connector_2,
    connector_3,
    connector_4,
    connector_5,
    connector_6,
    gauge_2,
    gauge_3,
    gauge_4,
    gauge_6,
    packer,
    compactor,
    pellet,
};

struct block {
    point::point position{0, 0, 0};
    std::vector<face::face> faces;
    type type_;
    block(point::point position);
    block(point::point position, type type_, std::vector<face::face> faces_);
    block(point::point position, type type_, face::face face_); // fills all 6 faces with provided face
    face::face face_at_point(point::point);
    std::vector<block> get_all_rotations();
    bool propogates_cram_connection(std::shared_ptr<block> other);
    std::string to_string();
};

}