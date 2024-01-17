#include "CEditNumber.h"

//#include <format>
BOOL CEditNumber::EventKey(UINT uMsg, int nVirtKey, LONG lKeyData)
{
#if 0
	switch (uMsg)
	{
	case WM_CHAR:
		::OutputDebugStringA(std::format("WM_CHAR nVirtKey={} {}\r\n", nVirtKey, (char)nVirtKey).c_str());
		if (('0' <= nVirtKey && '9' >= nVirtKey) || ('-' == nVirtKey) || ('.' == nVirtKey) || 8 == nVirtKey)
			return FALSE;
		else
			return TRUE;
		break;
	case WM_KEYUP:
		::OutputDebugStringA(std::format("WM_KEYUP nVirtKey={} {}\r\n", nVirtKey, (char)nVirtKey).c_str());
		break;
	case WM_KEYDOWN:
		::OutputDebugStringA(std::format("WM_KEYDOWN nVirtKey={} {}\r\n", nVirtKey, (char)nVirtKey).c_str());
		break;
	}
#endif
	if (WM_CHAR == uMsg)
	{
		switch (this->m_mode)
		{
		case NumberMode::UNSIGNED_INT:
			if (!(('0' <= nVirtKey && '9' >= nVirtKey) || 8 == nVirtKey))
				return TRUE;
			break;

		case NumberMode::FLOAT:
			if (!(('0' <= nVirtKey && '9' >= nVirtKey) || ('-' == nVirtKey) || ('.' == nVirtKey) || 8 == nVirtKey))
				return TRUE;
			break;

		case NumberMode::INTEGER:
			if (!(('0' <= nVirtKey && '9' >= nVirtKey) || ('-' == nVirtKey) || 8 == nVirtKey))
				return TRUE;
			break;

		}
	}

	return FALSE;
}

std::string CEditNumber::string()
{
	std::string buf;
	auto size = ::GetWindowTextLengthA(this->m_hWnd);
	buf.reserve(size + 1);
	buf.resize(size);
	::GetWindowTextA(this->m_hWnd, buf.data(), buf.size() + 1);
	return std::move(buf);
}


int CEditNumber::integer()
{
	auto s = this->string();
	return std::stoi(s);
}

int64_t CEditNumber::integer64()
{
	auto s = this->string();
	return std::stoll(s);
}

uint32_t CEditNumber::dword()
{
	auto s = this->string();
	return std::stoul(s);
}

uint64_t CEditNumber::qword()
{
	auto s = this->string();
	return std::stoull(s);
}

double CEditNumber::decimal()
{
	auto s = this->string();
	return std::stod(s);
}

