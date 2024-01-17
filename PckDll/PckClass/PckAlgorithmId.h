#pragma once
#include "pck_default_vars.h"

/*
	PCK版本判断，代码源于BeySoft的PckLib
*/
#pragma warning ( disable : 4309 )

class CPckAlgorithmId
{
public:
	CPckAlgorithmId(uint32_t id) noexcept;
	~CPckAlgorithmId() {};

	auto GetPckGuardByte0() const noexcept { return PckGuardByte0; }
	auto GetPckGuardByte1() const noexcept { return PckGuardByte1; }
	auto GetPckMaskDword() const noexcept { return PckMaskDword; }
	auto GetPckCheckMask() const noexcept { return PckCheckMask; }

private:
	uint32_t  PckGuardByte0, PckGuardByte1, PckMaskDword, PckCheckMask;
	void SetAlgorithmId(uint32_t id) noexcept;

};

