#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "../src/block/block.hpp"
#include "../src/block/grid.hpp"
#include "../src/solver/solver.hpp"
#include "../src/globals/globals.hpp"

TEST_CASE("Test block constructors and print") {
    block::block block_(point::point(0, 0, 0), block::air, face::none);
    REQUIRE(block_.to_string() == "block{0|0 0 0 0 0 0}");
    block_ = block::block(point::point(0, 0, 0), block::mantlet, {face::none,face::cannon_face,face::connector,face::connector_blank,face::gauge,face::packer_base});
    REQUIRE(block_.to_string() == "block{1|0 1 2 3 4 5}");
}

TEST_CASE("Test block::grid's add_point, add_box, and add_cylinder") {
    block::grid grid_;
    REQUIRE(grid_.blocks.size() == 0);

    // test add_point by size
    int desired = 0;
    grid_.add_point(point::point(0, 0, 0));
    desired++;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_point(point::point(0, 0, 1));
    desired++;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_point(point::point(123, 456, 789));
    desired++;
    REQUIRE(grid_.blocks.size() == desired);

    // test add_box by size
    grid_.add_box(point::point(0, 0, 0), point::point(0, 0, 0));
    desired += 0;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_box(point::point(0, 0, 0), point::point(1, 1, 10));
    desired += 8;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_box(point::point(200, 0, 0), point::point(1, 1, 10));
    desired += 10;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_box(point::point(-200, -200, -200), point::point(10, 10, 10));
    desired += 1000;
    REQUIRE(grid_.blocks.size() == desired);
    
    // test add_cylinder by size
    grid_.add_cylinder(point::point(0, 0, -10000), 1, 1);
    desired++;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_cylinder(point::point(0, 0, -10001), 3, 1);
    desired += 9;
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_cylinder(point::point(0, 0, -10010), 5, 2);
    desired += 2*(25 - 4);
    REQUIRE(grid_.blocks.size() == desired);
    grid_.add_cylinder(point::point(0, 0, -10000), 7, 10);
    desired += 10*(49 - 4*3) - 1;
    REQUIRE(grid_.blocks.size() == desired);
}

TEST_CASE("Test grid get, set, delete operations") {
    block::grid grid_;
    grid_.add_box(point::point(0, 0, 0), point::point(1, 1, 3));
    for (int i = 0; i < 3; i++) {
        REQUIRE(grid_.get_block(point::point(0, 0, i)).has_value() == true);
        REQUIRE(grid_.get_block(point::point(0, 0, i)).value()->position == point::point(0, 0, i));
        REQUIRE(grid_.get_block(point::point(0, 0, i)).value()->type_ == block::type::air);
    }
    REQUIRE(grid_.get_block(point::point( 0, 0,-1)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0, 0, 4)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 1, 0, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point(-1, 0, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0, 1, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0,-1, 0)).has_value() == false);

    grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::type::mantlet, face::type::cannon_face)));
    for (int i = 0; i < 3; i++) {
        REQUIRE(grid_.get_block(point::point(0, 0, i)).has_value() == true);
        REQUIRE(grid_.get_block(point::point(0, 0, i)).value()->position == point::point(0, 0, i));
        REQUIRE(grid_.get_block(point::point(0, 0, i)).value()->type_ == (i == 0 ? block::type::mantlet : block::type::air));
    }
    REQUIRE(grid_.get_block(point::point( 0, 0,-1)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0, 0, 4)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 1, 0, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point(-1, 0, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0, 1, 0)).has_value() == false);
    REQUIRE(grid_.get_block(point::point( 0,-1, 0)).has_value() == false);
}

TEST_CASE("Block rotations work as expected") {
    block::block unrotated_block(point::point(0, 0, 0), block::mantlet, {
        (face::type)0,
        (face::type)1,
        (face::type)2,
        (face::type)3,
        (face::type)4,
        (face::type)5,
    });
    std::vector<block::block> rotations = unrotated_block.get_all_rotations();

    // verify that each value [0-6) shows up once
    for (block::block block_ : rotations) {
        for (int check_value = 0; check_value < 6; check_value++) {
            int count = 0;
            for (int i = 0; i < 6; i++) {
                if (block_.faces[i].type_ == (face::type)check_value) count++;
            }
            REQUIRE(count == 1);
        }
    }
    
    // verify position[facetype 0] x position[facetype 1] = position[facetype 2] (side == 0)
    // verify position[facetype 3] x position[facetype 4] = position[facetype 5] (side == 1)
    for (block::block block_ : rotations) {
        for (int side = 0; side < 1; side++) {
            int index_a;
            int index_b;
            int index_c;
            for (int i = 0; i < 6; i++) {
                if (block_.faces[i].type_ == (face::type)(0 + 3*side)) index_a = i;
                if (block_.faces[i].type_ == (face::type)(1 + 3*side)) index_b = i;
                if (block_.faces[i].type_ == (face::type)(2 + 3*side)) index_c = i;
            }
            point::point a = block::block_face_positions[index_a];
            point::point b = block::block_face_positions[index_b];
            point::point c = block::block_face_positions[index_c];
            REQUIRE(a.cross(b) == c);
        }
    }
}

TEST_CASE("Test extend_cram_cannon") {
    {
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), (block::type)1, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), (block::type)2, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), (block::type)3, face::connector)));
        solver::solver solver_;
        solver_.solution = grid_;
        std::vector<std::shared_ptr<block::block>> cram_cannon = {grid_.get_block(point::point(0, 0, 0)).value()};
        REQUIRE(cram_cannon.size() == 1);
        solver_.extend_cram_cannon(cram_cannon);
        REQUIRE(cram_cannon.size() == 2);
        solver_.extend_cram_cannon(cram_cannon);
        REQUIRE(cram_cannon.size() == 3);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), (face::type)(rand() % 10))));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        std::vector<std::shared_ptr<block::block>> cram_cannon = {grid_.get_block(point::point(0, 0, 0)).value()};
        REQUIRE(cram_cannon.size() == 1);
        solver_.extend_cram_cannon(cram_cannon);
        REQUIRE(cram_cannon.size() == 1 + 6);
        solver_.extend_cram_cannon(cram_cannon);
        REQUIRE(cram_cannon.size() == 27 - 8);
        solver_.extend_cram_cannon(cram_cannon);
        REQUIRE(cram_cannon.size() == 27);
    }

    
}


