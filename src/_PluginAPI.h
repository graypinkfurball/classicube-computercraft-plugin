#ifndef _PLUGIN_API_H
#define _PLUGIN_API_H
#include "src/PluginAPI.h"

#ifdef HAS_DYNAMIC_H
  #undef CC_VAR
  #undef CC_API
  #define CC_VAR
  #define CC_API extern

  #include "dynamic.h"

  void LoadSymbols(void);
#else
  #define LoadSymbols()
#endif

#endif

