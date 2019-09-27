#pragma once
#include "pck_default_vars.h"

#define _INIT_SIZE_	(10 * 1024 * 1024)

class CPckMemoryCache
{
public:
	CPckMemoryCache();
	~CPckMemoryCache();

	BOOL add(const void* buf, size_t size);
	size_t size();
	const void* c_buffer();

private:

	char *buffer;
	size_t usedsize, poolsize;

	BOOL expand(size_t addsize);

};

