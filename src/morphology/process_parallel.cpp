#include "process_parallel.h"

#include "morphology/kernels.h"
#include "parallel/Plan.h"
#include "simd_props.h"

#include <omp.h>

template<typename Operator, typename OpArgs>
void processParallel(parallel::Plan &plan, OpArgs params, const bool noSimd) {
    const auto &alloc = plan.effectiveRegions().allocation();

    using namespace morph::kern;

#pragma omp parallel default(none) shared(alloc, params, noSimd) num_threads(alloc.size())
    {
        int core = omp_get_thread_num();

        for (const auto &ch : alloc[core]) {
            switch (ch.type) {
                case CHUNK_REGULAR:
                    // Use regular kernels here

                    if (noSimd) {
                        _kernel_single<Operator>(params, ch);
                        continue;
                    }

                    switch (ch.rect.w) {
#ifdef MORPH_ENABLE_SIMD_AVX512F
                        case SIMD_WIDTH_AVX512F:
                            _kernel_avx512f<Operator>(params, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                        case SIMD_WIDTH_AVX2:
                            _kernel_avx2<Operator>(params, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                        case SIMD_WIDTH_SSE2:
                            _kernel_sse2<Operator>(params, ch);
                            break;
#endif
                        default:
                            _kernel_single<Operator>(params, ch);
                    }
                    break;

                case CHUNK_N:
                case CHUNK_S:
                    // Use "safev" kernels here

                    if (noSimd) {
                        _kernel_single_safev<Operator>(params, ch);
                        continue;
                    }

                    switch (ch.rect.w) {
#ifdef MORPH_ENABLE_SIMD_AVX512F
                        case SIMD_WIDTH_AVX512F:
                            _kernel_avx512f_safev<Operator>(params, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_AVX2
                        case SIMD_WIDTH_AVX2:
                            _kernel_avx2_safev<Operator>(params, ch);
                            break;
#endif
#ifdef MORPH_ENABLE_SIMD_SSE2
                        case SIMD_WIDTH_SSE2:
                            _kernel_sse2_safev<Operator>(params, ch);
                            break;
#endif
                        default:
                            _kernel_single_safev<Operator>(params, ch);
                    }
                    break;

                default:
                    // Use "safe" kernels here (no SIMD available)
                    _kernel_single_safe<Operator>(params, ch);
                    break;
            }
        }
    }
}

/* Explicitly instantiate the template to keep things clean linker-wise
 * and avoid including it in each compilation unit that requires it.
 *
 * Otherwise we would need to #include "process_parallel.tcc" in our header and remove this from CMake
 */
template void processParallel<Dilate>(parallel::Plan &plan, BaseOpParams params, const bool noSimd);
template void processParallel<Erode>(parallel::Plan &plan, BaseOpParams params, const bool noSimd);
template void processParallel<SkelIter>(parallel::Plan &plan, SkelIterParams params, const bool noSimd);
