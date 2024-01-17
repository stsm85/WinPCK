
//////////////////////////////////////////////////////////////////////
// OpenSaveDlg.cpp: WinPCK 辅助函数部分
// 显示文件打开、保存对话框
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

#include "MyCommDlg.h"
#include "COpenFileListener.h"

CDlgOpenDirectory::CDlgOpenDirectory()
{
	CoInitialize(NULL);
	m_hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));
}

CDlgOpenDirectory::~CDlgOpenDirectory()
{
	if (SUCCEEDED(m_hr))
		pfd->Release();
	CoUninitialize();
}

std::wstring CDlgOpenDirectory::Show(HWND hWnd)
{
	
	FILEOPENDIALOGOPTIONS dwOptions;
	DWORD dwCookie = 0;
	std::wstring path;

	// CoCreate the dialog object.
	if (!SUCCEEDED(m_hr))
		throw std::runtime_error("CoCreateInstance FAIL!");

	HRESULT hr = pfd->GetOptions(&dwOptions);

	if (!SUCCEEDED(hr))
		throw std::runtime_error("GetOptions FAIL!");

	hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

	if (!SUCCEEDED(hr))
		throw std::runtime_error("SetOptions FAIL!");

	COpenFileListener* ofl = new COpenFileListener(path);

	hr = pfd->Advise(ofl, &dwCookie);

	if (!SUCCEEDED(hr))
		throw std::runtime_error("Advise FAIL!");

	hr = pfd->Show(hWnd);

	auto a = HRESULT_FROM_WIN32(hr);
	if ((!SUCCEEDED(hr)) && (HRESULT_FROM_WIN32(ERROR_CANCELLED) != hr))
		throw std::runtime_error("show dialog fail!");

	pfd->Unadvise(dwCookie);
	
	return path;
}


#define DEFAULT_FILTER	"所有文件\0*.*\0\0"

#define __L(quote) L##quote
#define _L(x) __L(x)

//构建文件过滤字符
template<typename T, class Ty>
Ty MakeFileExtFilter(std::vector<std::pair<Ty, Ty>> extlist)
{
	Ty result;
	for (auto& ext : extlist) {

		result.append(ext.first);
		result.append(1, 0);
		result.append(ext.second);
		result.append(1, 0);
	}
	result.append(1, 0);

	return result;
}

template std::string MakeFileExtFilter<char>(std::vector<std::pair<std::string, std::string>> extlist);
template std::wstring MakeFileExtFilter<wchar_t>(std::vector<std::pair<std::wstring, std::wstring>> extlist);

/******************************************************
只打开一个文件
******************************************************/
BOOL TGetOpenFileName(OPENFILENAMEA* ofn)
{
	return GetOpenFileNameA(ofn);
}

BOOL TGetOpenFileName(OPENFILENAMEW* ofn)
{
	return GetOpenFileNameW(ofn);
}

const char* GetDefaultFilter(const std::string& lpstrFilter)
{
	if (0 == lpstrFilter.size())
		return DEFAULT_FILTER;
	else
		return lpstrFilter.c_str();
}

const wchar_t* GetDefaultFilter(const std::wstring& lpstrFilter)
{
	if (0 == lpstrFilter.size())
		return _L(DEFAULT_FILTER);
	else
		return lpstrFilter.c_str();
}

template<typename T, class TOFN, class Ts>
int OpenSingleFile(HWND hWnd, Ts& lpszInitFileName, Ts lpstrFilter, uint32_t nFilterIndex)
{
	TOFN ofn{ 0 };
	T szStrPrintf[MAX_PATH]{ 0 };

	if((0 != lpszInitFileName.size()) && ('\\' == lpszInitFileName.at(lpszInitFileName.size()-1))){

		ofn.lpstrInitialDir = lpszInitFileName.c_str();
		ofn.lpstrFile = szStrPrintf;
	}
	else {
		lpszInitFileName.copy(szStrPrintf, lpszInitFileName.size(), 0);
		szStrPrintf[lpszInitFileName.size()] = 0;
		ofn.lpstrFile = szStrPrintf;
	}

	ofn.lStructSize = sizeof(TOFN);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = GetDefaultFilter(lpstrFilter);
	ofn.nFilterIndex = nFilterIndex;
	//ofn.lpstrFile         = lpszFileName;
	//ofn.lpstrInitialDir   = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING/* | OFN_ALLOWMULTISELECT*/;

	if (!TGetOpenFileName(&ofn))
		throw std::runtime_error("用户已取消或发生错误");

	lpszInitFileName = szStrPrintf;
	return ofn.nFilterIndex - 1;
}

template int OpenSingleFile<char>(HWND hWnd, std::string& lpszInitFileName, std::string lpstrFilter, uint32_t nFilterIndex);
template int OpenSingleFile<wchar_t>(HWND hWnd, std::wstring& lpszInitFileName, std::wstring lpstrFilter, uint32_t nFilterIndex);


BOOL OpenFiles(HWND hWnd, std::vector<std::wstring>& lpszFilePathArray)
{

	auto szBuffer = std::make_unique<wchar_t[]>(MAX_BUFFER_SIZE_OFN);
	if (szBuffer == nullptr) {
		return FALSE;
	}

	szBuffer[0] = 0;

	OPENFILENAMEW ofn{
		.lStructSize = sizeof(OPENFILENAMEW),
		.hwndOwner = hWnd,
		.lpstrFilter = L"所有文件\0*.*\0\0",
		.lpstrFile = szBuffer.get(),
		.nMaxFile = MAX_BUFFER_SIZE_OFN,
		.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST,
	};

	if (!GetOpenFileNameW(&ofn)) {
		if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL) {
			MessageBoxW(hWnd, L"选择的文件太多. 缓冲区无法装下所有文件的文件名。",
				L"缓冲区不够", MB_OK);
		}
		return FALSE;
	}

	lpszFilePathArray.clear();

	// if first part of szBuffer is a directory the user selected multiple files
	// otherwise szBuffer is filename + path

	fs::path base_file(szBuffer.get());
	if(fs::is_directory(base_file)) {
		// szBuffer 中第一个部分是目录 
		// 其他部分是 FileTitle

		auto szBufferPart = szBuffer.get() + ofn.nFileOffset;

		//根目录下时目录名中会带'\'
		if (4 == ofn.nFileOffset) {
			ofn.nFileOffset--;
			szBuffer[2] = 0;
		}

		while (0 != *szBufferPart) {

			std::wstring file_title(szBufferPart);
			lpszFilePathArray.push_back((base_file / file_title).wstring());

			szBufferPart += file_title.size() + 1;
		}
	}
	else { // 中选中了一个文件

		lpszFilePathArray.push_back(szBuffer.get());

	}
	return TRUE;
}

//保存文件
BOOL TGetSaveFileName(OPENFILENAMEA* ofn)
{
	return GetSaveFileNameA(ofn);
}

BOOL TGetSaveFileName(OPENFILENAMEW* ofn)
{
	return GetSaveFileNameW(ofn);
}

template<typename T, class TOFN, class Ts>
int SaveFile(HWND hWnd, Ts& lpszFileName, const Ts lpszDefaultExt, const Ts lpstrFilter, uint32_t nFilterIndex)
{
	/*
	nFilterIndex 的base index是1，所以输入时要+1，输出要-1
	*/
	T szStrPrintf[MAX_PATH]{ 0 };

	if (0 != lpszFileName.size()) {
		lpszFileName.copy(szStrPrintf, lpszFileName.size(), 0);
		szStrPrintf[lpszFileName.size()] = 0;
	}

	TOFN ofn = { 0 };

	ofn.lStructSize = sizeof(TOFN);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = GetDefaultFilter(lpstrFilter);
	ofn.lpstrFile = szStrPrintf;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = lpszDefaultExt.c_str();
	ofn.nFilterIndex = nFilterIndex + 1;

	//计算lpstrDefExt的字符值
	if (!TGetSaveFileName(&ofn))
		throw std::runtime_error("用户已取消或发生错误");

	lpszFileName = szStrPrintf;

	return ofn.nFilterIndex - 1;

}

template int SaveFile<char>(HWND hWnd, std::string& lpszFileName, const std::string lpszDefaultExt, const std::string lpstrFilter, uint32_t nFilterIndex);
template int SaveFile<wchar_t>(HWND hWnd, std::wstring& lpszFileName, const std::wstring lpszDefaultExt, const std::wstring lpstrFilter, uint32_t nFilterIndex);

fs::path AppendToFilename(fs::path filename, std::string str2add)
{
	auto filestem = filename.stem().string() + str2add + filename.extension().string();
	filename.replace_filename(filestem);
	return filename;
}


#if 0
BOOL BrowseForFolderByPath(HWND hWnd, TCHAR* lpszPathName)
{

	BROWSEINFO					cBI;
	ITEMIDLIST					idl;
	LPITEMIDLIST				pidl = &idl;

	cBI.hwndOwner = hWnd;
	cBI.pidlRoot = 0;
	cBI.lpszTitle = TEXT("请选择目录");
	cBI.lpfn = BFFCALLBACK(&BFFCallBack);

	cBI.lParam = (LPARAM)lpszPathName;

	cBI.pszDisplayName = lpszPathName;

	cBI.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	//OleInitialize(0);
	pidl = SHBrowseForFolder(&cBI);
	//OleUninitialize();
	if (NULL != pidl) {
		SHGetPathFromIDList(pidl, lpszPathName);
		CoTaskMemFree((VOID*)pidl);
		return TRUE;
	}
	return FALSE;
}

int CALLBACK BFFCallBack(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//TCHAR szPath[MAX_PATH];
	//TCHAR szExePath[MAX_PATH];

	//ITEMIDLIST					idl;
	//LPITEMIDLIST				pidl = &idl;
	//BOOL bTemp;

	switch (uMsg) {
	case BFFM_INITIALIZED:
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}

	return 0;
}
#endif