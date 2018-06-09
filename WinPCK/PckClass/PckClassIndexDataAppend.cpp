#include <Windows.h>
#include "PckClassIndexDataAppend.h"


#define INIT_SIZE	(1024*1024)
#define STEP_ADD	(512*1024)
#define MAX_SIZE	(10*1024*1024)

CPckClassIndexDataAppend::CPckClassIndexDataAppend():
	buffer_size(INIT_SIZE),
	buffer_used(0)
{
	if(NULL == (buffer = (unsigned char *)malloc(INIT_SIZE)))
		;
}

CPckClassIndexDataAppend::~CPckClassIndexDataAppend()
{
	free(buffer);
}

unsigned int CPckClassIndexDataAppend::size()
{
	return buffer_used;
}

unsigned char *	CPckClassIndexDataAppend::c_data()
{
	return buffer;
}

BOOL CPckClassIndexDataAppend::append(unsigned char * _data, unsigned int size)
{
	if((buffer_used + size) > buffer_size) {
		buffer_size = (buffer_used + size + STEP_ADD);

		unsigned char * newbuf = (unsigned char *)realloc(buffer, buffer_size);

		if(NULL == newbuf) {
			newbuf = (unsigned char *)malloc(buffer_size);
			memcpy(newbuf, buffer, buffer_used);
			free(buffer);
		}
		buffer = newbuf;
	}
	memcpy(buffer + buffer_used, _data, size);
	buffer_used += size;
	return TRUE;
}