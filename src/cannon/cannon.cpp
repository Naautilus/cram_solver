#include "cannon.hpp"

namespace cannon {

metrics cannon::get_metrics() {
    metrics output;
    output.block_count = blocks.size();
    for (int i = 0; i < blocks.size(); i++) {
        for (int j = i + 1; j < blocks.size(); j++) {
            if ((blocks[i]->position - blocks[j]->position).squaredNorm() != 1) continue;
            if (blocks[i]->check_for_relation(blocks[j], face::pellet_packing))
                output.pellet_connections++;
            if (blocks[i]->check_for_relation(blocks[j], face::compactor_packing))
                output.compactor_connections++;
        }
    }
    return output;
}

}