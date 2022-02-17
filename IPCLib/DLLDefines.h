#pragma once

#ifdef _WIN32
	//#ifdef shared_EXPORTS
		#define SHARED_EXPORT __declspec(dllexport)
	//#else
	//	#define SHARED_EXPORT __declspec(dllimport)
	//#endif
#else
#define SHARED_EXPORT
#endif