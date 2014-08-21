/** SCOMData class header.
	@file SCOMData.h

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

#include "SCOMData.h"

#include "IConv.hpp"

#include "Log.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace scom
	{
		using namespace synthese::util;
		using boost::property_tree::ptree;

		//------------------------------------- PUBLIC ------------------------------

		SCOMData::SCOMData () :
				_maxAge(60),
				_maxTimeDiff(boost::posix_time::seconds(120))
		{

		}

		// Parse the XML and store its values
		// If the XML message is not the wanted one, ignore it
		void SCOMData::AddXML(const std::string &data)
		{
			// Tree that will contain the XML structure
			ptree pt;

			// Convert the string in a stringstream (needed for read_xml)
			std::stringstream ss;
			ss << data;

			// Try to read the XML in the message
			try
			{
				read_xml(ss,pt);
			}
			catch (boost::property_tree::xml_parser::xml_parser_error &e)
			{
				Log::GetInstance().warn("SCOM Data : Failed to read xml message : " + std::string(e.what()));
				return;
			}
			catch (...)
			{
				Log::GetInstance().warn("SCOM Data : Failed to read xml message with unknown error");
				return;
			}

			// Too much debug for prod, so commented
			//Log::GetInstance().debug("SCOM Data : New data "+data);

			try
			{

				// For each message found, insert it
				BOOST_FOREACH( ptree::value_type & m, pt )
				{
					std::string msgType = m.first;

					// Ignore any other messages that RAFFIND
					if ( msgType != "RAFFIND" )
					{
						continue;
					}

					// Construct the basic datas
					Data d;
					d.receivedAt = boost::posix_time::second_clock::local_time();
					d.borne = m.second.get<std::string>("BORNE.<xmlattr>.NUMERO");

					// Integrate de sent date and time
					std::string date = m.second.get<std::string>("BORNE.<xmlattr>.DATE");
					std::string time = m.second.get<std::string>("BORNE.<xmlattr>.HEURE");
					std::istringstream ssd(date + " " + time);
					ssd.imbue(std::locale(ssd.getloc(), new boost::posix_time::time_input_facet("%d/%m/%Y %H:%M:%S")));
					ssd >> d.sentAt;

					// Fetch the lines
					BOOST_FOREACH( ptree::value_type & l, m.second.get_child("LISTE_LIGNE_ECRAN") )
					{
						// One data for each line, so a copy is made
						Data dl = d;
						dl.line = l.second.get<std::string>("<xmlattr>.LIGNE");

						// Separate each values (DEST1, DEST2, etc... very weird XML from my point of view)
						for (int i = 1; i < 3; i++)
						{
							std::string nb = boost::lexical_cast<std::string>(i);
							Data dls = dl;

							// Necessary values
							try
							{
								// The destination needs to be converted from iso-8859-1
								IConv conv("iso-8859-1","utf8");
								dls.destination	= conv.convert(l.second.get<std::string>("<xmlattr>.DEST"+nb));

								dls.carac		= l.second.get<std::string>("<xmlattr>.CARAC"+nb);
								dls.fiability	= l.second.get<std::string>("<xmlattr>.FIABLE"+nb);
								dls.tps			= l.second.get<int>("<xmlattr>.TPS"+nb);

								// If the given TPS equal to the "at the station" flag, replace it by 0
								if (dls.tps == BUSATSTOP)
								{
									dls.tps = 0;
								}

								// Calculate absolute time
								dls.busTime	= dls.sentAt + boost::posix_time::minutes(dls.tps);
							}
							catch (boost::property_tree::ptree_bad_path &)
							{
								// Ignore if a value is not found
								continue;
							}

							// Optionnal ones
							try
							{
								dls.via	= l.second.get<std::string>("<xmlattr>.VIA"+nb);
							}
							catch (boost::property_tree::ptree_bad_path &) { }

							// Append the line
							_append(dls);

							Log::GetInstance().debug("SCOM Data : Received data for borne " + d.borne + " at " + boost::posix_time::to_simple_string(d.sentAt));
						}
					}
				}
			}
			catch (boost::property_tree::xml_parser::xml_parser_error &e)
			{
				Log::GetInstance().warn("SCOM Data : Failed to parse xml message : " + std::string(e.what()));
			}
			catch (std::exception &e)
			{
				Log::GetInstance().warn("SCOM Data : Failed to parse xml message : " + std::string(e.what()));
			}
			catch (...)
			{
				Log::GetInstance().warn("SCOM Data : Failed to parse xml message with unknown error");
			}
		}

		// Return the waiting time as specified by SCOMData documentation
		boost::posix_time::ptime SCOMData::GetWaitingTime(
				const std::string &borne,
				const std::string &line,
				const std::string &destination,
				boost::posix_time::ptime originalWaitingTime)
		{
			// Note : do not forget to unlock
			_mutex.lock();

			// By default, use the given time
			// If a better on is found it will be replaced
			boost::posix_time::ptime time = originalWaitingTime;

			// Loop through the data received from SCOM
			bool found = false;
			bool invalidTime = false;
			for(std::vector<Data>::iterator it = _datas.begin(); it != _datas.end(); ++it)
			{
				// Check if the borne, line and destination are found in our data
				if ( borne == it->borne && line == it->line && destination == it->destination)
				{
					// The original time and the time received from SCOM must not differ too much
					boost::posix_time::time_duration diff = originalWaitingTime - it->busTime;
					if ( abs(diff.total_seconds()) < _maxTimeDiff.total_seconds() )
					{
						time = it->busTime;
						found = true;
						break;
					}
					else
					{
						invalidTime = true;
					}
				}
			}

			// If we didn't found relevant data
			std::string msg;
			if ( ! found )
			{
				if ( invalidTime )
				{
					msg = " not found. Reason : no time close enough found";
				}
				else
				{
					msg = " not found. Reason : borne, line or destination not found";
				}
			}
			else
			{
				msg = " found, replaced with " + boost::posix_time::to_simple_string(time);
			}

			// Logging (only on found data, else there is too much messages)
			if (found)
			{
				Log::GetInstance().debug("SCOMData : Corresponding waiting time for line " + line
										 + ", on borne " + borne
										 + " for destination " + destination
										 + " with a waiting time of " + boost::posix_time::to_simple_string(originalWaitingTime)
										 + msg
										 );
			}

			_mutex.unlock();

			// In any case, return the time, be it the originalWaitingTime or a better on found in our data
			return time;
		}

		//------------------------- GETTERS / SETTERS ------------------------------

		// Max age
		int SCOMData::MaxAge() const
		{
			return _maxAge;
		}

		void SCOMData::SetMaxAge(int maxAge)
		{
			_maxAge = maxAge;
		}

		// Max time diff
		int SCOMData::MaxTimeDiff() const
		{
			return _maxTimeDiff.total_seconds();
		}

		void SCOMData::SetMaxTimeDiff(int maxTimeDiff)
		{
			_maxTimeDiff = boost::posix_time::seconds(maxTimeDiff);
		}

		//------------------------------------- PRIVATE ------------------------------

		// Remove all values older than _maxAge
		// The calling function must use the writing mutex
		void SCOMData::_cleanup()
		{
			// Convert the max age in a "now - maxAge" date
			boost::posix_time::ptime maxDate = boost::posix_time::second_clock::local_time();
			maxDate -= boost::posix_time::seconds(_maxAge);

			// Remove anything older than this date
			for(std::vector<Data>::iterator it = _datas.begin(); it != _datas.end(); ++it) {
				if ( it->receivedAt < maxDate )
				{
					_datas.erase(it,_datas.end());
					break;
				}
			}
		}

		// Loop through the datas and insert when the time is in order
		void SCOMData::_append(const Data &data)
		{
			_mutex.lock();

			// If the vector is empty, just add it
			if (_datas.empty())
			{
				_datas.push_back(data);
			}
			else
			{
				// Try to find a data for the same borne/line/direction and remove it if it is older
				bool toInsert = true;
				for (std::vector<Data>::iterator it = _datas.begin(); it != _datas.end(); ++it)
				{
					if ( data.borne == it->borne && data.line == it->line && data.destination == it->destination)
					{
						// Replace the old value if the received on is more recent
						if (it->sentAt < data.sentAt)
						{
							_datas.erase(it);
						}
						else // This means that we received a data older than the one we have. Odd, but with the network between, might happen.
						{
							toInsert = false;
						}
						break; // As this is applied every time something is added, no need to search in all the data, the first found is enough
					}
				}

				// Insert sorted by received time (if needed)
				if ( toInsert )
				{
					bool inserted = false;
					for (std::vector<Data>::iterator it = _datas.begin(); it != _datas.end(); ++it)
					{
						// Sorted with receivedAt, why?
						// Because this data is cleaned using the local time, not the SCOM time.
						// So it is the arrival order that is used, not the message time
						if (it->receivedAt < data.receivedAt)
						{
							_datas.insert(it,data);
							inserted = true;
							break;
						}
					}

					// If not inserted now, insert at the bottom
					if ( ! inserted )
					{
						_datas.insert(_datas.end(),data);
					}

					// Cleanup the old stuff
					_cleanup();
				}
			}


			_mutex.unlock();
		}
	}
}
