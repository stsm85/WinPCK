#include "COpenFileListener.h"

COpenFileListener::COpenFileListener(std::wstring& pFilePath) :m_FilePath(pFilePath)
{}

COpenFileListener::~COpenFileListener()
{}

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
				hr = isi->GetDisplayName(SIGDN_FILESYSPATH,&pwsz);

				if (SUCCEEDED(hr)) {

					m_FilePath.assign(pwsz);
//#ifdef UNICODE
//					wcscpy_s(pFilePath, MAX_PATH, pwsz);
//#else
//					WideCharToMultiByte(CP_ACP, 0, pwsz, -1, pFilePath, MAX_PATH, "_", 0);
//#endif
					CoTaskMemFree(pwsz);
				}

				isi->Release();

			}
			psiaResults->Release();
		}
		fod->Release();
	}

	return S_OK;
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

