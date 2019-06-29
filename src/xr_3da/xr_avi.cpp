#include "stdafx.h"
#pragma hdrstop

#include "xr_avi.h"

_CAviPlayerCustom::_CAviPlayerCustom( )
{
	ZeroMemory(this,sizeof(*this));
}
//---------------------------------
BOOL _CAviPlayerCustom::Load (char* fn)
{
	// ******** Analyze header
    AVIFileInit		();
	PAVIFILE aviFile = 0;
	if( AVIERR_OK != AVIFileOpen( &aviFile, fn, OF_READ, 0 ) )	return FALSE;

	AVIFILEINFO		aviInfo;
	ZeroMemory		(&aviInfo,sizeof(aviInfo));
	if( AVIERR_OK != AVIFileInfo( aviFile, &aviInfo, sizeof(aviInfo) ) ){
		AVIFileRelease( aviFile );
		return FALSE;
	}

	dwFrameTotal	= aviInfo.dwLength;
	fRate			= (float) aviInfo.dwRate / (float)aviInfo.dwScale;

	dwWidth			= aviInfo.dwWidth;
	dwHeight		= aviInfo.dwHeight;

	R_ASSERT		( dwWidth && dwHeight );

	dwBufferSize	= aviInfo.dwSuggestedBufferSize;
	if( dwBufferSize )
		pBuffer		= xr_malloc(dwBufferSize);
	AVIFileRelease	( aviFile );

	// ********* Open stream
	if( AVIERR_OK != AVIStreamOpenFromFile( &aviStream, fn, streamtypeVIDEO, 0, OF_READ, 0 ) )
		return FALSE;

	long headerSize = sizeof(aviInFormat);
	if( AVIERR_OK != AVIStreamReadFormat( aviStream, 0, &aviInFormat, &headerSize) )
		return FALSE;

	ZeroMemory(&aviOutFormat,sizeof(aviOutFormat));
	aviOutFormat.biSize			= sizeof(aviOutFormat);
	aviOutFormat.biBitCount		= 32;
	aviOutFormat.biCompression	= BI_RGB;
	aviOutFormat.biPlanes		= 1;
	aviOutFormat.biWidth		= dwWidth;
	aviOutFormat.biHeight		= dwHeight;

	aviIC			= ICLocate(	ICTYPE_VIDEO, 0, &aviInFormat, &aviOutFormat, ICMODE_FASTDECOMPRESS);

	if( aviIC == 0 )	return FALSE;

	if( ICERR_OK != ICDecompressBegin(aviIC, &aviInFormat, &aviOutFormat) ) return FALSE;

	return TRUE;
}


_CAviPlayerCustom::~_CAviPlayerCustom( )
{
	if( aviStream ){
		AVIStreamRelease( aviStream );
		aviStream	= 0;
	};
	if( aviIC ){
		ICDecompressEnd( aviIC );
		ICClose		( aviIC );
		aviIC		= 0;
	}

	xr_free		( pBuffer );

    AVIFileExit	();
}

BOOL _CAviPlayerCustom::DecompressFrame(u32* pDest)
{
	R_ASSERT( pDest );

	dwFrameCurrent	= CalcFrame();

	if( AVIERR_OK != AVIStreamRead(aviStream, dwFrameCurrent, 1, pBuffer, dwBufferSize, 0, 0) )
		return FALSE;

	HRESULT hr = ICDecompress(aviIC, 0, &aviInFormat, pBuffer,	&aviOutFormat, pDest );
	if( ICERR_OK != hr )	return FALSE;

	return TRUE;
}
