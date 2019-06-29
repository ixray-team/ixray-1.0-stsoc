#pragma once

#ifndef XRGAMESPY_API
	#ifdef XRGAMESPY_EXPORTS
		#define XRGAMESPY_API __declspec(dllexport)
	#endif
#endif


#define EXPORT_FN_DECL(r, f, p)		XRGAMESPY_API r xrGS_##f p;