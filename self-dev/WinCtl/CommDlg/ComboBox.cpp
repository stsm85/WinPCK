#include "ComboBox.h"


std::string CComboBox::GetLBText(int index)
{
	std::string s;
	//char* str = nullptr;

	LRESULT len = GetLBTextLen(index); // length, excluding the terminating null character
	if (len == CB_ERR)
		return "";

	//str = new char[len + 1];
	s.resize(len);

	LRESULT len2 = GetLBText(index, s.data());

	if (len2 == CB_ERR) {
		//delete[] str;
		return "";
	}

	if (len > len2)
		s.resize(len2);
	//s.assign(str);
	//delete[] str;
	return s;
}

// selection helpers
int CComboBox::FindString(int nStartAfter, LPCSTR lpszString)
{
	int count = GetCount();
	if (0 == count)
		return CB_ERR;
	if (nStartAfter >= count)
		return CB_ERR;

	if (0 > nStartAfter)
		nStartAfter = 0;

	//std::string str;

	for (int i = nStartAfter; i < count; i++) {

		auto str = GetLBText(i);
		if (0 == str.compare(lpszString))
			return i;
	}
	return CB_ERR;
}
int CComboBox::SelectString(int nStartAfter, LPCSTR lpszString)
{
	return SetCurSel(FindString(nStartAfter, lpszString));
}

