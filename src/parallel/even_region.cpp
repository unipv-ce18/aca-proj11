#include "even_region.h"

#include <cmath>
#include <vector>

#ifdef MORPH_DEBUG_PLANNER
#include <iostream>
#include <iomanip>

struct _DbgRegionStep {
    float w, h, r, t, d;
};

inline _DbgRegionStep dbgRegionStep(float w, float h, float ratio, float target, float diff) {
    return {w, h, ratio, target, diff};
}

inline std::ostream &operator<<(std::ostream &os, const _DbgRegionStep &_d) {
    return os << std::fixed
              << std::setprecision(1) << _d.w << '*' << _d.h
              << std::setprecision(2) << " ratio " << _d.r << " target " << _d.t << " diff " << _d.d;
}
#endif

static void applyRegionBoundaries(const int imgW, const int imgH, parallel::RegionSpec &regionSpec) {
    // Region dimensions (can simplify to integer division imgW / coresH, but this is clearer)
    int rgW = static_cast<int>(std::floor(static_cast<float>(imgW) / regionSpec.coresH()));
    int rgH = static_cast<int>(std::floor(static_cast<float>(imgH) / regionSpec.coresV()));

    for (int j = 0; j < regionSpec.coresV(); ++j) {
        for (int i = 0; i < regionSpec.coresH(); ++i) {
            // Region position
            int rgX = i * rgW;
            int rgY = j * rgH;

            // Actual region dimensions, last of row/column must fit the complete image size
            int argW = (i + 1) % regionSpec.coresH() != 0 ? rgW : imgW - rgX;
            int argH = (j + 1) % regionSpec.coresV() != 0 ? rgH : imgH - rgY;

            regionSpec.data()[j * regionSpec.coresH() + i] = {rgX, rgY, argW, argH};
        }
    }
}

parallel::RegionSpec parallel::calculateRegions(const int imgW, const int imgH, const int cores) {
    float ratio = static_cast<float>(imgW) / static_cast<float>(imgH);

    // The algorithm is ran once by assuming that image width > height,
    // if that is not the case, flip the ratio and then swap the results at the end.
    bool invRatio = ratio < 1;
    if (invRatio) ratio = 1 / ratio;

    // Start by the "extreme" case ([#cores] regions width * 1 height)
    float w = static_cast<float>(cores);
    float h = 1;
    float diff = std::abs(std::floor(w) / h - ratio);

    // Continue float-dividing width by 2 and mul. height by 2
    // stopping when the ratio best approaches the one of the image.
    while (w > 1) {
        float nw = w / 2;
        float nh = 2 * h;
        float ndiff = std::abs(std::floor(nw) / nh - ratio);
#ifdef MORPH_DEBUG_PLANNER
        std::cerr << "[Region] Candidate: " << dbgRegionStep(nw, nh, std::floor(nw) / nh, ratio, ndiff) << std::endl;
#endif

        if (ndiff > diff) break;

        w = nw;
        h = nh;
        diff = ndiff;
    }

    // Ensure we have a round number of cores
    w = std::floor(w);

    diff = std::abs(w / h - ratio);
#ifdef MORPH_DEBUG_PLANNER
    std::cerr << "[Region] Result: " << dbgRegionStep(w, h, w / h, ratio, diff) << std::endl;
#endif

    // Swap for the case height > width as we said
    if (invRatio) std::swap(w, h);

    RegionSpec baseRegions{cores, static_cast<int>(w), static_cast<int>(h)};
#ifdef MORPH_DEBUG_PLANNER
    std::cerr << "[Region] Excess cores: " << baseRegions.excessCores() << std::endl;
#endif
    applyRegionBoundaries(imgW, imgH, baseRegions);
    return baseRegions;
}
