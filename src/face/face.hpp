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

struct face {
    type type_;
    face(type type__) {
        type_ = type__;
    }
    bool propogates_cram_connection(face other);
};

}