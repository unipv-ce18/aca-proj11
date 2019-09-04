for (int y = ch.rect.y; y < ch.rect.y + ch.rect.h; ++y) {
    for (int x = ch.rect.x; x < ch.rect.x + ch.rect.w; ++x) {
        int val=0;

        for (int j = strEl.yMin(); j <= strEl.yMax(); ++j) {
            for (int i = strEl.xMin(); i <= strEl.xMax(); ++i) {
                if (!strEl.isSet(j, i)) continue;

                int u = x + i;
                int v = y + j;

#ifdef ENABLE_BORDER_CHECKS
                if (v < 0 || v >= src.size().height) continue;
                if (u < 0 || u >= src.size().width) continue;
#endif

                int m = src.at<uint8_t>(v, u) + strEl.at(j, i);
                if (m > val) val = m;
            }
        }
        out.at<uint8_t>(y, x) = static_cast<uint8_t>(val < 0xFF ? val : 0xFF);
    }
}
