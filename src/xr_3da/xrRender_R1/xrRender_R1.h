// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRRENDER_R1_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRRENDER_R1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRRENDER_R1_EXPORTS
#define XRRENDER_R1_API __declspec(dllexport)
#else
#define XRRENDER_R1_API __declspec(dllimport)
#endif

// This class is exported from the xrRender_R1.dll
class XRRENDER_R1_API CxrRender_R1 {
public:
	CxrRender_R1(void);
	// TODO: add your methods here.
};

extern XRRENDER_R1_API int nxrRender_R1;

XRRENDER_R1_API int fnxrRender_R1(void);
