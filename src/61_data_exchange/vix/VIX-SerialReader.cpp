#include <stdio.h>

#include "VIX-SerialReader.hpp"
#include "VIX-BSC-defines.hpp"
#include "VIX-timeutil.hpp"

using namespace std;

//TODO: this is not for complex serial usage.
// We would then have to handle the NAKs and empty the queue and chech all timeouts
// So, we have a simple fallback case here that is the master polling.
// In the actual situation, the code trick is that the queue will 
// emtpy itself when falling in default mode. ( waiting to be polled from master. CheckForCommunication())

//TODO: reconnect COM port automatically when lost.

SerialReader::SerialReader(void)
{
  
#ifdef _DEBUG
	Ccrc16 crc;
	if( crc.testCRC()){
		printf(">CComPortMgr::WriteBuffer crc test SUCCEEDED!.\n");
	}else{
		printf(">CComPortMgr::WriteBuffer crc test FAILED!.\n");
	}
#endif

	// create and open the communication port
	m_comOk = false;
	//TODO(JD): make the com port variable using parameters
	m_pCom = new CComPortMgr(8, 9600);
	
	if(m_pCom){
		m_comOk = m_pCom->Open();
	}

	if(!m_comOk){
		printf("Cannot open the port\n");
	}
#ifdef RAW_DATA_SIMULATION_TEST
	m_comOk = true;
#endif 

#ifdef _DEBUG
	//m_logfile = NULL;
	//m_logfile = ::CreateFile("c:\\temp\\logtemp.bin",GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
 #ifdef _WIN32
	m_logfile = fopen ("c:/temp/logtemp.bin","w");
 #else
	m_logfile = fopen ("/tmp/logtemp.bin","w");
 #endif
#endif
}

SerialReader::~SerialReader(void)
{
	delete m_pCom;
	m_pCom=0;
	if (m_logfile!=NULL){
	  fclose (m_logfile);
	}
}

// retrieve one or more char from the com port.
// as we cannot choose to read only fiew chars, we use a simple double queue
// The second queue stores extra char we read not to lost them.
//TODO: add a timeout
//TODO: add COM PORT reconnect is lost. (No more polling from Master)
bool SerialReader::FillUpQueue(unsigned int iAmoutOfCharNeeded)
{
	if(!m_comOk)
		return false;

	bool bReadSucceeded=true;
	TimeUtil tu;
	unsigned long long msRef = tu.GetTickCount();
	unsigned long long msTimout = msRef + BSC_MASTER_TIMEOUT;

	// if queue is empty, try to read from com port.
	while(m_deque.size() < iAmoutOfCharNeeded){
		int n=0;
		if(m_comOk){
			//DEBUG
			//Sleep(60); // for test purpose
			//DEBUG
			n = m_pCom->ReadBuffer(m_buf, COM_PORT_BUFF_SIZE-1, m_logfile);
		}
		if(tu.GetTickCount()>msTimout){
			// Timed out. too long. We MUST NOT answer anything after this time.
			// Terminate the reading loop
			bReadSucceeded = false;
			break;
		}
		if(n==0){
			// nothing red. wait a bit
			tu.sleep(BSC_INTERCHAR_TIMEOUT);
		}else{
			// push char we just read into the queue
			for(int i=0;i<n;i++){
				m_deque.push_back(m_buf[i]);
			}
		}
	}

	return bReadSucceeded;
}

// look out for a known sequence of char from com port.
// return true if this sequence is a polling for at SAE address
// the trick is to consume one char at the time when we don't find the sequence we want
CHECKFORCOM SerialReader::CheckForCommunication()
{
	CHECKFORCOM comtagfound = NOTHING;
	if(!m_comOk)
		return NOTHING;
	
	FillUpQueue(4);
	if(m_deque.size()>=4){

		if( m_deque[0]==BSC_CODE_ASCII_EOT &&
			m_deque[1]==BSC_CODE_POLLING_SAE &&
			m_deque[2]==BSC_CODE_POLLING_SAE &&
			m_deque[3]==BSC_CODE_ASCII_ENQ){
				// Got polled from the master.
				// this is one of the condition we are waiting for!
				// remove this message from the queue
				m_deque.pop_front();
				m_deque.pop_front();
				m_deque.pop_front();
				m_deque.pop_front();

				comtagfound = POLLING;

		}else if(m_deque[0]==BSC_CODE_ASCII_EOT &&
				m_deque[1]==BSC_CODE_SELECTING_SAE &&
				m_deque[2]==BSC_CODE_SELECTING_SAE &&
				m_deque[3]==BSC_CODE_ASCII_ENQ){
				// Got selected from the master.
				// this is one of the condition we are waiting for!
				// remove this message from the queue
				m_deque.pop_front();
				m_deque.pop_front();
				m_deque.pop_front();
				m_deque.pop_front();

				comtagfound = SELECTING;
		}
	}

	if(comtagfound==NOTHING){
		// first char definitively useless. trow it out.
		if(m_deque.size()>=1)
			m_deque.pop_front();
	}

	return comtagfound;

}

bool SerialReader::PollingAnswerIntSurv(unsigned char *p, int nbOfCharToWrite)
{
	if(!m_comOk)
		return false;

	if(nbOfCharToWrite==0)
	{
		// nothing to write
		// tell the master (that is polling) that we have nothing to say
		// update from VIX: this DLE is not needed where: m_pCom->WriteByte(BSC_CODE_ASCII_DLE);
		m_pCom->WriteByte(BSC_CODE_ASCII_EOT);
	}
	if(nbOfCharToWrite>0 && p!=0)
	{
		// CRC on data to be write.
		unsigned short crc16 = m_crc.calculCrc16_ForTransparent_Mode(p,nbOfCharToWrite,0);
		// Add written ETX to crc!!! (Must be done like this) 
		m_crc.crc_calc(BSC_CODE_ASCII_ETX, &crc16);

		//#1 master just "polled" us.
		// and we have data to send. So do it.
						
		// Start of Text
		m_pCom->WriteByte(BSC_CODE_ASCII_DLE);
		m_pCom->WriteByte(BSC_CODE_ASCII_STX);
						
		//#2 Data (text)
		// then write the string.
		m_pCom->WriteBuffer(p, nbOfCharToWrite);
						
		// End of Text.
		m_pCom->WriteByte(BSC_CODE_ASCII_DLE);
		m_pCom->WriteByte(BSC_CODE_ASCII_ETX);
						
		// now finally write the CRC.
		m_pCom->WriteByte(crc16 & 0x00FF);
		m_pCom->WriteByte((crc16 & 0xFF00)>>8);

		printf("*** Validator wrote survey message. Done! ");
	}

	return true;
}

// we would like to get an DLE + ACT1 here.
// but we could get any other answers!
// Cannot do better here because we cannot wait. Timings are critical.
// TODO: check if we received a NAK? (Maybe better to do another function for that)
bool SerialReader::WaitForAck1()
{
	if(!m_comOk)
		return false;

	bool AckReceived = false;

	FillUpQueue(2);
	if(m_deque.size()>=2){
		if( m_deque[0]==BSC_CODE_ASCII_DLE &&
			m_deque[1]==BSC_CODE_ASCII_ACK1){
			
				// we received the message we where expected.
				// remove it from queue
				m_deque.pop_front();
				m_deque.pop_front();
				AckReceived = true;
		}
	}

	return AckReceived;
}

bool SerialReader::WriteEOT()
{
	if(!m_comOk)
		return false;

	m_pCom->WriteByte(BSC_CODE_ASCII_EOT);

	return true;
}

bool SerialReader::WaitForEOT()
{
	if(!m_comOk)
		return false;

	bool bReceived = false;

	FillUpQueue(1);
	if(m_deque.size()>=1){
		if( m_deque[0]==BSC_CODE_ASCII_EOT){
			// we received the message we where expected.
			// remove it from queue
			m_deque.pop_front();
			bReceived = true;
		}
	}

	return bReceived;
}

bool SerialReader::WriteDleAct0()
{
	if(!m_comOk)
		return false;

	m_pCom->WriteByte(BSC_CODE_ASCII_DLE);	
	m_pCom->WriteByte(BSC_CODE_ASCII_ACK0);	

	return true;
}

bool SerialReader::WriteDleAct1()
{
	if(!m_comOk)
		return false;

	m_pCom->WriteByte(BSC_CODE_ASCII_DLE);	
	m_pCom->WriteByte(BSC_CODE_ASCII_ACK1);	

	return true;
}

bool SerialReader::WaitForDleStx()
{
	if(!m_comOk)
		return false;

	bool bReceived = false;

	FillUpQueue(2);
	if(m_deque.size()>=2){
		if( m_deque[0]==BSC_CODE_ASCII_DLE &&
			m_deque[1]==BSC_CODE_ASCII_STX){

				// we received the message we where expected.
				// remove it from queue
				m_deque.pop_front();
				m_deque.pop_front();
				bReceived = true;
		}
	}

	return bReceived;
}

bool SerialReader::ReadCrc(unsigned char &crc1, unsigned char &crc2)
{
	if(!m_comOk)
		return false;

	bool bReceived = false;

	FillUpQueue(2);
	if(m_deque.size()>=2){
		crc1 = m_deque[0];
		crc2 = m_deque[1];
		// we received the message we where expected.
		// remove it from queue
		m_deque.pop_front();
		m_deque.pop_front();
		bReceived = true;

	}

	return bReceived;
}

//todo: add timeouts.
bool SerialReader::ReadMessageUntilDleEtx()
{
	if(!m_comOk)
		return false;

	bool bReceived = false;
	bool bEndOfMessage = false;
	while (!bEndOfMessage){
		FillUpQueue(1);
		if(m_deque.size()>=1)
		{
			if(m_deque[0]==BSC_CODE_ASCII_DLE)
			{
				m_deque.pop_front();
				FillUpQueue(1);
				if(m_deque.size()>=1)
				{
					if(m_deque[0]==BSC_CODE_ASCII_ETX)
				
					{
						// end of message
						bEndOfMessage = true; // needed to future handle to timeout
						bReceived = true;
						m_deque.pop_front();
					}
				}
			}else{
				// don't read the message itself now.
				// TODO: read and check the message.
				m_deque.pop_front();
			}
		}
	}

	return bReceived;
}


// Typically used to read a master message after we send a DLE ACT0
bool SerialReader::ValidateMasterMessage()
{
	if(!m_comOk)
		return false;

	// Here we do:
	// WAIT on DLE STX
	// read message until DLE ETX
	// read CRC
	//TODO: validate CRC
	// write DLE ACK1
	// wait on EOT

	bool bReceived = false;

	if(WaitForDleStx())
	{
		if(ReadMessageUntilDleEtx())
		{
			unsigned char crc1=0;
			unsigned char crc2=0;
			if(ReadCrc(crc1,crc2))
			{
				if(WriteDleAct1())
				{
					WaitForEOT();
					bReceived=true;
				}
			}
		}
	}

	return bReceived;
}