#pragma once

/*
	PCK版本判断，代码源于BeySoft的PckLib
*/
#pragma warning ( disable : 4309 )

class CPckAlgorithmId
{
public:
	CPckAlgorithmId(unsigned __int32 id);
	~CPckAlgorithmId();

	unsigned __int32 GetPckGuardByte0() { return PckGuardByte0; }
	unsigned __int32 GetPckGuardByte1() { return PckGuardByte1; }
	unsigned __int32 GetPckMaskDword() { return PckMaskDword; }
	unsigned __int32 GetPckCheckMask() { return PckCheckMask; }

private:
	unsigned __int32  PckGuardByte0, PckGuardByte1, PckMaskDword, PckCheckMask;

	void SetAlgorithmId(unsigned __int32 id);

};

