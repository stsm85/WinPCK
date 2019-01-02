#pragma once

#define _STRIP_ECM_ 0

#include <windows.h>
#include "PckStructs.h"
#include "PckModelStripDefines.h"

class CPckModelStrip
{
public:
	CPckModelStrip();
	~CPckModelStrip();

	BOOL	StripContent(BYTE* buffer, LPPCKFILEINDEX lpFileIndex, int flag);

private:

	int		GetFileTypeByName(const wchar_t* lpszFilename);
	BOOL	StripAtt(BYTE* buffer, LPPCKFILEINDEX lpFileIndex);
	BOOL	StripGfx(BYTE* buffer, LPPCKFILEINDEX lpFileIndex);
#if _STRIP_ECM_
	BOOL	StripEcm(BYTE* buffer, LPPCKFILEINDEX lpFileIndex);
#endif
};

