#ifndef MORPH_COREALLOCATION_H
#define MORPH_COREALLOCATION_H

#include "Rect.h"

#include <cstddef>
#include <vector>

namespace parallel {

    struct Chunk {
        int type;
        Rect rect;
    };

    class CoreAllocation {

        int chunksH_, chunksV_;
        int coresH_, coresV_;
        int excessCores_;
        std::vector<std::vector<Chunk>> allocation_;

    public:

        using gaps = std::vector<int>;

        CoreAllocation(const gaps &hSteps, const gaps &vSteps, int coresH, int coresV, int totalCores, std::vector<std::vector<Chunk>> &&allocation)
            : chunksH_(static_cast<int>(hSteps.size())),
              chunksV_(static_cast<int>(vSteps.size())),
              coresH_(coresH),
              coresV_(coresV),
              excessCores_(totalCores - static_cast<int>(allocation.size())),
              allocation_(std::move(allocation)) {}

        /// Total horizontal number of chunks
        int chunksH() const {
            return chunksH_;
        }

        /// Total vertical number of chunks
        int chunksV() const {
            return chunksV_;
        }

        /// Number of cores allocated on a row
        int coresH() const {
            return coresH_;
        }

        /// Number of cores allocated on a column
        int coresV() const {
            return coresV_;
        }

        /// Excess cores not used in this configuration
        int excessCores() const {
            return excessCores_;
        }

        /// Array of chunks allocated for the given core
        const std::vector<std::vector<Chunk>> &allocation() const {
            return allocation_;
        }

    };

}

#endif //MORPH_COREALLOCATION_H
