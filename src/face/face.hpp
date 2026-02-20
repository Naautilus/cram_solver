#pragma once

namespace face {

enum type {
    none,
    cannon_face,
    connector,
    connector_blank,
    gauge,
    packer_base,
    packer_payload,
    compactor,
    pellet,
};

enum relation {
    connection,
    pellet_packing,
    compactor_packing
};

struct face {
    type type_;
    face(type type__) {
        type_ = type__;
    }
    bool check_for_relation(face other, relation relation_);
};

}