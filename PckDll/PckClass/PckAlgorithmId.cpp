//////////////////////////////////////////////////////////////////////
// PckAlgorithmId.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include <pch.h>
#include "PckAlgorithmId.h"

#if PCK_DEBUG_OUTPUT
#include "PckClassLog.h"
#endif

CPckAlgorithmId::CPckAlgorithmId(uint32_t id) noexcept
{
	SetAlgorithmId(id);
}

void CPckAlgorithmId::SetAlgorithmId(uint32_t id) noexcept
{
	//0	  诛仙(Jade Dynasty),完美世界(Perfect World)
	//111 热舞派对(Hot Dance Party)
	//121 口袋西游(Ether Saga Odyssey)
	//131 神魔(Forsaken World)
	//161 圣斗士(Saint Seiya),笑傲江湖(Swordsman Online)

	switch (id)
	{
	case 111:
		this->PckGuardByte0 = 0xAB12908F;
		this->PckGuardByte1 = 0xB3231902;
		this->PckMaskDword = 0x2A63810E;
		this->PckCheckMask = 0x18734563;
		break;

	default:
		this->PckGuardByte0 = 0xFDFDFEEE + id * 0x72341F2;
		this->PckGuardByte1 = 0xF00DBEEF + id * 0x1237A73;
		this->PckMaskDword = 0xA8937462 + id * 0xAB2321F;
		this->PckCheckMask = 0x59374231 + id * 0x987A223;
		break;
	}
}