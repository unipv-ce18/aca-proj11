#define SIMD_WIDTH 16   // 128b
#define simdi_t    __m128i

// 8-bit unsigned arithmetic, saturated
#define simd_adds_epu8  _mm_adds_epu8
#define simd_subs_epu8  _mm_subs_epu8

// 8-bit unsigned comparison
#define simd_max_epu8   _mm_max_epu8
#define simd_min_epu8   _mm_min_epu8

// 8-bit set1 broadcast
#define simd_set1_epi8  _mm_set1_epi8

// 128 bit data load/store
#define simd_loadu      _mm_loadu_si128
// _mm_load_si128       (must be 16B aligned)
// _mm_lddqu_si128      (may perf. better when crossing cache line boundary)
#define simd_storeu     _mm_storeu_si128
// _mm_store_si128      (must be 16B aligned)
// _mm_stream_si128     (non temporal, must aligned)
// _mm_maskmoveu_si128  (mask store, non temporal, non align)
