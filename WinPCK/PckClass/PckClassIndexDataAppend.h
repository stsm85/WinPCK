#pragma once


class CPckClassIndexDataAppend
{
public:
	CPckClassIndexDataAppend();
	~CPckClassIndexDataAppend();

	unsigned int	size();
	unsigned char *	c_data();
	BOOL			append(unsigned char * _data, unsigned int size);

private:

	unsigned char * buffer;
	unsigned int	buffer_used, buffer_size;
};

