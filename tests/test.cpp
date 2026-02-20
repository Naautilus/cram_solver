#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "../src/block/block.hpp"
#include "../src/block/grid.hpp"
#include "../src/solver/solver.hpp"
#include "../src/globals/globals.hpp"

TEST_CASE("Test block constructors and print") {
    block::block block_(point::point(0, 0, 0), block::air, face::none);
    REQUIRE(block_.to_string() == "block{ 0|0 0 0 0 0 0} @ (  0  0  0)");
    block_ = block::block(point::point(0, 0, 0), block::mantlet, {face::none,face::cannon_face,face::connector,face::connector_blank,face::gauge,face::packer_base});
    REQUIRE(block_.to_string() == "block{ 1|0 1 2 3 4 5} @ (  0  0  0)");
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

    grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), block::type::connector_2, face::type::connector_blank)));
    grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), block::type::connector_3, face::type::connector)));
    REQUIRE(grid_.get_block(point::point(0, 0, 2)).has_value() == true);
    REQUIRE(grid_.get_block(point::point(0, 0, 2)).value()->position == point::point(0, 0, 2));
    REQUIRE(grid_.get_block(point::point(0, 0, 2)).value()->type_ == block::connector_3);

    grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), block::type::connector_2, face::type::connector_blank)));
    std::shared_ptr<block::block> same_block_change_but_without_getting = 
                                        std::make_shared<block::block>(block::block(point::point(0, 0, 2), block::type::connector_3, face::type::connector));
    REQUIRE(same_block_change_but_without_getting->position == point::point(0, 0, 2));
    REQUIRE(same_block_change_but_without_getting->type_ == block::connector_3);
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

TEST_CASE("Test set_block does not invalidate indices") {
    {
        block::grid grid0;
        grid0.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid0.blocks) {
            block_->type_ = (block::type)3;
            block_->faces = std::vector<face::face>(6, (face::type)3);
        }
        
        block::grid grid1;
        grid1.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid1.blocks) {
            grid1.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)3, (face::type)3)));
        }
        
        REQUIRE(grid0.blocks.size() == grid1.blocks.size());
        for (int i = 0; i < grid0.blocks.size(); i++) {
            REQUIRE(grid0.blocks[i]->position == grid1.blocks[i]->position);
            for (int j = 0; j < 6; j++) {
                REQUIRE(grid0.blocks[i]->faces[j].type_ == grid1.blocks[i]->faces[j].type_);
            }
        }
    }
}

TEST_CASE("Test extend_cram_cannon") {
    /*
     * 7 tests:
     *  - make a fully connectible 1x1x3 pillar: test for size 1, then 2, then 3 as the cram extends
     *  - make a fully connectible 3x3x3 box: test for sizes {1, 1+6, 27-8, 27}
     *  - Test 1 but with an air block in the middle of the pillar: size stays at 1
     *  - Test 2 but with an air block at each of the 8 corners: size stays at 27-8
     *  - make a 3x3x3 box with an air gap in the center: test for sizes {1, 5, 13, 21, 26} as the cram extends from (1, 0, 0)
     *  - make a 1x1x3 pillar of connectors with an empty face on +Z: size stays at 1
     *  - make a 1x1x3 pillar of connectors with an empty face on +X,+Y,-X,-Y: test for sizes {1, 2, 3}
    */
    {
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), (block::type)1, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), (block::type)2, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), (block::type)3, face::connector)));
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 2);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 3);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));

        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)4, face::connector)));
        }
        
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1 + 6);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27 - 8);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27);
    }
    {
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), (block::type)1, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), (block::type)2, face::none)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), (block::type)3, face::connector)));
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            if (block_->position.squaredNorm() == 3)
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::none)));
            else
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::connector)));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1 + 6);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27 - 8);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27 - 8);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27 - 8);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 27 - 8);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            if (block_->position.squaredNorm() == 0)
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::none)));
            else
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::connector)));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(1, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 5);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 13);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 21);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 26);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 26);
    }
    {
        block::grid grid_;
        for (int i = 0; i < 3; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), (block::type)1, {
                face::connector,
                face::connector,
                face::none,
                face::connector,
                face::connector,
                face::connector,
            })));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 1);
    }
    {
        block::grid grid_;
        for (int i = 0; i < 3; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), (block::type)1, {
                face::none,
                face::none,
                face::connector,
                face::none,
                face::none,
                face::connector,
            })));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = {{grid_.get_block(point::point(0, 0, 0)).value()}};
        REQUIRE(cannon_.blocks.size() == 1);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 2);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 3);
        solver_.extend_cram_cannon(cannon_);
        REQUIRE(cannon_.blocks.size() == 3);
    }
}

TEST_CASE("Test get_full_cram_cannon") {
    // Same 7 tests as "Test extend_cram_cannon", but with extend_cram_cannon replaced with get_full_cram_cannon
    {
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), (block::type)1, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), (block::type)2, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), (block::type)3, face::connector)));
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 3);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));

        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)4, (face::type)4)));
        }
        
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 27);
    }
    {
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), (block::type)1, face::connector)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), (block::type)2, face::none)));
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 2), (block::type)3, face::connector)));
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 1);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            if (block_->position.squaredNorm() == 3)
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::none)));
            else
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::connector)));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 27 - 8);
    }
    {
        block::grid grid_;
        grid_.add_box(point::point(-1,-1,-1), point::point( 3, 3, 3));
        for (std::shared_ptr<block::block> block_ : grid_.blocks) {
            if (block_->position.squaredNorm() == 0)
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::none)));
            else
                grid_.set_block(std::make_shared<block::block>(block::block(block_->position, (block::type)(rand() % 10), face::connector)));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(1, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 26);
    }
    {
        block::grid grid_;
        for (int i = 0; i < 3; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), (block::type)1, {
                face::connector,
                face::connector,
                face::none,
                face::connector,
                face::connector,
                face::connector,
            })));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 1);
    }
    {
        block::grid grid_;
        for (int i = 0; i < 3; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), (block::type)1, {
                face::none,
                face::none,
                face::connector,
                face::none,
                face::none,
                face::connector,
            })));
        }
        solver::solver solver_;
        solver_.solution = grid_;
        cannon::cannon cannon_ = solver_.get_full_cram_cannon(grid_.get_block(point::point(0, 0, 0)).value());
        REQUIRE(cannon_.blocks.size() == 3);
    }
}

TEST_CASE("Test find_first_block_of_type") {
    { // find a mantlet that exists
        block::grid grid_;
        for (int i = 0; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::mantlet, face::connector)));
            auto output = grid_.find_first_block_of_type(block::mantlet);
            REQUIRE(output.has_value() == true);
        }
    }
    { // don't find a mantlet that doesn't exist
        block::grid grid_;
        for (int i = 0; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, face::connector)));
            auto output = grid_.find_first_block_of_type(block::mantlet);
            REQUIRE(output.has_value() == false);
        }
    }
}

TEST_CASE("Test get_all_cram_cannon_metrics") {
    { // connectors with no mantlet should not create any cram cannon
        block::grid grid_;
        for (int i = 0; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, face::connector)));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 0);
        }
    }
    { // connectors with 1 mantlet should create a cram cannon with correct block count, no pellet connections, no compactor connections
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, face::connector)));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 1);
            REQUIRE(all_metrics[0].block_count == i+1);
            REQUIRE(all_metrics[0].pellet_connections == 0);
            REQUIRE(all_metrics[0].compactor_connections == 0);
        }
    }
    { // pellets + packers with 1 mantlet should create a cram cannon with correct block count, correct pellet connections, no compactor connections
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, {
                face::packer_payload,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
            })));
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::connector_6, {
                face::connector,
                face::connector,
                face::connector,
                face::pellet,
                face::connector,
                face::connector,
            })));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 1);
            REQUIRE(all_metrics[0].block_count == 2*i+1);
            REQUIRE(all_metrics[0].pellet_connections == i);
            REQUIRE(all_metrics[0].compactor_connections == 0);
        }
    }
    { // compactors + packers with 1 mantlet should create a cram cannon with correct block count, no pellet connections, correct compactor connections
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, {
                face::packer_payload,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
            })));
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::connector_6, {
                face::connector,
                face::connector,
                face::connector,
                face::compactor,
                face::connector,
                face::connector,
            })));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 1);
            REQUIRE(all_metrics[0].block_count == 2*i+1);
            REQUIRE(all_metrics[0].pellet_connections == 0);
            REQUIRE(all_metrics[0].compactor_connections == i);
        }
    }
    { // pellet face on the wrong side should not make a connection
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, {
                face::packer_payload,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
            })));
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::connector_6, {
                face::pellet,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
            })));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 1);
            REQUIRE(all_metrics[0].block_count == i+1);
            REQUIRE(all_metrics[0].pellet_connections == 0);
            REQUIRE(all_metrics[0].compactor_connections == 0);
        }
    }
    { // compactors + pellets should not make a connection
        block::grid grid_;
        grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, {
                face::pellet,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
                face::connector,
            })));
            grid_.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::connector_6, {
                face::connector,
                face::connector,
                face::connector,
                face::compactor,
                face::connector,
                face::connector,
            })));
            solver::solver solver_;
            solver_.solution = grid_;
            std::vector<cannon::metrics> all_metrics = solver_.get_all_cram_cannon_metrics();
            REQUIRE(all_metrics.size() == 1);
            REQUIRE(all_metrics[0].block_count == i+1);
            REQUIRE(all_metrics[0].pellet_connections == 0);
            REQUIRE(all_metrics[0].compactor_connections == 0);
        }
    }
}

TEST_CASE("Test score_current_solution") {
    { 
        // create two connector stacks, grid0 and grid1; grid0 always has 1 more block than grid1; its score should always be higher
        block::grid grid0, grid1;
        grid0.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        grid1.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 0), block::mantlet, face::connector)));
        
        grid0.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 1), block::connector_6, face::connector)));
        for (int i = 1; i < 100; i++) {
            grid0.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i+1), block::connector_6, face::connector)));
            grid1.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, i), block::connector_6, face::connector)));
            solver::solver solver0, solver1;
            solver0.solution = grid0;
            solver1.solution = grid1;
            double score0 = solver0.score_current_solution();
            double score1 = solver1.score_current_solution();
            REQUIRE(score0 > 0);
            REQUIRE(score1 > 0);
            REQUIRE(score0 > score1);
        }
        
        // equalize grid0 and grid1
        grid1.set_block(std::make_shared<block::block>(block::block(point::point(0, 0, 100), block::connector_6, face::connector)));
        {
            solver::solver solver0, solver1;
            solver0.solution = grid0;
            solver1.solution = grid1;
            double score0 = solver0.score_current_solution();
            double score1 = solver1.score_current_solution();
            REQUIRE(score0 > 0);
            REQUIRE(score1 > 0);
            REQUIRE(score0 == score1);
        }

        // add a row of packers and compactors to both grid0 and grid1; grid0 always has one more than grid1; grid0 should always have more points
        // grid1 gets extra connectors to have equal blocks
        grid1.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, 0), block::connector_6, face::connector)));
        grid1.set_block(std::make_shared<block::block>(block::block(point::point(2, 0, 0), block::connector_6, face::connector)));
        for (int i = 0; i < 100; i++) {
            grid0.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::packer, {
                face::packer_payload,
                face::packer_payload,
                face::packer_payload,
                face::packer_base,
                face::packer_payload,
                face::packer_payload,
            })));
            if (i > 0) {
                grid1.set_block(std::make_shared<block::block>(block::block(point::point(1, 0, i), block::packer, {
                    face::packer_payload,
                    face::packer_payload,
                    face::packer_payload,
                    face::packer_base,
                    face::packer_payload,
                    face::packer_payload,
                })));
            }

            // the ideal balance of pellets and compactors is around 70% compactors past the first 9 pellets
            face::type type_to_add;
            if (i < 9)
                type_to_add = face::pellet;
            else
                type_to_add = (fmod((double)i, 1.0/0.7) < 1.0 ? face::compactor : face::pellet);

            grid0.set_block(std::make_shared<block::block>(block::block(point::point(2, 0, i), block::pellet, type_to_add)));
            if (i > 0) {
                grid1.set_block(std::make_shared<block::block>(block::block(point::point(2, 0, i), block::compactor, type_to_add)));
            }
            solver::solver solver0, solver1;
            solver0.solution = grid0;
            solver1.solution = grid1;
            double score0 = solver0.score_current_solution();
            double score1 = solver1.score_current_solution();
            REQUIRE(score0 > 0);
            REQUIRE(score1 > 0);
            REQUIRE(score0 > score1);
        }

        // grid1 gets 50 of its mixed payload blocks replaced with purely compactors, worsening its ratio
        for (int i = 0; i < 50; i++) {
            grid1.set_block(std::make_shared<block::block>(block::block(point::point(2, 0, i), block::compactor, face::pellet)));
        }
        solver::solver solver0, solver1;
        solver0.solution = grid0;
        solver1.solution = grid1;
        double score0 = solver0.score_current_solution();
        double score1 = solver1.score_current_solution();
        REQUIRE(score0 > 0);
        REQUIRE(score1 > 0);
        REQUIRE(score0 > score1);
    }
}

TEST_CASE("Test that modify_solution actually changes anything") {
    globals::initialize_blocks();
    for (int i = 0; i < 100; i++) {
        block::grid grid0;
        grid0.add_box(point::point(-1, -1, -1), point::point(3, 3, 3));
        grid0.set_block(std::make_shared<block::block>(block::block(
            point::point(0, 0, -1),
            globals::forwards_mantlet.type_,
            globals::forwards_mantlet.faces
        )));
    
        solver::solver solver_;
        solver_.solution = grid0;
        solver_.modify_solution();
        block::grid grid1 = solver_.solution;

        REQUIRE(grid0.blocks.size() == grid1.blocks.size());
        bool anything_changed = false;
        for (int i = 0; i < grid0.blocks.size(); i++) {
            if (grid0.blocks[i]->position != grid1.blocks[i]->position) anything_changed = true;
            if (grid0.blocks[i]->type_ != grid1.blocks[i]->type_) anything_changed = true;
            for (int j = 0; j < 6; j++) {
                if (grid0.blocks[i]->faces[j].type_ != grid1.blocks[i]->faces[j].type_) anything_changed = true;
            }
        }
        REQUIRE(anything_changed == true);
    }
}