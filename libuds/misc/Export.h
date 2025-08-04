#pragma once

//±àÒëÎª¶¯Ì¬Á´½Ó¿â
#ifdef _DLL
	#ifdef UDS_BUILD_DLL
		#define UDS_DLL_EXPORT __declspec(dllexport)
		#pragma warning(disable:4251)
	#else
		#define UDS_DLL_EXPORT __declspec(dllimport)
	#endif // UDS_BUILD_DLL
#else
	#ifdef _LIB
		#define UDS_DLL_EXPORT
	#else
		#define UDS_DLL_EXPORT
	#endif // _LIB
#endif // _DLL

