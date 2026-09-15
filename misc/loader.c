#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define NOSERVICE
  #define NOMCX
  #define NOIME
  #include <windows.h>

  #define LoadSymbol(name) (void*)GetProcAddress(GetModuleHandleA(NULL), name)
#else
  #define _GNU_SOURCE
  #include <dlfcn.h>

  #define LoadSymbol(name) dlsym(RTLD_DEFAULT, name)
#endif

#include "dynamic.c"

static int loaded = 0;
void LoadSymbols(void) {
  if (loaded) { return; }
  loaded = 1;
  
  char **name = names;
  void ***addr = addrs;
  while (*name) { *(*addr++) = LoadSymbol(*name++); }
}

