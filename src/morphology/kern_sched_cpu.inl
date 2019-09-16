#if defined(K_ENABLE_BORDER_CHECKS) && defined(SIMD_WIDTH)
#error SIMD variant only works in safe areas
#endif

// Argument mapping
#if defined(K_METHOD_DILATE) || defined(K_METHOD_ERODE)
cv::Mat &out = args.out;
cv::Mat const &src = args.src;
StrEl &strEl = args.strEl;

#elif defined(K_METHOD_SKELPART)
// SkelIter algorithm implies dilation(nxI, strEl) -> skel; skel is then processed further
#define K_METHOD_DILATE
cv::Mat &out = args.skel;
cv::Mat const &src = args.nxImg;
StrEl &strEl = args.strEl;

cv::Mat const &img = args.img;

#endif

int seYa, seYb, seXa, seXb;

// Outer pixel loop
#ifdef SIMD_WIDTH
#ifndef NDEBUG
assert(ch.rect.w == SIMD_WIDTH);
#endif

const int x = ch.rect.x;    // SIMD_WIDTH Xes processed in parallel, starting at ch.rect.x (constant)
#include "kern_sched_cpu_sebound_x.inl"
for (int y = ch.rect.y; y < ch.rect.y + ch.rect.h; ++y) {
#include "kern_sched_cpu_sebound_y.inl"

#else
for (int y = ch.rect.y; y < ch.rect.y + ch.rect.h; ++y) {
#include "kern_sched_cpu_sebound_y.inl"
for (int x = ch.rect.x; x < ch.rect.x + ch.rect.w; ++x) {
#include "kern_sched_cpu_sebound_x.inl"
#endif

// Initialize accumulator
#if defined(K_METHOD_DILATE)
#define __INIT_VAL 0
#elif defined(K_METHOD_ERODE)
#define __INIT_VAL 255
#endif
#ifndef SIMD_WIDTH
    int val = __INIT_VAL;
#else
    simdi_t val = simd_set1_epi8(__INIT_VAL);
#endif
#undef __INIT_VAL

    // Inner structural element pixel loop
    for (int j = seYa; j <= seYb; ++j) {
        for (int i = seXa; i <= seXb; ++i) {
            if (!strEl.isSet(j, i)) continue;

            int u = x + i;
            int v = y + j;

            // Actual sum/subtraction and max/min operation
#if defined(K_METHOD_DILATE)
#ifndef SIMD_WIDTH
            int m = src.at<uint8_t>(v, u) + strEl.at(i, j);
            if (m > val) val = m;
#else
            simdi_t m = simd_adds_epu8(
                simd_loadu(reinterpret_cast<const simdi_t *>(src.ptr(v) + u)),
                simd_load(reinterpret_cast<const simdi_t *>(&strEl.at(i, j))));
            val = simd_max_epu8(m, val);
#endif

#elif defined(K_METHOD_ERODE)
#ifndef SIMD_WIDTH
            int m = src.at<uint8_t>(v, u) - strEl.at(i, j);
            if (m < val) val = m;
#else
            simdi_t m = simd_subs_epu8(
                simd_loadu(reinterpret_cast<const simdi_t *>(src.ptr(v) + u)),
                simd_loadu(reinterpret_cast<const simdi_t *>(&strEl.at(i, j))));
            val = simd_min_epu8(m, val);
#endif

#else
#error Unsupported operation method
#endif

        }
    }

    // If this operator is SkelPart, perform additional steps before storing
#ifdef K_METHOD_SKELPART
#ifndef SIMD_WIDTH
    // "out" is actually "skel" to update
    uint8_t skelPx = out.at<uint8_t>(y, x);
    int imgDiff = img.at<uint8_t>(y, x) - val;
    val = skelPx > imgDiff ? skelPx : (imgDiff < 0 ? 0 : imgDiff);  // max
#else
    val = simd_max_epu8(
            simd_loadu(reinterpret_cast<const simdi_t *>(out.ptr(y) + x)),
            simd_subs_epu8(
                simd_loadu(reinterpret_cast<const simdi_t *>(img.ptr(y) + x)),
                val
            )
    );
#endif
#endif


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
    simd_store(reinterpret_cast<simdi_t *>(out.ptr(y) + x), val);
#endif

}
#ifndef SIMD_WIDTH
}
#endif


#undef K_METHOD_DILATE
#undef K_METHOD_ERODE
#undef K_METHOD_SKELPART
#undef K_ENABLE_BORDER_CHECKS
#undef K_ENABLE_BORDER_CHECKS_VERTICAL
