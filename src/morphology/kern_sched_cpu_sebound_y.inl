#if defined(K_ENABLE_BORDER_CHECKS)
if (ch.type == CHUNK_NW || ch.type == CHUNK_NE) {
    seYa = -y; seYb = strEl.yMax();
} else if (ch.type == CHUNK_SW || ch.type == CHUNK_SE) {
    seYa = strEl.yMin(); seYb = src.size().height - y - 1;
} else {
#elif defined(K_ENABLE_BORDER_CHECKS_VERTICAL)
if (ch.type == CHUNK_N) {
    seYa = -y; seYb = strEl.yMax();
} else if (ch.type == CHUNK_S) {
    seYa = strEl.yMin(); seYb = src.size().height - y - 1;
} else {
#endif
    seYa = strEl.yMin(); seYb = strEl.yMax();
#if defined(K_ENABLE_BORDER_CHECKS) || defined(K_ENABLE_BORDER_CHECKS_VERTICAL)
}
#endif
