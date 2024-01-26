#ifndef _GCC_EXCEPTION_H
#define _GCC_EXCEPTION_H

#if defined(_MSC_VER)
#include <stdexcept>
typedef std::exception MyException;

#else
#include <string>
class MyException : public std::exception
{
public:
	MyException() noexcept {}
	explicit MyException(const std::string& data) noexcept { buf = data; };
	virtual ~MyException() noexcept {};
	virtual char const* what() const { return buf.c_str(); };
private:
	std::string buf;
};
#endif

#define __ExceptionWithLine(msg, file, func, line) MyException( ##msg " at: "##file ", function: " func ", line: "#line)
#define _ExceptionWithLine(msg, file, func, line) __ExceptionWithLine(msg, file, func, line)
#define MyExceptionEx(msg) _ExceptionWithLine(msg, __FILE__, __FUNCTION__, __LINE__)

class detectversion_error : public std::exception { // base of all generic_error exceptions
public:
	explicit detectversion_error(const char* prefix, const std::string& _Message) { buf.assign(prefix); buf.append(_Message); }
	explicit detectversion_error(const char* prefix, const char* _Message) { buf.assign(prefix); buf.append(_Message); }
	explicit detectversion_error(const std::string& _Message) { buf = _Message; }
	explicit detectversion_error(const char* _Message) { buf.assign(_Message); }

	virtual char const* what() const override { return buf.c_str(); };

protected:
	std::string buf;
};


#endif //_GCC_EXCEPTION_H
