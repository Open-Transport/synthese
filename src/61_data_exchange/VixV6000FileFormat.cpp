
/** VixV6000FileFormat class implementation.
	@file VixV6000FileFormat.cpp

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

#include "VixV6000FileFormat.hpp"

#include "Log.h"
#include "ServerModule.h"
#include "DataSource.h"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"
#include "StopPoint.hpp"
#include "ScheduledService.h"
#include "CommercialLine.h"

#include "vix/VIX-SerialReader.hpp"

#include <boost/thread.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace util;
	using namespace db;
	using namespace vehicle;
	

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, VixV6000FileFormat>::FACTORY_KEY = "vix_v6000";
	}

	namespace data_exchange
	{
		const std::string VixV6000FileFormat::Exporter_::PARAMETER_COM_PORT_NUMBER = "com_port_number";
		const std::string VixV6000FileFormat::Exporter_::PARAMETER_COM_PORT_RATE = "com_port_rate";
		const std::string VixV6000FileFormat::Exporter_::PARAMETER_DATASOURCE_ID = "data_source_id";



		VixV6000FileFormat::Exporter_::Exporter_(
			const Export& export_
		):	PermanentThreadExporterTemplate<VixV6000FileFormat>(export_),
			Exporter(export_),
			_status(OFFLINE),
			_timeNextMessage(0)
		{}



		util::ParametersMap VixV6000FileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_COM_PORT_NUMBER, _comPortNb);
			map.insert(PARAMETER_COM_PORT_RATE, _comPortRate);
			if(_dataSource)
			{
				map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
			}

			return map;
		}



		void VixV6000FileFormat::Exporter_::setFromParametersMap(const util::ParametersMap& map)
		{
			_comPortNb = map.getDefault<int>(PARAMETER_COM_PORT_NUMBER, 8);
			_comPortRate = map.getDefault<int>(PARAMETER_COM_PORT_RATE, 9600);

			// Datasource
			RegistryKeyType dataSourceId(
				map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0)
			);
			if(dataSourceId) try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(dataSourceId);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
			}
		}



		void VixV6000FileFormat::Exporter_::_onStart() const
		{
			_srt.reset(new SerialReader(_comPortNb, _comPortRate));
			_timeNextMessage = 0;
			
			Log::GetInstance().info(str(format("VixV6000FileFormat: PortNumber=%d. Rate=%d") % _comPortNb % _comPortRate));

			if(!_dataSource)
			{
				_status = INCONSISTENT_PARAMETERS;
			}
		}



		void VixV6000FileFormat::Exporter_::_loop() const
		{
			if(_status == INCONSISTENT_PARAMETERS)
			{
				return;
			}

			// WARNING: the sleep is into the lower level. (look into  SerialReader::FillUpQueue)
			// This is critical NOT to put it here. 
			// Timings are critical in rs485 protocols
			CHECKFORCOM com = _srt->CheckForCommunication();
			if(com==POLLING)
			{
				util::Log::GetInstance().debug("VixV6000FileFormat : got polled from master at our address\n");
				_status = ONLINE_POLLING;

				int iToBeWritten = 0;
				unsigned char buf[COM_PORT_BUFF_SIZE];

				// check if we have to send something
				if(_tu.GetTickCount() > _timeNextMessage)
				{
					// create data char array
					util::Log::GetInstance().debug("VixV6000FileFormat : message creation.\n");
					CIntSurvMsg int_surv(_getMessage());
			
					iToBeWritten = int_surv.StreamToBuffer(buf, COM_PORT_BUFF_SIZE-1);
					util::Log::GetInstance().debug("VixV6000FileFormat : message is sent.\n");

					_timeNextMessage = _tu.GetTickCount() + 50; //BSC_SURV_TIME_MS;
				}
				else
				{
					util::Log::GetInstance().debug("VixV6000FileFormat : writing is ignored because the last message was sent too recently.\n");
				}

				_srt->PollingAnswerIntSurv(buf,iToBeWritten);

				if(_srt->WaitForAck1())
				{
					_srt->WriteEOT();
				}

				//TODO: Could receive a NAK too.
				// so, we need to handle it 3 time then ignore and then restart normal listening.

			}
			else if(com==SELECTING)
			{
				//util::Log::GetInstance().debug("got selected from master at our address\n");
				_status = ONLINE_SELECTING;

				// master wants to talk to us. Have to empty his queue.

				// tell master we are ready to answer.
				_srt->WriteDleAct0();
				_srt->ValidateMasterMessage();

				//TODO: read an validate CRC of Master message.
				// if ok, DLE ACK1 then wait on EOT
				// if not, NAK 3 times
			}
			else
			{
				_status = OFFLINE;
			}
		}



		boost::posix_time::time_duration VixV6000FileFormat::Exporter_::_getWaitingTime() const
		{
			if(_status == ONLINE_POLLING || _status == ONLINE_SELECTING)
			{
				return seconds(0);
			}

			return seconds(1);
		}



		void VixV6000FileFormat::Exporter_::_onStop() const
		{
			_srt.reset();
		}



		synthese::CIntSurvMsg VixV6000FileFormat::Exporter_::_getMessage() const
		{
			CIntSurvMsg result;

			//TODO: add NULL pointer 
			//TODO?: add timer not to update too often
			// add time and date.
			ptime now(second_clock::local_time());	
			result.year	= now.date().year()-2000;
			result.month	= now.date().month();
			result.day		= now.date().day();
			result.hour	= now.time_of_day().hours();
			result.min		= now.time_of_day().minutes();
			result.sec		= now.time_of_day().seconds();

			// Get stop number, direction and line.
			VehiclePosition &vp = VehicleModule::GetCurrentVehiclePosition();
			// TODO: clue: in Synthese all services are unique and there is one service number per bus path. (service = Time+AtoZ)

			pt::ScheduledService *pService = vp.getService(); 
			if(pService)
			{
				const std::string &servicenumber = pService->getServiceNumber();
				result.num_service	= boost::lexical_cast<short>(servicenumber);

				if(pService->getRoute()->getWayBack())
				{
					result.direction = 1;
				}
				else
				{
					result.direction = 0;
				}

				pt::CommercialLine* line(pService->getRoute()->getCommercialLine());
				const std::vector<std::string> &lineCodes = line->getCodesBySource(*_dataSource);
				
				if(lineCodes.size()>0)
				{
					result.num_line = boost::lexical_cast<unsigned int>(*lineCodes.begin());
				}
			}
			else
			{
				//TODO: unknown service
			}

			pt::StopPoint* pStoppoint = vp.getStopPoint();
			if(pStoppoint)
			{
				const std::vector<std::string> &stopCodes = pStoppoint->getCodesBySource(*_dataSource);

				if(stopCodes.size()>0)
				{
					result.num_stop = boost::lexical_cast<unsigned int>(*stopCodes.begin());
				}
			}
			else
			{
				//TODO: unknown position. use non located
			}

			//TODO: lower priority, but we need to get those too
			result.num_driver	= 1;	// TODO: low priority
			result.num_park	= 799;	// TODO: low priority
			result.etat_expl	= 1;	// TODO: low priority
			result.num_journey	= 1;	// Don't care: course NOT USED in VIX code.
		
			return result;
		}
}	}

