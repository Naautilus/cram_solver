#include <catch2/catch_test_macros.hpp>
#include "../src/block/block.hpp"

TEST_CASE("Block constructors and print are working as intended") {
    block::block block_(block::air, face::none);
    REQUIRE(block_.to_string() == "block{0|0 0 0 0 0 0}");
    block_ = block::block(block::mantlet, {face::none,face::cannon_face,face::connector,face::connector_blank,face::gauge,face::packer_base});
    REQUIRE(block_.to_string() == "block{1|0 1 2 3 4 5}");
}