#pragma once

#ifdef _USRDLL
#define CC_DLL _declspec(dllexport)
#else
#define CC_DLL _declspec(dllimport)
#endif


