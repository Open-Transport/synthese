
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

#include "Log.h"
#include "PermanentThread.hpp"
#include "ServerModule.h"
#include "DataSource.h"

#include "vix/VIX-CIntSurvMsg.hpp"
#include "vix/VIX-SerialReader.hpp"
#include "vix/VIX-timeutil.hpp"

#include <boost/thread.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace data_exchange;
	using namespace server;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Device, ValidatorVIXv6000DevicePoller>::FACTORY_KEY = "ValidatorVIXv6000Device_Poller";
	}

	namespace data_exchange
	{
		const std::string ValidatorVIXv6000DevicePoller::Poller_::PARAMETER_VALIDATOR_COM_PORT_NUMBER("validator_com_port_number");
		const std::string ValidatorVIXv6000DevicePoller::Poller_::PARAMETER_VALIDATOR_COM_PORT_RATE("validator_com_port_rate");
		const std::string ValidatorVIXv6000DevicePoller::Poller_::PARAMETER_VALIDATOR_DATA_SOURCE_KEY("validator_data_source_key");

		int ValidatorVIXv6000DevicePoller::Poller_::_ComPortNb=1;
		int ValidatorVIXv6000DevicePoller::Poller_::_ComPortRate=9600;
		util::RegistryKeyType ValidatorVIXv6000DevicePoller::Poller_::_dataSourceKey=0;
		
		bool ValidatorVIXv6000DevicePoller::Poller_::launchPoller(
			) const {

				// Launch the thread and returns true
				ServerModule::AddThread(boost::bind(&ValidatorVIXv6000DevicePoller::Poller_::startPolling, this), "ValidatorVIXv6000DevicePoller");

				return true;
		}

		ValidatorVIXv6000DevicePoller::Poller_::Poller_(
			util::Env& env,
			const server::PermanentThread& permanentThread,
			util::ParametersMap& pm
			):	Poller(env, permanentThread, pm)
		{}

		util::ParametersMap ValidatorVIXv6000DevicePoller::Poller_::getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_VALIDATOR_COM_PORT_NUMBER, _ComPortNb);
			map.insert(PARAMETER_VALIDATOR_COM_PORT_RATE, _ComPortRate);
			map.insert(PARAMETER_VALIDATOR_DATA_SOURCE_KEY, _dataSourceKey);

			return map;
		}

		void ValidatorVIXv6000DevicePoller::Poller_::setFromParametersMap(const util::ParametersMap& map)
		{
			_ComPortNb = map.getDefault<int>(PARAMETER_VALIDATOR_COM_PORT_NUMBER, 8);
			_ComPortRate = map.getDefault<int>(PARAMETER_VALIDATOR_COM_PORT_RATE, 9600);
			_dataSourceKey = map.getDefault<util::RegistryKeyType>(PARAMETER_VALIDATOR_DATA_SOURCE_KEY, 0);
		}

		void ValidatorVIXv6000DevicePoller::Poller_::startPolling() const
		{
			SerialReader srt(_ComPortNb,_ComPortRate);
			CIntSurvMsg int_surv;
			unsigned char buf[COM_PORT_BUFF_SIZE];
			unsigned long long timeNextMessage = 0;
			TimeUtil tu;

			/*DEBUG(JD)*/
			_dataSourceKey= 16607027920896001;// TODO: TOID RTTB urbain
			/*DEBUG(JD)*/

			Log::GetInstance().info(str(format("ValidatorVIXv6000DevicePoller: PortNumber=%d. Rate=%d") % _ComPortNb % _ComPortRate));

			boost::shared_ptr<const impex::DataSource> dataSource = Env::GetOfficialEnv().get<impex::DataSource>(_dataSourceKey);

			while (true)
			{

				// WARNING: the sleep is into the lower level. (look into  SerialReader::FillUpQueue)
				// This is critical NOT to put it here. 
				// Timings are critical in rs485 protocols
				CHECKFORCOM com = srt.CheckForCommunication();
				if(com==POLLING)
				{
					//util::Log::GetInstance().debug("got polled from master at our address\n");

					int iToBeWritten = 0;
					// check if we have to send something
					if(timeNextMessage<tu.GetTickCount()){
						// create data char array

						//

						iToBeWritten = int_surv.StreamToBuffer(buf, COM_PORT_BUFF_SIZE-1, dataSource);
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
					//util::Log::GetInstance().debug("got selected from master at our address\n");

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
	}
}

