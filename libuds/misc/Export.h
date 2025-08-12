#pragma once

//±àÒëÎª¶¯Ì¬Á´½Ó¿â
#ifdef LIBUDS_NOT_EXPORTS
#define LIBUDS_DLL_EXPORT
#else
#ifdef LIBUDS_EXPORTS
#define LIBUDS_DLL_EXPORT __declspec(dllexport)
#pragma warning(disable:4251)
#else
#define LIBUDS_DLL_EXPORT __declspec(dllimport)
#endif // LIBUDS_EXPORTS
#endif // LIBUDS_NOT_EXPORTS

