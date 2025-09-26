#ifndef PLATFORMHEADER_HPP
#define PLATFORMHEADER_HPP
#ifdef _WIN32
  #ifdef MYLIB_EXPORTS
    #define YAZF_API __declspec(dllexport)
  #else
    #define YAZF_API __declspec(dllimport)
  #endif
#else
  #define YAZF_API
#endif
#endif