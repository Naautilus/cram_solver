#include "globals.hpp"

namespace globals {

std::vector<block::block> blocks = {};

block::block forwards_mantlet = block::block(point::point(0, 0, 0), block::mantlet, {
    face::cannon_face,      face::connector,        face::connector,
    face::connector,        face::connector,        face::connector
});

namespace {

std::vector<block::block> blocks_unrotated = {
    // air
    block::block(point::point(0, 0, 0)),
    
    // mantlet
    block::block(point::point(0, 0, 0), block::mantlet, {
        face::cannon_face,      face::connector,        face::connector,
        face::connector,        face::connector,        face::connector}),

    // 2-way connector
    block::block(point::point(0, 0, 0), block::connector_2, {
        face::connector,        face::connector_blank,  face::connector_blank,
        face::connector_blank,  face::connector_blank,  face::connector}),

    // 3-way connector
    block::block(point::point(0, 0, 0), block::connector_3, {
        face::connector_blank,  face::connector,        face::connector_blank,
        face::connector_blank,  face::connector,        face::connector}),
        
    // 4-way connector
    block::block(point::point(0, 0, 0), block::connector_4, {
        face::connector,        face::connector,        face::connector_blank,
        face::connector,        face::connector,        face::connector_blank}),
        
    // 5-way connector
    block::block(point::point(0, 0, 0), block::connector_5, {
        face::connector,        face::connector,        face::connector_blank,
        face::connector,        face::connector,        face::connector}),

    // 6-way connector
    block::block(point::point(0, 0, 0), block::connector_6, {
        face::connector,        face::connector,        face::connector,
        face::connector,        face::connector,        face::connector}),

    // 2-way gauge
    block::block(point::point(0, 0, 0), block::gauge_2, {
        face::none,             face::none,             face::gauge,
        face::none,             face::none,             face::gauge}),

    // 3-way gauge
    block::block(point::point(0, 0, 0), block::gauge_3, {
        face::none,             face::gauge,            face::gauge,
        face::none,             face::gauge,            face::none}),

    // 4-way gauge
    block::block(point::point(0, 0, 0), block::gauge_4, {
        face::gauge,           	face::gauge,            face::gauge,
        face::none,            	face::gauge,            face::none}),

    // 6-way gauge
    block::block(point::point(0, 0, 0), block::gauge_6, face::gauge),

    // payload packer
    block::block(point::point(0, 0, 0), block::packer, {
        face::packer_payload,   face::packer_base,      face::packer_payload,
        face::packer_base,      face::packer_base,      face::packer_payload}),

    // compactor
    block::block(point::point(0, 0, 0), block::compactor, face::compactor),

    // pellet
    block::block(point::point(0, 0, 0), block::pellet, face::pellet),

};

}

void initialize_blocks() {
    for (block::block block_ : blocks_unrotated) {
        std::vector<block::block> block_rotations = block_.get_all_rotations();
        blocks.insert(blocks.end(), block_rotations.begin(), block_rotations.end());
    }
}

}