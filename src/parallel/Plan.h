#include <utility>

#ifndef MORPH_PLAN_H
#define MORPH_PLAN_H

#include "RegionSpec.h"

namespace parallel {

    class Plan {

        int imageWidth_;
        int imageHeight_;
        int cores_;
        int sap_;
        RegionSpec baseRegions_;
        // effectiveRegions

    public:

        Plan(int imageWidth_, int imageHeight_, int cores_, int sap_, RegionSpec &&baseRegions_) :
            imageWidth_(imageWidth_),
            imageHeight_(imageHeight_),
            cores_(cores_),
            sap_(sap_),
            baseRegions_(std::move(baseRegions_)) {}

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

    };

}

#endif //MORPH_PLAN_H
