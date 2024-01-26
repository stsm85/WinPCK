//////////////////////////////////////////////////////////////////////
// AccessCheck.cpp
// 
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2024.1.1
//////////////////////////////////////////////////////////////////////

#include "AccessCheck.h"
#include <memory>

namespace myMiscUtil {

	BOOL _GetFileSecurity(LPCSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD nLength, LPDWORD lpnLengthNeeded)
	{
		return ::GetFileSecurityA(lpFileName, RequestedInformation, pSecurityDescriptor, nLength, lpnLengthNeeded);
	}

	BOOL _GetFileSecurity(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD nLength, LPDWORD lpnLengthNeeded)
	{
		return ::GetFileSecurityW(lpFileName, RequestedInformation, pSecurityDescriptor, nLength, lpnLengthNeeded);
	}
};

// 将要检测的权限GENERIC_XXXXXX传递给dwGenericAccessMask，可检测对
// 文件或者文件夹的权限
template<typename T>
BOOL CanAccessFile(const T* strPath, DWORD dwGenericAccessMask)
{

	DWORD dwSize = 0;
	PSECURITY_DESCRIPTOR psd = NULL;
	SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;

	// 获取文件权限信息结构体大小  
	BOOL bRet = myMiscUtil::_GetFileSecurity(strPath, si, psd, 0, &dwSize);
	if (bRet || ::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return FALSE;
	}

	auto pBuf = std::make_unique<char[]>(dwSize);
	ZeroMemory(pBuf.get(), dwSize);
	psd = (PSECURITY_DESCRIPTOR)pBuf.get();

	// 获取文件权限信息结构体大小  
	bRet = myMiscUtil::_GetFileSecurity(strPath, si, psd, dwSize, &dwSize);
	if (!bRet)
	{
		return FALSE;
	}

	HANDLE hToken = NULL;
	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		return FALSE;
	}

	// 模拟令牌  
	HANDLE hImpersonatedToken = NULL;
	if (!::DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken))
	{
		::CloseHandle(hToken);
		return FALSE;
	}

	// 在检测是否有某个权限时，将GENERIC_WRITE等值传给本函数的第二个参数dwGenericAccessMask
	// GENERIC_WRITE等参数在调用CreateFile创建文件时会使用到，下面调用MapGenericMask将
	// GENERIC_WRITE等转换成FILE_GENERIC_WRITE等
	// 将GENERIC_XXXXXX转换成FILE_GENERIC_XXXXXX
	GENERIC_MAPPING genMap{
		.GenericRead = FILE_GENERIC_READ,
		.GenericWrite = FILE_GENERIC_WRITE,
		.GenericExecute = FILE_GENERIC_EXECUTE,
		.GenericAll = FILE_ALL_ACCESS,
	};
	::MapGenericMask(&dwGenericAccessMask, &genMap);

	// 调用AccessCheck来检测是否有指定的权限
	PRIVILEGE_SET privileges = { 0 };
	DWORD dwGrantedAccess = 0;
	DWORD privLength = sizeof(privileges);
	BOOL bGrantedAccess = FALSE;
	if (::AccessCheck(psd, hImpersonatedToken, dwGenericAccessMask,
		&genMap, &privileges, &privLength, &dwGrantedAccess, &bGrantedAccess))
	{
		if (bGrantedAccess)
		{
			if (dwGenericAccessMask == dwGrantedAccess)
			{
				bGrantedAccess = TRUE;
			}
			else
			{
				bGrantedAccess = FALSE;
			}
		}
		else
		{
			bGrantedAccess = FALSE;
		}
	}
	else
	{
		bGrantedAccess = FALSE;
	}

	CloseHandle(hImpersonatedToken);
	CloseHandle(hToken);
	return bGrantedAccess;
}

template BOOL CanAccessFile<char>(const char* strPath, DWORD dwGenericAccessMask);
template BOOL CanAccessFile<wchar_t>(const wchar_t* strPath, DWORD dwGenericAccessMask);
