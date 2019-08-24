#ifndef MORPH_MODES_H
#define MORPH_MODES_H

using ModeProc = void(*)(int, char*[]);

ModeProc getRunMode(const char *modeStr);

void printUsage(const char *execStr);

#endif //MORPH_MODES_H
