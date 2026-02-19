#include "block.hpp"
#include <format>

namespace {

std::vector<Eigen::Matrix3i> all_cube_rotations = {
        
    (Eigen::Matrix3i() <<  1, 0, 0,   0, 1, 0,   0, 0, 1).finished(),
    (Eigen::Matrix3i() <<  1, 0, 0,   0, 0,-1,   0, 1, 0).finished(),
    (Eigen::Matrix3i() <<  1, 0, 0,   0,-1, 0,   0, 0,-1).finished(),
    (Eigen::Matrix3i() <<  1, 0, 0,   0, 0, 1,   0,-1, 0).finished(),
    
    (Eigen::Matrix3i() <<  0,-1, 0,   1, 0, 0,   0, 0, 1).finished(),
    (Eigen::Matrix3i() <<  0, 0, 1,   1, 0, 0,   0, 1, 0).finished(),
    (Eigen::Matrix3i() <<  0, 1, 0,   1, 0, 0,   0, 0,-1).finished(),
    (Eigen::Matrix3i() <<  0, 0,-1,   1, 0, 0,   0,-1, 0).finished(),
    
    (Eigen::Matrix3i() <<  0, 0,-1,   0, 1, 0,   1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0, 1, 0,   0, 0, 1,   1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0, 0, 1,   0,-1, 0,   1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0,-1, 0,   0, 0,-1,   1, 0, 0).finished(),
    
    (Eigen::Matrix3i() << -1, 0, 0,   0,-1, 0,   0, 0, 1).finished(),
    (Eigen::Matrix3i() << -1, 0, 0,   0, 0,-1,   0,-1, 0).finished(),
    (Eigen::Matrix3i() << -1, 0, 0,   0, 1, 0,   0, 0,-1).finished(),
    (Eigen::Matrix3i() << -1, 0, 0,   0, 0, 1,   0, 1, 0).finished(),
    
    (Eigen::Matrix3i() <<  0, 1, 0,  -1, 0, 0,   0, 0, 1).finished(),
    (Eigen::Matrix3i() <<  0, 0, 1,  -1, 0, 0,   0,-1, 0).finished(),
    (Eigen::Matrix3i() <<  0,-1, 0,  -1, 0, 0,   0, 0,-1).finished(),
    (Eigen::Matrix3i() <<  0, 0,-1,  -1, 0, 0,   0, 1, 0).finished(),
    
    (Eigen::Matrix3i() <<  0, 0,-1,   0,-1, 0,  -1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0,-1, 0,   0, 0, 1,  -1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0, 0, 1,   0, 1, 0,  -1, 0, 0).finished(),
    (Eigen::Matrix3i() <<  0, 1, 0,   0, 0,-1,  -1, 0, 0).finished(),
        
};


}     

namespace block {

std::vector<Eigen::Vector3i> block_face_positions = {
    { 1, 0, 0},
    { 0, 1, 0},
    { 0, 0, 1},
    {-1, 0, 0},
    { 0,-1, 0},
    { 0, 0,-1},
};

block::block(point::point position_) {
    position = position_;
    type_ = air;
    faces = std::vector<face::face>(6, face::none);
}

block::block(point::point position_, type type__, face::face face_) {
    position = position_;
    type_ = type__;
    faces = std::vector<face::face>(6, face_);
}

block::block(point::point position_, type type__, std::vector<face::face> faces_) {
    position = position_;
    type_ = type__;
    faces = faces_;
}

face::face block::face_at_point(point::point point_) {
    if (point_ == point::point( 1, 0, 0)) return faces[0];
    if (point_ == point::point( 0, 1, 0)) return faces[1];
    if (point_ == point::point( 0, 0, 1)) return faces[2];
    if (point_ == point::point(-1, 0, 0)) return faces[3];
    if (point_ == point::point( 0,-1, 0)) return faces[4];
    if (point_ == point::point( 0, 0,-1)) return faces[5];
    std::abort();
}

std::vector<block> block::get_all_rotations() {
    std::vector<block> output;

    for (Eigen::Matrix3i rotation : all_cube_rotations) {

        std::vector<Eigen::Vector3i> face_positions_rotated;
        for (Eigen::Vector3i face_position : block_face_positions) {
            face_positions_rotated.push_back(rotation * face_position);
        }

        std::vector<face::face> faces_;
        for (Eigen::Vector3i face_position : face_positions_rotated) {
            faces_.push_back(face_at_point(face_position));
        }

        output.push_back(block(point::point(0, 0, 0), type_, faces_));

    }

    return output;
}

bool block::propogates_cram_connection(std::shared_ptr<block> other) {
    if ((position - other->position).squaredNorm() != 1) continue;
    
}

std::string block::to_string() {
    std::string output = std::format("block{}", "{");
    output += std::format("{:>1}|", (int)type_);
    for (auto face : faces)
        output += std::format("{:>1} ", (int)face.type_);
    output = output.substr(0, output.size() - 1);
    output += std::format("{}", "}");
    return output;
}

}