
/** StopButtonFilePoller class implementation.
	@file StopButtonFilePoller.cpp

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

#include "StopButtonFilePoller.hpp"

#include "Env.h"
#include "Exception.h"
#include "Log.h"
#include "CurrentJourney.hpp"
#include "ServerModule.h"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <fstream>


using namespace boost::posix_time;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace server;
	using namespace vehicle;
	
	
	namespace data_exchange
	{
		const string StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_FILE = "stop_monitoring_file";
		const string StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_PERIOD_MS = "stop_monitoring_period_ms";
		
		boost::shared_ptr<StopButtonFilePoller> StopButtonFilePoller::_theConnection(new StopButtonFilePoller);
		

		StopButtonFilePoller::StopButtonFilePoller(
		):
			_status(offline)			
		{
		}


		void StopButtonFilePoller::RunThread()
		{
			// Main loop (never ends)
			while(true)
			{
				if(	_theConnection->_status == online ||
					_theConnection->_status == connect
				){
					try
					{
						ServerModule::SetCurrentThreadRunningAction();

						time_duration periodMS(milliseconds(atoi(_theConnection->_periodMS.c_str())));
						ifstream ifs ( _theConnection->_file.c_str() );

						// Fine we have a valid period and opened the file to pool
						if(ifs.is_open() && periodMS.total_milliseconds())
						{
							_theConnection->_status = online;
						}
						else if(periodMS.total_milliseconds() == 0)
						{
							// Don't even try again if the period is 0
							_theConnection->_status = offline;
							util::Log::GetInstance().warn(
										"StopButtonFilePoller failed to parse 'stop_monitoring_period_ms' parameter. "
										"Must be a positive number of milliseconds."
							);
						}
						else
						{
							// Maybe the file will appear later
							_theConnection->_status = connect;
							util::Log::GetInstance().warn(
										"StopButtonFilePoller failed to open file '" + _theConnection->_file + "'"
							);
						}
							
						while(_theConnection->_status == online)
						{
							string line;
							
							if( ! ifs.good() )
							{
								// Will try again later
								_theConnection->_status = connect;
								break;
							}

							getline(ifs, line);
							ifs.seekg (0, ios::beg);

							if(line == "1")
							{
								VehicleModule::GetCurrentJourney().setStopRequested(true);
							}
							// Else we don't set the stopRequestTo 0, it must be reseted when the
							// bus reaches the next bus stop.
							
							this_thread::sleep(periodMS);

						}
					}
					catch(std::exception& e)
					{
						util::Log::GetInstance().info(
							e.what()
						);
					}
					catch(thread_interrupted)
					{
						throw thread_interrupted();
					}
					catch(...)
					{

					}
				}
				
				VehicleModule::GetCurrentJourney().setStopRequested(false);
				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}



		void StopButtonFilePoller::ParameterCallback( const std::string& name,
													  const std::string& value )
		{
			// File and Period
			bool changed(false);
			if(name == MODULE_PARAM_STOP_MONITORING_FILE)
			{
				changed = (_theConnection->_file != value);
				_theConnection->_file = value;
			}
			else if(name == MODULE_PARAM_STOP_MONITORING_PERIOD_MS)
			{
				changed = (_theConnection->_periodMS != value);
				_theConnection->_periodMS = value;
			}

			if(changed)
			{
				if(	!_theConnection->_file.empty() &&
					!_theConnection->_periodMS.empty()
				){
					_theConnection->_status = connect;
				}
				else
				{
					_theConnection->_status = offline;
				}
			}

		}

}	}

