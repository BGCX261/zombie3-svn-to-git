#if defined(WIN32)

#if defined(COMMON_EXPORTS)
/* DLL export */
#ifdef EXPORT_COMMON
#undef EXPORT_COMMON
#endif
#define EXPORT_COMMON __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_COMMON __declspec(dllimport)
//#define EXPORT
#endif
#else
#define EXPORT_COMMON 
#endif