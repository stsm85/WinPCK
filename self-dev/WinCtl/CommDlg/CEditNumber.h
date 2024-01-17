#pragma once
#include "CEdit.h"
#include <format>


class CEditNumber :
    public CEdit
{
public:
	CEditNumber() = default;
	virtual ~CEditNumber() = default;

	enum class NumberMode
	{
		INTEGER,
		FLOAT,
		UNSIGNED_INT,
	};
	
	void SetNumberMode(NumberMode mode)
	{
		this->m_mode = mode;
	}

	std::string string();
	int integer();
	int64_t integer64();
	uint32_t dword();
	uint64_t qword();
	double decimal();

	template<typename T>
	void set_value(T&& value)
	{
		::SetWindowTextA(this->m_hWnd, std::format("{}", std::forward<T>(value)).c_str());
	}

private:

	virtual BOOL	EventKey(UINT uMsg, int nVirtKey, LONG lKeyData);

	NumberMode m_mode = NumberMode::FLOAT;
};

