#include "planners.h"

#include "Plan.h"
#include "even_region.h"
#include "ostream_dump.h"

parallel::Plan parallel::planSimdExecution(int cores, int imgW, int imgH, int sap) {
    RegionSpec rs = calculateRegions(imgW, imgH, cores);
    // TODO: Actual chunk placement

    return {imgW, imgH, cores, sap, std::move(rs)};
}
