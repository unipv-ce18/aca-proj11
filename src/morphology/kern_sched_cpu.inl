// ----- AVX2 definitions -----
#if defined(K_SIMD_TYPE_AVX2)

#define SIMD_WIDTH 32
typedef __m256i simdi_t;

// 8-bit unsigned arithmetic, saturated
#define simd_adds_epu8  _mm256_adds_epu8
#define simd_subs_epu8  _mm256_subs_epu8

// 8-bit unsigned comparison
#define simd_max_epu8   _mm256_max_epu8
#define simd_min_epu8   _mm256_min_epu8

// 8-bit set1 broadcast
#define simd_set1_epi8  _mm256_set1_epi8

// 128 bit data load/store
#define simd_loadu      _mm256_loadu_si256
#define simd_storeu     _mm256_storeu_si256


// ----- SSE2 definitions -----
#elif defined(K_SIMD_TYPE_SSE2)

#define SIMD_WIDTH 16
typedef __m128i simdi_t;

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
#define simd_storeu     _mm_storeu_si128


// ----- No SIMD support -----
#else

// Ensure SIMD is not enabled
#undef SIMD_WIDTH

#endif



// ----- The algorithm itself -----

// Outer pixel loop
#ifdef SIMD_WIDTH
assert(ch.rect.w == SIMD_WIDTH);

const int x = ch.rect.x;    // SIMD_WIDTH Xes processed in parallel, starting at ch.rect.x (constant)
for (int y = ch.rect.y; y < ch.rect.y + ch.rect.h; ++y) {
#else
for (int y = ch.rect.y; y < ch.rect.y + ch.rect.h; ++y) {
for (int x = ch.rect.x; x < ch.rect.x + ch.rect.w; ++x) {
#endif

    // Initialize accumulator
#ifdef SIMD_WIDTH
    simdi_t val = simd_set1_epi8(0);
#else
    int val = 0;
#endif

    // Inner structural element pixel loop
    for (int j = strEl.yMin(); j <= strEl.yMax(); ++j) {
        for (int i = strEl.xMin(); i <= strEl.xMax(); ++i) {
            if (!strEl.isSet(j, i)) continue;

            int u = x + i;
            int v = y + j;

#ifdef K_ENABLE_BORDER_CHECKS
#ifndef SIMD_WIDTH
            if (v < 0 || v >= src.size().height) continue;
            if (u < 0 || u >= src.size().width) continue;
#else
            #error SIMD variant only works in safe areas
#endif
#endif

            // Actual sum/subtraction and max/min operation
#if defined(K_METHOD_DILATE)
#ifndef SIMD_WIDTH
            int m = src.at<uint8_t>(v, u) + strEl.at(j, i);
            if (m > val) val = m;
#else
            simdi_t m = simd_adds_epu8(
                simd_loadu(reinterpret_cast<const simdi_t *>(src.ptr(v) + u)),
                simd_set1_epi8(strEl.at(j, i)));
            val = simd_max_epu8(m, val);
#endif

#elif defined(K_METHOD_ERODE)
#ifndef SIMD_WIDTH
            int m = src.at<uint8_t>(v, u) - strEl.at(j, i);
            if (m < val) val = m;
#else
            simdi_t m = simd_subs_epu8(
                simd_loadu(reinterpret_cast<const simdi_t *>(src.ptr(v) + u)),
                simd_set1_epi8(strEl.at(j, i)));
            val = simd_min_epu8(m, val);
#endif

#else
#error Unsupported operation method
#endif

        }
    }

    // Store result
#ifndef SIMD_WIDTH
    out.at<uint8_t>(y, x) = static_cast<uint8_t>(
#if defined(K_METHOD_DILATE)
        val < 0xFF ? val : 0xFF
#elif defined(K_METHOD_ERODE)
        val > 0x00 ? val : 0x00
#endif
);
#else
    simd_storeu(reinterpret_cast<simdi_t *>(out.ptr(y) + x), val);
#endif

}
#ifndef SIMD_WIDTH
}
#endif


#undef K_METHOD_DILATE
#undef K_METHOD_ERODE
#undef K_SIMD_TYPE_SSE2
#undef K_SIMD_TYPE_AVX2
#undef K_ENABLE_BORDER_CHECKS
