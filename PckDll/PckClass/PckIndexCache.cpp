#include "PckIndexCache.h"
#include "gccException.h"
#include "pck_default_vars.h"

CPckMemoryCache::CPckMemoryCache() :
	usedsize(0),
	poolsize(0),
	buffer(NULL)
{
}

CPckMemoryCache::~CPckMemoryCache()
{
	if (NULL != buffer)
		free(buffer);
}

BOOL CPckMemoryCache::expand(size_t addsize)
{
	size_t newsize = (usedsize + addsize) < (poolsize + _INIT_SIZE_) ? (poolsize + _INIT_SIZE_) : (usedsize + addsize + _INIT_SIZE_);

	char* newbuffer = (char*)realloc(buffer, newsize);

	if (NULL != newbuffer) {
		poolsize = newsize;
		buffer = newbuffer;
	}
	else {

		if (NULL != (newbuffer = (char*)malloc(newsize))) {

			memcpy(newbuffer, buffer, poolsize);
			free(buffer);
			poolsize = newsize;
			buffer = newbuffer;
		}
		else {
			throw std::bad_alloc();
		}
	}
	return TRUE;
}

BOOL CPckMemoryCache::add(const void* buf, size_t size)
{
	if (0 != size) {

		if (poolsize < (usedsize + size)) {
			expand(size);
		}
		memcpy(buffer + usedsize, buf, size);
		usedsize += size;
	}

	return TRUE;
}

size_t CPckMemoryCache::size()
{
	return usedsize;
}

const void* CPckMemoryCache::c_buffer()
{
	return buffer;
}