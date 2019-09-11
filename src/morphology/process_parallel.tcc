#include "process_parallel.h"

#include "morphology/kernels.h"
#include "morphology/StrEl.h"
#include "parallel/Plan.h"
#include "simd_props.h"

#include <omp.h>

template<typename Operator, typename OpArgs>
void processParallel(parallel::Plan &plan, OpArgs params, const bool noSimd) {
    const auto &alloc = plan.effectiveRegions().allocation();

    using namespace morph::kern;

#pragma omp parallel default(none) shared(alloc, params) num_threads(alloc.size())
    {
        int core = omp_get_thread_num();

        for (const auto &ch : alloc[core]) {
            switch (ch.type) {
                case CHUNK_REGULAR:
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
                default:
                    _kernel_safe<Operator>(params, ch);
                    break;
            }
        }
    }
}
