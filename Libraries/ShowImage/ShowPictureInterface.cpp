

#include "showimgpch.h"
#include "ShowPictureWithZoom.h"


std::unique_ptr<IShowPicture> MakeShowPictureInstance()
{
	return std::make_unique<CShowPicture>();
}

std::unique_ptr<IShowPicture> MakeZoomShowPictureInstance()
{
	return std::make_unique<CShowPictureWithZoom>();
}