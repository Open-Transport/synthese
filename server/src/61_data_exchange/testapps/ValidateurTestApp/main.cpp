#include <iostream>
#include <stdio.h>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "VIX-CIntSurvMsg.hpp"
#include "VIX-SerialReader.hpp"
#include "VIX-timeutil.hpp"

using namespace std;
using namespace boost;
//using namespace boost::this_thread;
using namespace synthese;

#define DEBUG_THREADS_MAX_COUNT 100000

bool isNumParkValid(std::string &s, short &parknumber)
{
  bool bNumParkValid=false;
    try{
	parknumber = boost::lexical_cast<short>(s);  

	if((parknumber>709)&&(parknumber<800)){
	    bNumParkValid=true;
	}
  
    }catch(bad_lexical_cast &){
      bNumParkValid=false;
    }  
    
    return bNumParkValid;
}

int main(int argc, char **argv) {
  
	short parknumber=0;
	
	{
		ifstream theFile( "numpark.txt", ios_base::in );
		std::string s;
		theFile >> s;
		
		bool valid = isNumParkValid(s, parknumber);
		while (valid==false)
		{
			std::cout << "Numpark.txt introuvable ou invalide!" << std::endl;
			std::cout << "Un numero de park est necessaire!" << std::endl;
			std::cout << "Veillez choisir entre 710 et 799" << std::endl;
			std::cout << "Veillez entrer un numero de park: ";
			std::getline(std::cin, s);
		      
			valid = isNumParkValid(s, parknumber);
			
			if(valid){
				ofstream theNewFile;
				theNewFile.open("numpark.txt");
				theNewFile << parknumber;
				theNewFile.close();
			}
		}
		std::cout << "Park number is: " << parknumber << std::endl;
	}

	SerialReader srt(0,9600);
	CIntSurvMsg int_surv;
	unsigned char buf[COM_PORT_BUFF_SIZE];
	unsigned long long timeNextMessage = 0;
	TimeUtil tu;

	while( true /*!_kbhit()*/)
	{  
		CHECKFORCOM com = srt.CheckForCommunication();
		if(com==POLLING)
		{
			//printf("got polled from master at our address\n");
			int iToBeWritten = 0;
			// check if we have to send something
			if(timeNextMessage<tu.GetTickCount())
			{
				// create data char array
				int_surv.num_park = parknumber;
				iToBeWritten = int_surv.StreamToBuffer(buf, COM_PORT_BUFF_SIZE-1);
				timeNextMessage = tu.GetTickCount() + BSC_SURV_TIME_MS;
			}
			srt.PollingAnswerIntSurv(buf,iToBeWritten);

			if(srt.WaitForAck1())
			{
				srt.WriteEOT();
			}

			//TODO: Could receive a NAK too.
			// so, we need to handle it 3 time then ignore and then restart normal listening.

		}else if(com==SELECTING)
		{
			//printf("got selected from master at our address\n");
			// master wants to talk to us. Have to empty his queue.

			// tell master we are ready to answer.
			srt.WriteDleAct0();
			srt.ValidateMasterMessage();

			//TODO: read an validate CRC of Master message.
			// if ok, DLE ACK1 then wait on EOT
			// if not, NAK 3 times
		}

	}
	getchar(); // consume the keyboard input

	printf("hit ENTER to exit");
	getchar();
	
	return 0;
}
