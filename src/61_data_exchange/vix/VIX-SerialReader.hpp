#pragma once

#include <deque>

#include "VIX-ComPortMgr.hpp"
#include "VIX-Ccrc16.hpp"

namespace synthese
	{

	#define COM_PORT_BUFF_SIZE 255

	enum CHECKFORCOM {
		NOTHING,
		POLLING,
		SELECTING
	};

	class SerialReader
	{
	private:
		CComPortMgr *m_pCom;
		bool m_comOk;
		unsigned char m_buf[COM_PORT_BUFF_SIZE];
		FILE* m_logfile;
		std::deque<unsigned char> m_deque;
		Ccrc16 m_crc;

	public:
		SerialReader(int comport, int baudrate);
		~SerialReader(void);

		CHECKFORCOM CheckForCommunication();

		bool PollingAnswerIntSurv(unsigned char *p, int nbOfCharToWrite);
		bool WaitForAck1();
		bool WriteEOT();
		bool WriteDleAct0();
		bool ValidateMasterMessage();

	private:
		bool FillUpQueue(unsigned int iAmoutOfCharNeeded);
		bool WaitForDleStx();
		bool ReadMessageUntilDleEtx();
		bool ReadCrc(unsigned char &crc1, unsigned char &crc2);
		bool WriteDleAct1();
		bool WaitForEOT();
	};

}