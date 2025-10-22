#pragma once

#ifdef _WIN32 // windows sucks
    #ifndef _WINSOCKAPI_
        #define _WINSOCKAPI_
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>