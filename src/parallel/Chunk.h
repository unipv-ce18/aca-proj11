#ifndef MORPH_CHUNK_H
#define MORPH_CHUNK_H

#include "Rect.h"

#define CHUNK_REGULAR   0
#define CHUNK_NW        1
#define CHUNK_N         2
#define CHUNK_NE        3
#define CHUNK_E         4
#define CHUNK_SE        5
#define CHUNK_S         6
#define CHUNK_SW        7
#define CHUNK_W         8

namespace parallel {

    struct Chunk {
        int type;
        Rect rect;
    };

}

#endif //MORPH_CHUNK_H
