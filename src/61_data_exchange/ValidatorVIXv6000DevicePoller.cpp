
/** ValidatorVIXv6000DevicePoller class implementation.
	@file ValidatorVIXv6000DevicePoller.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ValidatorVIXv6000DevicePoller.hpp"

#include "Env.h"
#include "Exception.h"
#include "Log.h"

//temporary validator stuffs
#include "vix/VIX-CIntSurvMsg.hpp"
#include "vix/VIX-SerialReader.hpp"
#include "vix/VIX-timeutil.hpp"
#include "gps.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <fstream>

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	namespace data_exchange
	{
		boost::shared_ptr<ValidatorVIXv6000DevicePoller> ValidatorVIXv6000DevicePoller::_theConnection(new ValidatorVIXv6000DevicePoller);
		

		ValidatorVIXv6000DevicePoller::ValidatorVIXv6000DevicePoller()
		{
		}


		void ValidatorVIXv6000DevicePoller::RunThread()
		{
			SerialReader srt;
			CIntSurvMsg int_surv;
			unsigned char buf[COM_PORT_BUFF_SIZE];
			unsigned long long timeNextMessage = 0;
			TimeUtil tu;

			// Main loop (never ends)
			while(true)
			{
				// WARNING: the sleep is into the lower level
				// This is critical NOT to put it here. 
				// Timings are critical in rs485 protocols
				CHECKFORCOM com = srt.CheckForCommunication();
				if(com==POLLING)
				{
					printf("got polled from master at our address\n");
					int iToBeWritten = 0;
					// check if we have to send something
					if(timeNextMessage<tu.GetTickCount()){
						// create data char array
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
					printf("got selected from master at our address\n");
					// master wants to talk to us. Have to empty his queue.

					// tell master we are ready to answer.
					srt.WriteDleAct0();
					srt.ValidateMasterMessage();

					//TODO: read an validate CRC of Master message.
					// if ok, DLE ACK1 then wait on EOT
					// if not, NAK 3 times
				}
			}
		}


		void ValidatorVIXv6000DevicePoller::ParameterCallback( const std::string& name,
													  const std::string& value )
		{
		}
}	}

