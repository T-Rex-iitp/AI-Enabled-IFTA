#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <exception>
#include <errno.h>

#define EXCEPTION_MESSAGE_BUFFER 1024

/*
 * Some clever books say that exception should be as lightweight
 * as possible, and I try stick to this rule for now.
 */

/**
 * Generic exception class.
 * Contains only error message.
 */ 
class Exception_: public std::exception {
public:
	Exception_(const char *str) throw() : m_Message(str) { }
	virtual ~Exception_() throw() { }

	virtual const char* what() const throw() { return m_Message; }

private:
	const char *m_Message;
};

/**
 * Exception for failed syscalls, like open().
 * Contains erro message and error code (on which strerror() may be used).
 */ 
class SysException: public std::exception {
public:
	SysException(const char *str, int errn) throw() : m_Message(str), m_Errno(errn) { }
	virtual ~SysException() throw() { }

	virtual const char* what() const throw() { return m_Message; };
	virtual int errn() const throw() { return m_Errno; };

private:
	const char	*m_Message;
	int		m_Errno;
};

/**
 * Exception for failed libcurl calls.
 * Cointains error message and curl error code.
 */ 
class CurlException: public std::exception {
public:
	CurlException(const char *str, int errn) throw() : m_Message(str), m_Errno(errn) { }
	virtual ~CurlException() throw() { }

	virtual const char* what() const throw() { return m_Message; };
	virtual int errn() const throw() { return m_Errno; };

private:
	const char	*m_Message;
	int		m_Errno;
};

/*
 * Prototype exception class; maybe usefule
 *
class SysException: public std::exception {
public:
	SysException(const char *str, int errno) : m_Messsage(str), m_Errno(errno), m_TempBuf(0) throw() { }
	virtual ~Exception() throw() { delete[] m_TempBuf; }

	virtual const char* what() const throw() {
		if (!m_TempBuf) {
			m_TempBuf = new char [EXCEPTION_MESSAGE_BUFFER];
			snprintf(m_TempBuf, EXCEPTION_MESSAGE_BUFFER, "%s: %s", 
		}

		return m_Message;
	};

private:
	const char	*m_Message;
	int		m_Errno;

	char		*m_TempBuf;
};*/

#endif
