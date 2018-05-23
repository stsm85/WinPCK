//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.h
//
// Functions for loading a DDS texture without using D3DX
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "DDS.h"

D3DFORMAT GetD3D9Format( const DDS_PIXELFORMAT& ddpf );
UINT BitsPerPixel( D3DFORMAT fmt );
