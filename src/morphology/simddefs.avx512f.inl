#define SIMD_WIDTH 64   // 512b
#define simdi_t    __m512i

// 8-bit unsigned arithmetic, saturated
#define simd_adds_epu8  _mm512_adds_epu8
#define simd_subs_epu8  _mm512_subs_epu8

// 8-bit unsigned comparison
#define simd_max_epu8   _mm512_max_epu8
#define simd_min_epu8   _mm512_min_epu8

// 8-bit set1 broadcast
#define simd_set1_epi8  _mm512_set1_epi8

// 512 bit data load/store
#define simd_loadu      _mm512_loadu_si512
#ifdef MORPH_ALIGN_IMAGES
#define simd_load       _mm512_load_si512   // (must be 64B aligned)
#else
#define simd_load       simd_loadu
#endif
#define simd_storeu     _mm512_storeu_si512
#ifdef MORPH_ALIGN_IMAGES
#define simd_store      _mm512_store_si512  // (must be 64B aligned)
#else
#define simd_store      simd_storeu
#endif
// _mm512_stream_si512  (non temporal, must aligned)
