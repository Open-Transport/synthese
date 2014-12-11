//////////////////////////////////////////////////////////////////////////
/// AlertProcessingThread class implementation.
///	@file AlertProcessingThread.cpp
///	@author Marc Jambert
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#include "AlertProcessingThread.hpp"
#include "Log.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "CallbackRequestAlertProcessor.hpp"

using namespace std;

namespace synthese
{
    namespace regulation
    {

        AlertProcessingThread::AlertProcessingThread()
            : _hasToStop(false)
            , _thread()
        {
            _alertProcessors.push_back(boost::shared_ptr<AlertProcessor>(
                                           new CallbackRequestAlertProcessor()));
        }

    
        void
        AlertProcessingThread::operator()()
        {
            while (!hasToStop())
            {
                util::Log::GetInstance().info("MJ This is stuff that my thread does");
				BOOST_FOREACH(boost::shared_ptr<AlertProcessor>& alertProcessor, _alertProcessors)
				{
                    alertProcessor->processAlerts();
                }
                boost::this_thread::sleep(boost::posix_time::seconds(3));
            }
        }

    
        void
        AlertProcessingThread::start()
        {
            _thread.reset(new boost::thread(boost::ref(*this)));

        }

        
        bool
        AlertProcessingThread::hasToStop() const
        {
            _mutex.lock();
            bool result(_hasToStop);
            _mutex.unlock();
            return result;
        }

    
        void
        AlertProcessingThread::stop()
        {
            _mutex.lock();
            _hasToStop = true;
            _mutex.unlock();
        }

    }
}
