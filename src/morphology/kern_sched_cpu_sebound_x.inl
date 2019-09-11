#if defined(K_ENABLE_BORDER_CHECKS)
if (ch.type == CHUNK_W || ch.type == CHUNK_NW || ch.type == CHUNK_SW) {
    seXa = -x; seXb = strEl.xMax();
} else if (ch.type == CHUNK_E || ch.type == CHUNK_NE || ch.type == CHUNK_SE) {
    seXa = strEl.xMin(); seXb = src.size().width - x - 1;
} else {
#endif
    seXa = strEl.xMin(); seXb = strEl.xMax();
#if defined(K_ENABLE_BORDER_CHECKS)
}
#endif
