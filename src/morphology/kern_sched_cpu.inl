// Argument mapping
#if defined(K_METHOD_DILATE) || defined(K_METHOD_ERODE)
cv::Mat &out = args.out;
cv::Mat const &src = args.src;
StrEl const &strEl = args.strEl;

#elif defined(K_METHOD_SKELPART)
// SkelIter algorithm implies dilation(nxI, strEl) -> skel; skel is then processed further
#define K_METHOD_DILATE
cv::Mat &out = args.skel;
cv::Mat const &src = args.nxImg;
StrEl const &strEl = args.strEl;

cv::Mat const &img = args.img;

#endif



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
    simd_storeu(reinterpret_cast<simdi_t *>(out.ptr(y) + x), val);
#endif

}
#ifndef SIMD_WIDTH
}
#endif


#undef K_METHOD_DILATE
#undef K_METHOD_ERODE
#undef K_METHOD_SKELPART
#undef K_ENABLE_BORDER_CHECKS
