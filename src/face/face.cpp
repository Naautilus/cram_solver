#include <algorithm>
#include <iostream>
#include "face.hpp"

namespace face {

namespace {

bool propogates_cram_connection_internal(type a, type b) {
    if (a > b) std::swap(a, b);

    if (a == none) {
        if (b == none           ) return false;
        if (b == cannon_face    ) return false;
        if (b == connector      ) return false;
        if (b == connector_blank) return false;
        if (b == gauge          ) return false;
        if (b == packer_base    ) return false;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == cannon_face) {
        if (b == cannon_face    ) return false;
        if (b == connector      ) return false;
        if (b == connector_blank) return false;
        if (b == gauge          ) return false;
        if (b == packer_base    ) return false;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == connector) {
        if (b == connector      ) return true;
        if (b == connector_blank) return false;
        if (b == gauge          ) return true;
        if (b == packer_base    ) return true;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return true;
        if (b == pellet         ) return true;  
    }

    else if (a == connector_blank) {
        if (b == connector_blank) return false;
        if (b == gauge          ) return false;
        if (b == packer_base    ) return false;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == gauge) {
        if (b == gauge          ) return true;
        if (b == packer_base    ) return false;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == packer_base) {
        if (b == packer_base    ) return false;
        if (b == packer_payload ) return false;
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == packer_payload) {
        if (b == packer_payload ) return false;
        if (b == compactor      ) return true;
        if (b == pellet         ) return true;
    }

    else if (a == compactor) {
        if (b == compactor      ) return false;
        if (b == pellet         ) return false;
    }

    else if (a == pellet) {
        if (b == pellet         ) return false;
    }

    // should not be reached with any valid combination of face types
    std::cerr << "fatal error in propogates_cram_connection_internal; aborting\n";
    std::abort();
}

bool is_pellet_connection_internal(type a, type b) {
    if (a > b) std::swap(a, b);
    if (a == packer_payload && b == pellet) return true;
    return false;
}

bool is_compactor_connection_internal(type a, type b) {
    if (a > b) std::swap(a, b);
    if (a == packer_payload && b == compactor) return true;
    return false;
}

}

bool face::check_for_relation(face other, relation relation_) {
    switch(relation_) {
        case connection:
            return propogates_cram_connection_internal(type_, other.type_);
        case pellet_packing:
            return is_pellet_connection_internal(type_, other.type_);
        case compactor_packing:
            return is_compactor_connection_internal(type_, other.type_);
        default: std::abort();
    }
}

}
