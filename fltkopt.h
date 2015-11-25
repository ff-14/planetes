#ifndef _FLTK_LINKER_OPTIONS_
#define _FLTK_LINKER_OPTIONS_

#ifdef _DEBUG
	#pragma comment (linker, "/nodefaultlib:\"libcmtd\" ")
#elif defined(NDEBUG)
	#pragma comment (linker, "/nodefaultlib:\"libcmt\" ")
#endif

#pragma comment (lib, "./lib/fltk.lib")
#pragma comment (lib, "./lib/fltkforms.lib")
#pragma comment (lib, "./lib/fltkgl.lib")
#pragma comment (lib, "./lib/fltkimages.lib")
#pragma comment (lib, "./lib/fltkjpeg.lib")
#pragma comment (lib, "./lib/fltkpng.lib")
#pragma comment (lib, "./lib/fltkz.lib")

#pragma comment (lib, "wsock32.lib")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

#endif //_FLTK_LINKER_OPTIONS_


