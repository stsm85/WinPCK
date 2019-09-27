#ifndef _GCC_EXCEPTION_H
#define _GCC_EXCEPTION_H

using namespace std;

#if defined(_MSC_VER)
#include <exception>
#define MyException exception

#else
#include <string>

class MyException
{
public:
	MyException(string data) { buf = data; };
	~MyException() /*noexcept*/ {};
	virtual char const* what() const { return buf.c_str(); };
private:
	std::string buf;
};
#endif

#define __ExceptionWithLine(msg, file, func, line) MyException(##msg " at: "##file ", function: " func ", line: "#line)
#define _ExceptionWithLine(msg, file, func, line) __ExceptionWithLine(msg, file, func, line)
#define MyExceptionEx(msg) _ExceptionWithLine(msg, __FILE__, __FUNCTION__, __LINE__)

#endif //_GCC_EXCEPTION_H
