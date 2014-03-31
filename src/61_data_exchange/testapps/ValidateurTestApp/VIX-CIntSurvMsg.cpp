#include <stdio.h>
#include <string.h>

//#include "Log.h"

#include "VIX-CIntSurvMsg.hpp"
#include "VIX-BSC-defines.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/date.hpp"

//#include "Vehicle.hpp"
//#include "VehicleModule.hpp"
//#include "StopPoint.hpp"
//#include "ScheduledService.h"

namespace synthese
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
  
//	using namespace vehicle;
//	using namespace pt;

	CIntSurvMsg::CIntSurvMsg(void)
	{
		type		= INT_SURV;
		//TODO(JD): set those values to 0 by default when debug done
		year		= 113;
		month		= 8;
		day		= 19;
		hour		= 12;
		min		= 10;
		sec		= 50;
		num_driver	= 4320;
		num_park	= 705;
		etat_expl	= 1;
		num_line	= 3;
		num_service	= 4024;	// No service agent
		num_journey	= 21;	// course NOT USED in VIX code.
		num_stop	= 1934;
		direction	= 0;
	}

	CIntSurvMsg::~CIntSurvMsg(void)
	{
	}

	void CIntSurvMsg::IntLigneToString(char *p, unsigned int i)
	{
		//TODO(jd): add buffer len check
		p[0]=0x20;
		p[1]=0;
		p[2]=0;
		sprintf(p, "%d", i);
		size_t len = strlen(p);
		if(len>0)
		{
			p[len]=0x20;
			p[len+1]=0x20;
			p[len+2]=0x20;
			p[len+3]=0x20;
			p[len+4]=0;
			p[len+5]=0;
		}
	}

	// because of BSC transparent mode, we need to double any char the has a DLE value.
	// so, we must return the number of real char inserted. (So 0 if any error, 1 or 2)
	int CIntSurvMsg::insertCharToBufferTransparentMode(unsigned char *buf, int bufSize, unsigned char cToInsert)
	{
		//BSC (BiSync) Transparent mode. Double the any DLE found in the sequence.
		int nbInserted = 0;

		// check if the char to insert is a special one.
		if(cToInsert==BSC_CODE_ASCII_DLE)
		{
			// yes, we need to double it.
			if(bufSize>1){
				buf[0]=BSC_CODE_ASCII_DLE;
				buf[1]=BSC_CODE_ASCII_DLE;
				nbInserted=2;	// we must not cound transpatent mode extra data (Will be removed)
			}
		}else{
			// ok, just insert it.
			if(bufSize>0){
				//not a DLE special control character. we can insert it.
				buf[0]=cToInsert;
				nbInserted=1;
			}
		}

		if(nbInserted==0)
		{
			// got an error. buffer too small
			printf("CIntSurv::insertCharToBufferTransparentMode CANNOT insert char. Buffer too small\n");
		}

		return nbInserted;

	}

	// Update the variables from the Synthese Environment.
	// Return true if anything has changed.
	bool CIntSurvMsg::UpdateVariablesFromEnv()
	{
		bool bUpdated = false;

		//TODO?: add timer not to update too often
		{
			// add time and date.
			ptime now(second_clock::local_time());	
			
			// trick for winter time change.
			ptime winterchange(date(2013,Oct,27), hours(3)); 
			ptime summerchange(date(2014,Mar,20), hours(2));

			if((now>winterchange)&&(now<summerchange))
			{
				printf("Validator: Winter");
				now-=hours(1);
			}else{
				printf("Validator: Summer");
			}
			
			year	= now.date().year()-2000;
			if(year<0){
				year=0;
			}
			month	= now.date().month();
			day	= now.date().day();
			hour	= now.time_of_day().hours();
			min	= now.time_of_day().minutes();
			sec	= now.time_of_day().seconds();
			
			printf(": %d, %d, %d. hour %d:%d:%d (numpark=%d)\n", day, month, year+2000, hour, min, sec, num_park);

			// get stop number, direction and line.
//			VehiclePosition &vp = VehicleModule::GetCurrentVehiclePosition();
//
//			std::string &stoppointname = vp.getStopPoint()->getName();
//			num_stop	= 4052;	// TODO!
//
//			const std::string &servicenumber = vp.getService()->getServiceNumber();
//			num_service	= boost::lexical_cast<short>(servicenumber);
/*
			num_driver	= 9998;
//			num_park	= 710;
			etat_expl	= 0;
			num_line	= 1;
			num_service	= 9998;
			num_journey	= 1;	// course NOT USED in VIX code.
			num_stop	= 9998;	
			direction	= 0;
*/
/*
			direction	= 0;	// TODO!
			num_line	= 6;	// TODO!
			//TODO: lower priority, but we need to get it
			num_driver	= 1;	// TODO: low priority
			num_park	= 1;	// TODO: low priority
			etat_expl	= 1;	// TODO: low priority
			num_journey	= 1;	// Don't care: course NOT USED in VIX code.
*/
			bUpdated=true;
		}

		return bUpdated;

	}

	int CIntSurvMsg::StreamToBuffer(unsigned char *buf, int bufSize)
	{
		// Vehicle position is update in the gps poller
		UpdateVariablesFromEnv();

		if(bufSize<INT_SURV_BUF_SIZE){
			// buf size Must be at least equal to INT_SURV_BUF_SIZE
			printf("CIntSurv::StreamToBuffer INVALID buffer size\n");
			return 0;
		}

		int o = 0;
		buf[o] = INT_SURV_DATA_SIZE;
		o+=1;
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, type);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, year);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, month);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, day);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, hour);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, min);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, sec);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, (num_driver>>8) & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, num_driver & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, (num_park>>8) & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, num_park & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, etat_expl);

		// this one is a "Text" field.
		// we have to guess the codding. Probably "regular" ASCII
		// but with 'low char' first. and space for null char. like "1   " for decimal 1.
		// so, "9998" for 9998. => 0x38 0x39 0x39 0x39
		// and "128 " for 128. => 0x31 0x32 0x38 0x20
		char p[10];
		IntLigneToString(p, num_line);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, p[0]);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, p[1]);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, p[2]);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, p[3]);

		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, (num_service>>8) & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, num_service & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, (num_journey>>8) & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, num_journey & 0xFF);

		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, (num_stop>>8) & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, num_stop & 0xFF);
		o+=insertCharToBufferTransparentMode(&buf[o], bufSize-o, direction);

		return o;
	}
}