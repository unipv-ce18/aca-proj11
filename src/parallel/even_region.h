#ifndef MORPH_EVEN_REGION_H
#define MORPH_EVEN_REGION_H

#include "RegionSpec.h"

namespace parallel {

    /**
     * Internal planning function to evenly subdivide an image for the given number of cores.
     *
     * As an example, images with a very large aspect ratio may end up having a single row and multiple columns,
     * while images with a ratio near to 1 (square) will have the same number of rows and columns.
     *
     * Since the regions form up a grid, some excess cores may end up unused.
     *
     * @param imgW The width of the image
     * @param imgH The height of the image
     * @param cores The number of cores and upper bound of the total region count
     * @return The resulting region specification
     */
    RegionSpec calculateRegions(int imgW, int imgH, int cores);

}

#endif //MORPH_EVEN_REGION_H
