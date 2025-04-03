// config.hpp
#pragma once

#ifdef __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_OS_IOS
    #define USING_SDL2
  #else
    #define USING_SDL3
  #endif
#else
  #define USING_SDL3
#endif