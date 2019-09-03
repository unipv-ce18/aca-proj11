#include <utility>

#ifndef MORPH_PLAN_H
#define MORPH_PLAN_H

#include "RegionSpec.h"
#include "CoreAllocation.h"

namespace parallel {

    class Plan {

        int imageWidth_;
        int imageHeight_;
        int cores_;
        int sap_;
        RegionSpec baseRegions_;
        CoreAllocation effectiveRegions_;

    public:

        Plan(int imageWidth, int imageHeight, int cores, int sap,
             RegionSpec &&baseRegions, CoreAllocation &&effectiveRegions) :
            imageWidth_(imageWidth),
            imageHeight_(imageHeight),
            cores_(cores),
            sap_(sap),
            baseRegions_(std::move(baseRegions)),
            effectiveRegions_(std::move(effectiveRegions)) {}

        /// The width of the image this plan schedules for
        int imageWidth() const {
            return imageWidth_;
        }

        /// The height of the image this plan schedules for
        int imageHeight() const {
            return imageHeight_;
        }

        /// The number of cores intended to be used for executing this plan
        int cores() const {
            return cores_;
        }

        /// The Safe Area Padding used for generating this plan
        int sap() const {
            return sap_;
        }

        /// Even allocation region data for non-SIMD execution
        RegionSpec &baseRegions() {
            return baseRegions_;
        }

        /// The effective chunk allocation for each core
        CoreAllocation &effectiveRegions() {
            return effectiveRegions_;
        }

    };

}

#endif //MORPH_PLAN_H
