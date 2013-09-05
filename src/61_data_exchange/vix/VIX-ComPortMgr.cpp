#include <queue>
#include <stdio.h>

#include "Log.h"

#include "VIX-ComPortMgr.hpp"
#include "VIX-BSC-defines.hpp"
#include "VIX-rs232.hpp"

// cool info at http://ckp.made-it.com/bisync.html


namespace synthese
{

	CComPortMgr::CComPortMgr(int cport_nr, int bdrate)
	{
		m_cport_nr=cport_nr;	// com port ID. Start from 0. IE: /dev/ttyS0 (COM1 on windows)
		m_bdrate=bdrate	;	// com port "speed". IE 9600 baud
	}

	CComPortMgr::~CComPortMgr(void)
	{
		RS232_CloseComport(m_cport_nr);
	}

	bool CComPortMgr::Open()
	{
	#ifdef RAW_DATA_SIMULATION_TEST
		return true;
	#endif

		//printf(">CComPortMgr::Open ENTER.\n");
		bool bRet = false; 

		RS232_CloseComport(m_cport_nr);
		std::string msg;
		msg.append("CComPortMgr::Open FAILED. port index: %d. Baud: %d", m_cport_nr, m_bdrate);

		if(RS232_OpenComport(m_cport_nr, m_bdrate)){
			util::Log::GetInstance().error(msg);
			bRet = false;
		}else{
			util::Log::GetInstance().debug(msg);
			bRet = true;
		}

		//printf("<CComPortMgr::Open EXIT.\n");
		return bRet;
	}

	int CComPortMgr::ReadBuffer(unsigned char *buf, int size, FILE *logfile)
	{
		//printf(">CComPortMgr::ReadBuffer ENTER.\n");
	#ifdef RAW_DATA_SIMULATION_TEST


		static int index=0;
		index++;
		int nBytesRed = 0;

		switch (index)
		{
		case 1:
			buf[0]=0x04;buf[1]=0xC1;buf[2]=0xC1;buf[3]=0x05;
			nBytesRed=4;
			break;
		case 2:
			buf[0]=0x04;buf[1]=0xCC;buf[2]=0xCC;buf[3]=0x05;
			nBytesRed=4;
			break;
		case 3:
			buf[0]=0x10;buf[1]=0x31;
			nBytesRed=2;
			break;
		case 4:
			buf[0]=0x04;buf[1]=0x8C;buf[2]=0x8C;buf[3]=0x05;
			nBytesRed=4;
			break;
		case 5:
			buf[0]=0x10;buf[1]=0x02;buf[2]=0x41;buf[3]=0x00;buf[4]=0x10;buf[5]=0x03;buf[6]=0x10;buf[7]=0x15;
			nBytesRed=8;
			break;
		case 6:
			buf[0]=0x04;
			nBytesRed=1;
			break;
		case 7:
			buf[0]=0x04;buf[1]=0xCD;buf[2]=0xCD;buf[3]=0x05;
			nBytesRed=4;
			break;
		default:
			Sleep(1000);
			index = 0;
		}
		
	#else
		int nBytesRed = RS232_PollComport(m_cport_nr, buf, size);
	#endif

	#ifdef _DEBUG_ME
		if(nBytesRed >0)
		{
			if(logfile){
				fwrite(buf, 1, nBytesRed,logfile);
			}

			//printf("received %i bytes:tick(%0x04x)", nBytesRed, tick);
			printf("<ReadBuffer %i bytes: ", nBytesRed);
			for(int i=0; i < nBytesRed; i++)
			{
				//printf("¦%04X:%02X", tick, buf[i]);
				printf("%02X", buf[i]);
			}

			printf("\n");
		}
	#endif

		//printf("<CComPortMgr::ReadBuffer EXIT.\n");
		return nBytesRed;
	}

	// return true if buffer was successfully written
	bool CComPortMgr::WriteBuffer(unsigned char *buf, int size)
	{
		//printf(">CComPortMgr::WriteBuffer ENTER.\n");

		bool bRet = true; 

	#ifdef RAW_DATA_SIMULATION_TEST
		int nBytesWritten = size;
	#else
		int nBytesWritten = RS232_SendBuf(m_cport_nr, buf, size);
	#endif

	#ifdef _DEBUG_ME
		printf("CComPortMgr::WriteBuffer buffer: ");
		for (int i=0;i<size;i++)
		{
			printf("%02X ",buf[i]);
		}
		printf("\n");
	#endif
		if(nBytesWritten<0)
		{
			util::Log::GetInstance().error("CComPortMgr::WriteBuffer FAILED. buffer maybe not sent!");
			bRet = false;
		}

		//printf("<CComPortMgr::WriteBuffer EXIT.\n");
		return bRet;
	}

	bool CComPortMgr::WriteByte(unsigned char byte)
	{
	#ifdef _DEBUG_ME
		printf(">WriteByte. 0x%02X\n", byte);
	#endif
		bool bRet = true; 

	#ifdef RAW_DATA_SIMULATION_TEST
		int nBytesWritten = 1;
	#else
		int nBytesWritten = RS232_SendByte(m_cport_nr, byte);
	#endif

		if(nBytesWritten<0)
		{
			util::Log::GetInstance().error("CComPortMgr::WriteByte FAILED. buffer maybe not sent!");
			bRet = false;
		}

		//printf("<CComPortMgr::WriteByte EXIT.\n");
		return bRet;
	}

}