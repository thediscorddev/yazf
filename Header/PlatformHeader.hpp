#ifndef PLATFORMHEADER_HPP
#define PLATFORMHEADER_HPP
#define YAZF_VERSION "1.0 beta"
#define ENCRYPTIONKEYSIZE 512
#define HEADERSIZE 8192
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