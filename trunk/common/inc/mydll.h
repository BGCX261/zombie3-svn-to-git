#ifndef _MYDLL_H_
#define _MYDLL_H_


#if defined(WIN32)
#ifdef EXPORT
#undef EXPORT
#endif
#if defined(BUILD_DLL) || defined(_USRDLL)
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
//#define EXPORT

#endif

#else

#define EXPORT 

#endif


#endif /* _MYDLL_H_ */
