#pragma once

#include <functional>
using namespace std;

typedef std::function<void(const char, const wchar_t*)> InsertLogFunc;
typedef std::function<void(const wchar_t*)> SetStatusBarInfoFunc;

class CLogUnits
{
public:
	CLogUnits();
	~CLogUnits();

	void setInsertLogFunc(InsertLogFunc func) {
		m_InsertLog = func;
	}

	void setSetStatusBarInfoFunc(SetStatusBarInfoFunc func) {

		m_SetStatusBarInfo = func;
	}

	void InsertLog(const char ch, const wchar_t* str) {
		try {
			m_InsertLog(ch, str);
		}
		catch (std::bad_function_call ex) {
			;
		}
	}

	void SetStatusBarInfo(const wchar_t* str) {
		try {
			m_SetStatusBarInfo(str);
		}
		catch(std::bad_function_call ex) {
			;
		}
	}

private:

	InsertLogFunc m_InsertLog = nullptr;
	SetStatusBarInfoFunc m_SetStatusBarInfo = nullptr;

};

extern CLogUnits LogUnits;
void	PreInsertLogToList(const char, const wchar_t *);

//»’÷æ
#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

