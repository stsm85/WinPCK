#include "PckAlgorithmId.h"
#include "pck_default_vars.h"
#if PCK_DEBUG_OUTPUT
#include "PckClassLog.h"
#endif

CPckAlgorithmId::CPckAlgorithmId(unsigned __int32 id)
{
	SetAlgorithmId(id);
}


CPckAlgorithmId::~CPckAlgorithmId()
{}

void CPckAlgorithmId::SetAlgorithmId(unsigned __int32 id)
{
	//0	  诛仙(Jade Dynasty),完美世界(Perfect World)
	//111 热舞派对(Hot Dance Party)
	//121 口袋西游(Ether Saga Odyssey)
	//131 神魔(Forsaken World)
	//161 圣斗士(Saint Seiya),笑傲江湖(Swordsman Online)

	switch (id)
	{
	case 111:
		PckGuardByte0 = 0xAB12908F;
		PckGuardByte1 = 0xB3231902;
		PckMaskDword = 0x2A63810E;
		PckCheckMask = 0x18734563;
		break;

	default:
		PckGuardByte0 = 0xFDFDFEEE + id * 0x72341F2;
		PckGuardByte1 = 0xF00DBEEF + id * 0x1237A73;
		PckMaskDword = 0xA8937462 + id * 0xAB2321F;
		PckCheckMask = 0x59374231 + id * 0x987A223;
		break;
	}
}