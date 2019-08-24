#ifndef MORPH_OSTREAM_DUMP_H
#define MORPH_OSTREAM_DUMP_H

#include <ostream>

#include "Plan.h"

namespace parallel {
    namespace print_format_json {
        std::ostream &operator<<(std::ostream &os, Plan &plan);
        std::ostream &operator<<(std::ostream &os, RegionSpec &plan);
        std::ostream &operator<<(std::ostream &os, const Rect &plan);
    }
}

#endif //MORPH_OSTREAM_DUMP_H
