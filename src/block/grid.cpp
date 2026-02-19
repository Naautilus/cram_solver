#include "grid.hpp"
#include <iostream>

namespace block {

grid::grid(){};

bool grid::contains_point(point::point position) {
    for (std::shared_ptr<block> block_ : blocks) {
        if (block_->position == position) return true;
    }
    return false;
}

bool grid::add_point(point::point position) {
    if (contains_point(position)) return false;
    std::shared_ptr<block> block_ = std::make_shared<block>(block(position));
    blocks.push_back(block_);
    update_cache();
    return true;
}

void grid::add_box(point::point position, point::point size) {
    for (point::point position_ : point::points_in_range(position, size)) {
        add_point(position_);
    }
}

void grid::add_cylinder(point::point position, int diameter, int height) {
    if (diameter % 2 == 0) diameter--;
    for (point::point position_ : point::points_in_range(position - point::point(diameter/2+1, diameter/2+1, 0), point::point(diameter+2, diameter+2, height))) {
        point::point relative_position = position_ - position;
        relative_position.z() = 0;
        if (relative_position.squaredNorm() <= diameter*diameter/4) {
            add_point(position_);
        }
    }
}

std::optional<std::shared_ptr<block>> grid::find_first_block_of_type(type type_) {
    for (std::shared_ptr<block> block_ : blocks) {
        if (block_->type_ = type_) return std::make_optional<std::shared_ptr<block>>(block_);
    }
    return std::nullopt;
}


std::vector<std::shared_ptr<block>> grid::find_blocks_of_type(type type_) {
    std::vector<std::shared_ptr<block>> output;
    for (std::shared_ptr<block> block_ : blocks) {
        if (block_->type_ = type_) output.push_back(block_);
    }
    return output;
}


void grid::update_cache() {
    block_location_cache.clear();
    for (std::shared_ptr<block> block_ : blocks) {
        block_location_cache[block_->position] = block_;
    }
}

std::optional<std::shared_ptr<block>> grid::get_block(point::point position) {
    if (!block_location_cache.contains(position)) return std::nullopt;
    return block_location_cache[position];
}

void grid::set_block(std::shared_ptr<block> block_) {
    std::optional<std::shared_ptr<block>> block_at_point = get_block(block_->position);
    if (block_at_point.has_value()) delete_block(block_at_point.value());
    blocks.push_back(block_);
    update_cache();
}

bool grid::delete_block(std::shared_ptr<block> block_) {
    auto location = std::find(blocks.begin(), blocks.end(), block_);
    if (location == blocks.end()) return false;
    blocks.erase(location);
    update_cache();
    return true;
}

}