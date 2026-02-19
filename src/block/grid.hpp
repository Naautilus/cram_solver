#pragma once
#include <unordered_map>
#include <optional>
#include <Eigen/Geometry>
#include "block.hpp"

namespace block {

struct grid {
    std::vector<std::shared_ptr<block>> blocks;
    std::unordered_map<point::point, std::shared_ptr<block>, point::point_hasher> block_location_cache;

    grid();

    bool contains_point(point::point position);
    // add_point returns false if there was any overlap with existing points
    bool add_point(point::point position);
    void add_box(point::point position, point::point size);
    void add_cylinder(point::point position, int diameter, int height); // from top center
    std::optional<std::shared_ptr<block>> find_first_block_of_type(type type_);
    std::vector<std::shared_ptr<block>> find_blocks_of_type(type type_);
    std::optional<std::shared_ptr<block>> get_block(point::point position);
    void set_block(std::shared_ptr<block> block_);
    bool erase_block(std::shared_ptr<block> block_); // INVALIDATES INDICES! Do not iterate over the list with this.
    bool replace_block(std::shared_ptr<block> block_old, std::shared_ptr<block> block_new); // returns true if a block was replaced

    private:
    void update_cache();

};

}