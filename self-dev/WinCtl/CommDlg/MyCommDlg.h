#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <vector>
#include <string>
#include "template_type.h"

#define	MAX_BUFFER_SIZE_OFN			0xFFFFF


class CDlgOpenDirectory
{
public:
	CDlgOpenDirectory();
	~CDlgOpenDirectory();

	//打开文件夹
	std::wstring Show(HWND hwnd);

private:

	IFileOpenDialog* pfd;
	HRESULT m_hr;
};


//构建文件过滤字符
template<typename T, class Ty = std::basic_string<T>>
Ty MakeFileExtFilter(std::vector<std::pair<Ty, Ty>> extlist);

//打开文件
template<typename T, class TOFN = char_enable_if_t<T, OPENFILENAMEA, OPENFILENAMEW>, class Ts = std::basic_string<T>>
int OpenSingleFile(HWND hWnd, Ts& lpszInitFileName, Ts lpstrFilter = Ts(), uint32_t nFilterIndex = 1);

BOOL OpenFiles(HWND hWnd, std::vector <std::wstring>& lpszFilePathArray);

template<typename T, class TOFN = char_enable_if_t<T, OPENFILENAMEA, OPENFILENAMEW>, class Ts = std::basic_string<T>>
int SaveFile(HWND hWnd, Ts& lpszFileName, const Ts lpszDefaultExt = Ts(), const Ts lpstrFilter = Ts(), uint32_t nFilterIndex = 0);


fs::path AppendToFilename(fs::path filename, std::string str2add);

#if 0
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR* lpszPathName);
static int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
#endif

