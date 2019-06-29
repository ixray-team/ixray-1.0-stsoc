#ifndef __XR_AVI_H__
#define __XR_AVI_H__
#include "vfw.h"

class ENGINE_API		_CAviPlayerCustom
{
public:
	u32					dwBufferSize;
	void				*pBuffer;

	PAVISTREAM			aviStream;
	HIC					aviIC;
	BITMAPINFOHEADER	aviOutFormat;
	BITMAPINFOHEADER	aviInFormat;

	float				fRate;
	u32					dwFrameTotal;
	u32					dwFrameCurrent;

	u32					dwWidth,dwHeight;

	u32					CalcFrame()
	{	return iFloor(Device.fTimeGlobal*fRate)%dwFrameTotal; }
public:
						_CAviPlayerCustom	( );
						~_CAviPlayerCustom	( );

	BOOL				Load				( char* fn     );
	BOOL				DecompressFrame		( u32* pDest );

	IC BOOL				NeedUpdate			() { return CalcFrame()!=dwFrameCurrent; }
};
#endif //__XR_AVI_H__
