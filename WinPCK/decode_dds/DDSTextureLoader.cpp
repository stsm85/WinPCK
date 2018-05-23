//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.cpp
//
// Functions for loading a DDS texture without using D3DX
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#include "d3d9types.h"
#include "windows.h"
#include "DDSTextureLoader.h"
//#include "DDS.h"

// Comment out this define if using Windows Vista DXGI 1.0 headers
#define DXGI_1_1_FORMATS

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
UINT BitsPerPixel(D3DFORMAT fmt)
{
	UINT fmtU = (UINT)fmt;
	switch(fmtU) {
	case D3DFMT_A32B32G32R32F:
		return 128;

	case D3DFMT_A16B16G16R16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		return 64;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_G16R16:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_A2W10V10U10:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D24FS8:
	case D3DFMT_INDEX32:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 32;

	case D3DFMT_R8G8B8:
		return 24;

	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_L16:
	case D3DFMT_A8L8:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_V8U8:
	case D3DFMT_CxV8U8:
	case D3DFMT_L6V5U5:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
	case D3DFMT_D16:
	case D3DFMT_INDEX16:
	case D3DFMT_R16F:
	case D3DFMT_YUY2:
		return 16;

	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_A8P8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_A4L4:
		return 8;

	case D3DFMT_DXT1:
		return 4;

	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		return  8;

		// From DX docs, reference/d3d/enums/d3dformat.asp
		// (note how it says that D3DFMT_R8G8_B8G8 is "A 16-bit packed RGB format analogous to UYVY (U0Y0, V0Y1, U2Y2, and so on)")
	case D3DFMT_UYVY:
		return 16;

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directshow/htm/directxvideoaccelerationdxvavideosubtypes.asp
	case MAKEFOURCC('A', 'I', '4', '4'):
	case MAKEFOURCC('I', 'A', '4', '4'):
		return 8;

	case MAKEFOURCC('Y', 'V', '1', '2'):
		return 12;

		//#if !defined(D3D_DISABLE_9EX)
		//        case D3DFMT_D32_LOCKABLE:
		//            return 32;
		//
		//        case D3DFMT_S8_LOCKABLE:
		//            return 8;
		//
		//        case D3DFMT_A1:
		//            return 1;
		//#endif // !D3D_DISABLE_9EX

	default:
		//assert( FALSE ); // unhandled format
		return 0;
	}
}


//--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )

//--------------------------------------------------------------------------------------
D3DFORMAT GetD3D9Format(const DDS_PIXELFORMAT& ddpf)
{
	if(ddpf.dwFlags & DDS_RGB) {
		switch(ddpf.dwRGBBitCount) {
		case 32:
			if(ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
				return D3DFMT_A8R8G8B8;
			if(ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
				return D3DFMT_X8R8G8B8;
			if(ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				return D3DFMT_A8B8G8R8;
			if(ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000))
				return D3DFMT_X8B8G8R8;

			// Note that many common DDS reader/writers swap the
			// the RED/BLUE masks for 10:10:10:2 formats. We assumme
			// below that the 'correct' header mask is being used
			if(ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
				return D3DFMT_A2R10G10B10;
			if(ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000))
				return D3DFMT_A2B10G10R10;

			if(ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
				return D3DFMT_G16R16;
			if(ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
				return D3DFMT_R32F; // D3DX writes this out as a FourCC of 114
			break;

		case 24:
			if(ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
				return D3DFMT_R8G8B8;
			break;

		case 16:
			if(ISBITMASK(0x0000f800, 0x000007e0, 0x0000001f, 0x00000000))
				return D3DFMT_R5G6B5;
			if(ISBITMASK(0x00007c00, 0x000003e0, 0x0000001f, 0x00008000))
				return D3DFMT_A1R5G5B5;
			if(ISBITMASK(0x00007c00, 0x000003e0, 0x0000001f, 0x00000000))
				return D3DFMT_X1R5G5B5;
			if(ISBITMASK(0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000))
				return D3DFMT_A4R4G4B4;
			if(ISBITMASK(0x00000f00, 0x000000f0, 0x0000000f, 0x00000000))
				return D3DFMT_X4R4G4B4;
			if(ISBITMASK(0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00))
				return D3DFMT_A8R3G3B2;
			break;
		}
	} else if(ddpf.dwFlags & DDS_LUMINANCE) {
		if(8 == ddpf.dwRGBBitCount) {
			if(ISBITMASK(0x0000000f, 0x00000000, 0x00000000, 0x000000f0))
				return D3DFMT_A4L4;
			if(ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
				return D3DFMT_L8;
		}

		if(16 == ddpf.dwRGBBitCount) {
			if(ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
				return D3DFMT_L16;
			if(ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
				return D3DFMT_A8L8;
		}
	} else if(ddpf.dwFlags & DDS_ALPHA) {
		if(8 == ddpf.dwRGBBitCount) {
			return D3DFMT_A8;
		}
	} else if(ddpf.dwFlags & DDS_FOURCC) {
		if(MAKEFOURCC('D', 'X', 'T', '1') == ddpf.dwFourCC)
			return D3DFMT_DXT1;
		if(MAKEFOURCC('D', 'X', 'T', '2') == ddpf.dwFourCC)
			return D3DFMT_DXT2;
		if(MAKEFOURCC('D', 'X', 'T', '3') == ddpf.dwFourCC)
			return D3DFMT_DXT3;
		if(MAKEFOURCC('D', 'X', 'T', '4') == ddpf.dwFourCC)
			return D3DFMT_DXT4;
		if(MAKEFOURCC('D', 'X', 'T', '5') == ddpf.dwFourCC)
			return D3DFMT_DXT5;

		if(MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.dwFourCC)
			return D3DFMT_R8G8_B8G8;
		if(MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.dwFourCC)
			return D3DFMT_G8R8_G8B8;

		if(MAKEFOURCC('U', 'Y', 'V', 'Y') == ddpf.dwFourCC)
			return D3DFMT_UYVY;
		if(MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.dwFourCC)
			return D3DFMT_YUY2;

		// Check for D3DFORMAT enums being set here
		switch(ddpf.dwFourCC) {
		case D3DFMT_A16B16G16R16:
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_R16F:
		case D3DFMT_G16R16F:
		case D3DFMT_A16B16G16R16F:
		case D3DFMT_R32F:
		case D3DFMT_G32R32F:
		case D3DFMT_A32B32G32R32F:
		case D3DFMT_CxV8U8:
			return (D3DFORMAT)ddpf.dwFourCC;
		}
	}

	return D3DFMT_UNKNOWN;
}
