#ifndef MORPH_REGIONSPEC_H
#define MORPH_REGIONSPEC_H

#include "Rect.h"

#include <cstddef>
#include <vector>

namespace parallel {

    class RegionSpec {

        int coresH_;
        int coresV_;
        int excessCores_;

        // Ideally use a fixed-size std::array but it allocates everything on the stack
        std::vector<Rect> data_;

    public:

        RegionSpec(int totalCores, int coresH, int coresV) :
            coresH_(coresH),
            coresV_(coresV),
            excessCores_(totalCores - coresH * coresV),
            data_(static_cast<size_t>(coresH * coresV)) {}

        /// Number of columns in this region grid
        int coresH() const {
            return coresH_;
        }

        /// Number of rows in this region grid
        int coresV() const {
            return coresV_;
        }

        /// Number of cores in the plan that do not fit in the grid
        int excessCores() const {
            return excessCores_;
        }

        /// Array of rectangles defining the regions in the image
        std::vector<Rect> &data() {
            return data_;
        }

    };

}

#endif //MORPH_REGIONSPEC_H
