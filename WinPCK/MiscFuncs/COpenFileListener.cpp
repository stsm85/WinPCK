#include "COpenFileListener.h"

STDMETHODIMP COpenFileListener::OnFileOk(IFileDialog* pfd)
{
	IShellItemArray *psiaResults;
	HRESULT hr;
	IFileOpenDialog *fod;

	hr = pfd->QueryInterface(IID_PPV_ARGS(&fod));

	if(SUCCEEDED(hr)) {
		hr = fod->GetSelectedItems(&psiaResults);

		if (SUCCEEDED(hr)) {
			DWORD fileCount;
			IShellItem *isi;
			LPWSTR pwsz = NULL;

			psiaResults->GetCount(&fileCount);
			for(DWORD i=0;i<fileCount;i++) {
				psiaResults->GetItemAt(i,&isi);
				isi->GetDisplayName(SIGDN_FILESYSPATH,&pwsz);
				isi->Release();
#ifdef UNICODE
				wcscpy_s(pFilePath, MAX_PATH, pwsz);
#else
				WideCharToMultiByte(CP_ACP, 0, pwsz, -1, pFilePath, MAX_PATH, "_", 0);
#endif
				CoTaskMemFree(pwsz);
			}
			psiaResults->Release();
		}
		fod->Release();
	}

	return S_OK;
}


COpenFileListener::COpenFileListener(TCHAR	*pFilePath)
{
    m_cRef = 0L;
	this->pFilePath = pFilePath;
}

COpenFileListener::~COpenFileListener()
{
}

STDMETHODIMP COpenFileListener::QueryInterface(REFIID riid, LPVOID *ppv)
{
    *ppv = NULL;

	if (IsEqualIID(riid, IID_IFileDialogEvents) || IsEqualIID(riid, IID_IUnknown))
    {
		*ppv = (IFileDialogEvents *)this;
    }    

    if (*ppv)
    {
        AddRef();

        return NOERROR;
    }

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) COpenFileListener::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) COpenFileListener::Release()
{
    if (--m_cRef)
        return m_cRef;

    delete this;

    return 0L;
}

