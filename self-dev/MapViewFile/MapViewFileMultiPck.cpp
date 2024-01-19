#include "pch_mvf.h"
#include "MapViewFileMultiPck.h"


CMapViewFileMultiPck::CMapViewFileMultiPck()
{
	this->SetPackageCountAndSuffix({ ".pck", ".pkx", ".pkg" });
}

void CMapViewFileMultiPck::SetPackageCountAndSuffix(const std::vector<std::string>& name)
{
	this->m_package_count = name.size();
	this->m_fileSuffix = name;
	this->m_szPckFileName.resize(name.size());
	this->m_uqwPckSize.resize(name.size());
}

void CMapViewFileMultiPck::initialization_filenames(const fs::path& lpszFilename)
{
	for (auto& file : this->m_szPckFileName) {
		file = lpszFilename;
	}

	for (int i = 1; i < this->m_szPckFileName.size(); i++)
	{
		this->m_szPckFileName[i].replace_extension(this->m_fileSuffix[i]);
	}
}