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

#ifndef SYNTHESE_SCOMData_h__
#define SYNTHESE_SCOMData_h__

#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace scom
	{
		/// Constant defining the code for a bus currently at the stop
		static const int BUSATSTOP = 67;
		/// Constant defining the code for a bus that left the stop
		static const int BUSGONE = -1;

		/** Data handler for SCOM

		  This object has two roles :
		  * Analyse and import the XML fetched from the SCOM tcp socket
		  * Return a sensible time-to-wait value for a borne/line/time

		  == Data cleaning ==
		  Any data older than a limit is automatically destroyed on the next addXML() call.
		  As the data is not persistent we need this to avoid filling the memory.
		  A sensible default value is set for the oldest acceptable data but it can be changed
		  through SetMaxAge().

		  == Why not in the database with the rest ? ==
		  We would like to, but it isn't possible.

		  Why? Because the SCOM messages doesn't contains enough information to link
		  it with a specific course of a specific line.
		  No ID is provided, so the only thing we can do is give the SCOM waiting time for a specific
		  theorical waiting time from Synthese, with a possibility of a false result.

		  TODO Matching

		  TODO Thread safety

		  TODO XML Format
		*/
		class SCOMData
		{
		public:

			// Setup the XML object
			SCOMData();

			/** Add an SCOM formatted XML data
			  The data will be stored until it gets too old.

			  This function is protected by a mutex to avoid concurency problems on
			  sorting and cleaning.

			  Only RAFFIND message should be sent here, any other message will be
			  discarded and a warning message printed in the log.

			  @param data The XML data to store
			*/
			void AddXML(const std::string& data);

			/** The waiting time from SCOM for the corresponding line in Synthese

				Remember that the result of this function might be wrong because of a lack of information.
				See the class documentation.

				The match is done with the given parameters.
				See the class documentation for a full description.

				@param borne The borne for which to give the waiting time
				@param line The bus line
				@param destination The bus destination
				@param originalWaitingTime The waiting time currently used (from Synthese)
			*/
			int GetWaitingTime (
					const std::string& borne,
					const std::string& line,
					const std::string& destination,
					int originalWaitingTime);

			/** Set the maximum age of a data
			  Any data older than that will be removed on the next addXML() call.
			  @param maxAge The maximum allowed age of a data, in seconds
			  */
			void SetMaxAge (int maxAge);

		private:

			// Structure used internally to store the XML data
			// It corresponds to one waiting value (not an XML LINE)
			struct Data
			{
				// The borne for which this value has been emitted by SCOM
				std::string borne;
				// Bus line
				std::string line;
				// Bus destination
				std::string destination;
				// The bus characteristics
				std::string carac;
				// The line's VIA
				std::string via;
				// Waiting time fiability
				std::string fiability;
				// Time to wait until the arrival of the bus to the borne/stop
				int tps;
				// Time this information has been sent
				boost::posix_time::ptime& sentAt;
				// Time this information has been received
				boost::posix_time::ptime& receivedAt;
			};

			// Vector of data
			// This vector should always be sorted by sent date (most recent first)
			std::vector<Data> _datas;

			// Maximum age of a data before its being cleaned
			int _maxAge;

			// Cleanup any old data around here
			void _cleanup ();

			// Add the data to the vector in the right order
			void _append (const Data& data);

		};
	}
}

#endif // SYNTHESE_SCOMData_h__
