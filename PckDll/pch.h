#pragma once
#include <memory>
#include <stdint.h>
#include "gccException.h"
#include <map>
#include <mutex>
#include <limits.h>
#include <string>
#include <deque>
#include <format>
#include <thread>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <functional>
#include <stdlib.h>
#include <zlib.h>
#include <libdeflate.h>
#include <algorithm>
#include <base64.h>
#include <filesystem>
namespace fs = std::filesystem;

//#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

#include <Windows.h>
#include <intrin.h>
#include <CharsCodeConv.h>
#include <TextLineSpliter.h>

#include <MapViewFileMultiPck.h>

#include <spdloghelper/Logger.h>
#include <spdloghelper/LoggerInit.h>

#include <source_location>
#include <spdlog\stopwatch.h>
