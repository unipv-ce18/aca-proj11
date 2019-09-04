#include "planners.h"

#include "Plan.h"
#include "even_region.h"

#include <cmath>
#include <memory>
#include <iostream>

#define SIMD_WIDTH 64
#define MAX_CHUNK_HEIGHT 32

static std::vector<int> calcColumnSplits(int imgW, int sap) {
    int safeWidth = imgW - 2 * sap;
    int lastChunkWidth = safeWidth % SIMD_WIDTH;

    // (could simplify to integer division)
    int numChunks = 2 + (lastChunkWidth == 0 ? 0 : 1) +
                    static_cast<int>(std::floor(static_cast<float>(safeWidth) / SIMD_WIDTH));

    std::vector<int> hSteps(static_cast<size_t>(numChunks));

    for (int i = 0; i < numChunks; ++i) {
        if (i == 0)  // First "unsafe area" chunk
            hSteps[i] = 0;
        else if (i == numChunks - 1)  // Last "unsafe area" chunk
            hSteps[i] = imgW - sap;
        else
            hSteps[i] = sap + (i - 1) * SIMD_WIDTH;
    }

    return hSteps;
}

static int computeAdditionalRowCount(const parallel::RegionSpec &rs, int lineAllocCores) {
    int lineExcessCores = rs.coresH() - lineAllocCores;
    std::fprintf(stderr, "Chunk line excess cores: %d\n", lineExcessCores);

    int totalExcessCores = rs.excessCores() + lineExcessCores * rs.coresV();
    std::fprintf(stderr, "Total excess cores: %d + %d * %d = %d\n",
                 rs.excessCores(), lineExcessCores, rs.coresV(), totalExcessCores);

    int additionalRows = static_cast<int>(std::floor(static_cast<float>(totalExcessCores) / lineAllocCores));
    if (additionalRows > 0)
        std::fprintf(stderr, "Planning additional %d region rows to minimize excess cores (%d -> %d)\n",
                     additionalRows, totalExcessCores, totalExcessCores - additionalRows * lineAllocCores);
    else
        std::fprintf(stderr, "Not enough unused cores to schedule for new rows\n");

    return additionalRows;
}

static std::vector<int> calcNewVRegions(int imgH, int sap, int totalRows,
                                        int &rowHeight, int &excludedRowsTop, int &excludedRowsBottom) {
    excludedRowsTop = 0;
    excludedRowsBottom = 0;
    rowHeight = static_cast<int>(std::floor(imgH) / totalRows);
    std::vector<int> newVRegions(static_cast<size_t>(totalRows));

    for (int i = 0; i < totalRows; ++i) {
        int split = i * rowHeight;
        newVRegions[i] = split;

        if (sap >= split && sap < split + rowHeight)
            excludedRowsTop = i;
        if (imgH - sap > split && imgH - sap <= split + rowHeight)
            excludedRowsBottom = totalRows - i - 1;
    }

    return newVRegions;
}

static std::vector<int> calcRowSplits(int imgH, int sap, int totalRows, int &totalSafeRows,
                                      int &chunksVFirst, int &chunksV, int &chunksVLast) {
    // Evenly split the image vertically using our new totalRows count,
    // this is the same as the split from calculateRegions() if additional row count is 0.
    int rowHeight, excludedRowsTop, excludedRowsBottom;
    std::vector<int> vRegions = calcNewVRegions(imgH, sap, totalRows, rowHeight, excludedRowsTop, excludedRowsBottom);

    totalSafeRows = totalRows - excludedRowsTop - excludedRowsBottom;

    // # of chunks for each (region) row - define here, used in allocation algorithm later
    // chunksV, chunksVFirst, chunksVLast;

    if (totalSafeRows > 1) {
        // Plan for at least 2 rows

        int lastRowHeight = imgH - vRegions[vRegions.size() - 1];

        int exclPxTop = excludedRowsTop * rowHeight;
        int exclPxBottom = excludedRowsBottom < 2
                           ? (excludedRowsBottom * lastRowHeight)
                           : (lastRowHeight + (excludedRowsBottom - 1) * rowHeight);

        int sapInRegionTop = sap - exclPxTop;
        int sapInRegionBottom = sap - exclPxBottom;

        chunksV = static_cast<int>(std::ceil(static_cast<float>(rowHeight) / MAX_CHUNK_HEIGHT));
        chunksVFirst = 1 + static_cast<int>(std::ceil(
            static_cast<float>(rowHeight - sapInRegionTop) /
            MAX_CHUNK_HEIGHT));
        chunksVLast = 1 + static_cast<int>(std::ceil(
            static_cast<float>((excludedRowsBottom == 0 ? lastRowHeight : rowHeight) - sapInRegionBottom) /
            MAX_CHUNK_HEIGHT));

        std::vector<int> vSteps(static_cast<size_t>(chunksVFirst + chunksV * (totalSafeRows - 2) + chunksVLast));

        // --- 1st row
        vSteps[0] = 0;
        vSteps[1] = sap;
        for (int i = 2; i < chunksVFirst; ++i)
            vSteps[i] = vSteps[i - 1] + MAX_CHUNK_HEIGHT;
        // --- middle rows
        int startIdx = chunksVFirst;
        for (int r = excludedRowsTop + 1; r < totalRows - excludedRowsBottom - 1; ++r) {
            vSteps[startIdx] = vRegions[r];
            for (int i = startIdx + 1; i < startIdx + chunksV; ++i) {
                vSteps[i] = vSteps[i - 1] + MAX_CHUNK_HEIGHT;
            }
            startIdx += chunksV;
        }
        // --- final row
        vSteps[startIdx] = vRegions[totalRows - excludedRowsBottom - 1];
        for (int i = startIdx + 1; i < startIdx + chunksVLast - 1; ++i) {
            vSteps[i] = vSteps[i - 1] + MAX_CHUNK_HEIGHT;
        }
        vSteps[startIdx + chunksVLast - 1] = imgH - sap;

        return vSteps;

    } else {
        // Single row
        chunksV = 2 + static_cast<int>(std::ceil(static_cast<float>(imgH - 2 * sap) / MAX_CHUNK_HEIGHT));

        std::vector<int> vSteps(static_cast<size_t>(chunksV));

        vSteps[0] = 0;
        vSteps[1] = sap;
        for (int i = 2; i < chunksV - 1; ++i)
            vSteps[i] = vSteps[i - 1] + MAX_CHUNK_HEIGHT;
        vSteps[chunksV - 1] = imgH - sap;

        return vSteps;
    }
}

static parallel::CoreAllocation assignCores(const std::vector<int> &hSteps, const std::vector<int> &vSteps,
                                            int imgW, int imgH, int colsPerCore, int coresH, int coresV,
                                            int chunksVFirst, int chunksV, int chunksVLast, int totalCores, int &totalUsedCores) {
    totalUsedCores = coresH * coresV;
    std::vector<std::vector<parallel::Chunk>> allocation(static_cast<size_t>(totalUsedCores));

    int numChunks = static_cast<int>(hSteps.size());

    for (int core = 0; core < totalUsedCores; ++core) {
        int rgnX = core % coresH;
        int rgnY = static_cast<int>(std::floor(static_cast<float>(core) / coresH));

        int chX = rgnX * colsPerCore;
        int chW = std::min(colsPerCore, numChunks - chX);

        int chY = rgnY > 0 ? chunksVFirst + (rgnY - 1) * chunksV : 0;
        int chH =
            (coresV > 1 && rgnY == 0) ? chunksVFirst :
            (coresV > 1 && rgnY == coresV - 1) ? chunksVLast :
            chunksV;

        std::vector<parallel::Chunk> coreChunks(static_cast<size_t>(chW * chH));

        for (int j = chY; j < chY + chH; ++j) {
            for (int i = chX; i < chX + chW; ++i) {
                int type =
                    (i == 0 && j == 0) ? CHUNK_NW :
                    (i == numChunks - 1 && j == 0) ? CHUNK_NE :
                    (i == numChunks - 1 && j == vSteps.size() - 1) ? CHUNK_SE :
                    (i == 0 && j == vSteps.size() - 1) ? CHUNK_SW :
                    (i == 0) ? CHUNK_W :
                    (i == numChunks - 1) ? CHUNK_E :
                    (j == 0) ? CHUNK_N :
                    (j == vSteps.size() - 1) ? CHUNK_S :
                    CHUNK_REGULAR;
                coreChunks[(j - chY) * chW + i - chX] = {type, {
                    /* x */ hSteps[i],
                    /* y */ vSteps[j],
                    /* w */ (i + 1 < hSteps.size() ? hSteps[i + 1] : imgW) - hSteps[i],
                    /* h */ (j + 1 < vSteps.size() ? vSteps[j + 1] : imgH) - vSteps[j]
                }};
            }
        }

        // TODO: Can we be more efficient on this assignment op?
        allocation[core] = std::move(coreChunks);
    }

    return {hSteps, vSteps, coresH, coresV, totalCores, std::move(allocation)};
}

parallel::Plan parallel::planSimdExecution(int cores, int imgW, int imgH, int sap) {
    RegionSpec rs = calculateRegions(imgW, imgH, cores);

    std::vector<int> hSteps = calcColumnSplits(imgW, sap);
    int numChunks = static_cast<int>(hSteps.size());

    // # of columns processed by a single core
    int colsPerCore = static_cast<int>(std::ceil(static_cast<float>(numChunks) / rs.coresH()));

    // # of cores actually used on a line
    int lineAllocCores = static_cast<int>(std::ceil(static_cast<float>(numChunks) / colsPerCore));

    // Recalculate region row count to optimize core allocation
    int totalRows = rs.coresV() + computeAdditionalRowCount(rs, lineAllocCores);

    // The effective vertical region count for vSteps considering very large safe areas
    int totalSafeRows;

    int chunksVFirst, chunksV, chunksVLast;
    std::vector<int> vSteps = calcRowSplits(imgH, sap, totalRows, totalSafeRows, chunksVFirst, chunksV, chunksVLast);

    // Assign regions to cores (horizontal first)
    int totalUsedCores;
    CoreAllocation ca = assignCores(hSteps, vSteps, imgW, imgH, colsPerCore, lineAllocCores, totalSafeRows,
                                    chunksVFirst, chunksV, chunksVLast, cores, totalUsedCores);

    if (totalRows != totalSafeRows) {
        std::fprintf(stderr, "Effectively used %d cores out of %d: SAP size exceeds region\n",
                     totalUsedCores, lineAllocCores * totalRows);
    }

    return {imgW, imgH, cores, sap, std::move(rs), std::move(ca)};
}
