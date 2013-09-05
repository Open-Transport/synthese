#pragma once

#include "VIX-BSC-defines.hpp"

#define WAIT_BUFFER_SIZE 20

namespace synthese
{

	class CComPortMgr
	{
	private:

		int m_cport_nr;		// /dev/ttyS0 (COM1 on windows)
		int m_bdrate;		// 9600 baud
		unsigned char m_waitbuffer[WAIT_BUFFER_SIZE]; 

	public:
		CComPortMgr(int cport_nr, int bdrate);
		~CComPortMgr(void);

		bool Open();	// returns true if the COM port is successfully opened.
		int ReadBuffer(unsigned char *, int size, FILE *logfile); // read and return the number of bytes red.
		bool WriteBuffer(unsigned char *buf, int size); // write the buffer to the opened port.
		bool WriteByte(unsigned char byte);
	};
}