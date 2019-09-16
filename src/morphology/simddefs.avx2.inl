#define SIMD_WIDTH 32   // 256b
#define simdi_t    __m256i

// 8-bit unsigned arithmetic, saturated
#define simd_adds_epu8  _mm256_adds_epu8
#define simd_subs_epu8  _mm256_subs_epu8

// 8-bit unsigned comparison
#define simd_max_epu8   _mm256_max_epu8
#define simd_min_epu8   _mm256_min_epu8

// 8-bit set1 broadcast
#define simd_set1_epi8  _mm256_set1_epi8

// 256 bit data load/store
#define simd_loadu      _mm256_loadu_si256
#ifdef MORPH_ALIGN_IMAGES
#define simd_load       _mm256_load_si256   // (must be 32B aligned)
#else
#define simd_load       simd_loadu
#endif
// _mm256_lddqu_si256   (may perf. better when crossing cache line boundary)
#define simd_storeu     _mm256_storeu_si256
#ifdef MORPH_ALIGN_IMAGES
#define simd_store      _mm256_store_si256  // (must be 32B aligned)
#else
#define simd_store      simd_storeu
#endif
// _mm256_stream_si256  (non temporal, must aligned)
