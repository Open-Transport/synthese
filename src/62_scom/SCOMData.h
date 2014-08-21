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
#include <boost/signals2/mutex.hpp>

namespace synthese
{
	namespace scom
	{
		/** Data handler for SCOM

		  This object has two roles :
		  * Analyse and import the XML fetched from the SCOM tcp socket
		  * Return a sensible time-to-wait value for a borne/line/time

		  <h2> Data cleaning </h2>
		  Any data older than a limit is automatically destroyed on the next addXML() call.
		  As the data is not persistent we need this to avoid filling the memory.
		  A sensible default value is set for the oldest acceptable data but it can be changed
		  through SetMaxAge().

		  Only the last value for a line/destination/borne is kept.
		  An incoming value (through AddXML()) that have been sent (sent time) at a later time will remove the old value when inserted.

		  <h2> Why not in the database with the rest ? </h2>
		  We would like to, but it isn't possible.

		  Why? Because the SCOM messages doesn't contains enough information to link
		  it with a specific course of a specific line.
		  No ID is provided, so the only thing we can do is give the SCOM waiting time for a specific
		  theorical waiting time from Synthese, with a possibility of a false result.

		  <h2> Matching </h2>
		  Same line, borne and destination
		  Time diff original/found < maxDiffTime

		  <h2> Thread safety </h2>
		  The writing (AddXML()) and reading (GetWaitingTime()) operations are thread safe thanks to the use of a mutex.
		  Concurrent read operations are not possible for now.

		  <h2> XML Format </h2>
		  The full specification of the XML format can be found in the SCOM specification.

		  Here, only one kind of message is used : RAFFIND.
		  Any other message will be quietly discarded.

		  A RAFFIND XML :
		  <pre>
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- RAFFIND le 14/08/2014 \e0 10:15:48  -->
<RAFFIND>
	<BORNE NUMERO="517" DATE="14/08/2014" HEURE="10:15:48"/>
	<LISTE_LIGNE_ECRAN>
		<LIGNE_ECRAN NUMERO="1" LIGNE="17" DEST1="Renens-14 Avril" CARAC1="0" FIABLE1="F" TPS1="67" DEST2="Bussigny C-Peage" CARAC2="184" FIABLE2="F" TPS2="8"/>
	</LISTE_LIGNE_ECRAN>
</RAFFIND>
		  </pre>
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
			  discarded.

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
			boost::posix_time::ptime GetWaitingTime (
					const std::string& borne,
					const std::string& line,
					const std::string& destination,
					boost::posix_time::ptime originalWaitingTime);

			/** Maximum age of a data
			  Any data older than that will be removed on the next AddXML() call.
			  @param maxAge The maximum allowed age of a data, in seconds
			  */
			void SetMaxAge (int maxAge);
			int MaxAge () const;

			/** Maximum time difference allowed for a data
			  If the difference between the original time and the one found in SCOM
			  (for the same line, direction and borne) is bigger than this value it will be ignored.
			  @param maxAge The maximum allowed time difference, in seconds
			  */
			void SetMaxTimeDiff (int maxTimeDiff);
			int MaxTimeDiff () const;


			/// Constant defining the code for a bus currently at the stop
			static const int BUSATSTOP = 67;
			/// Constant defining the code for a bus that left the stop
			static const int BUSGONE = -1;

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
				// Absolute time made from tps and the sentAt time
				boost::posix_time::ptime busTime;
				// Time this information has been received
				boost::posix_time::ptime receivedAt;
				// Time this information has been sent by SCOM
				boost::posix_time::ptime sentAt;
			};

			// Vector of data
			// This vector should always be sorted by sent date (most recent first)
			std::vector<Data> _datas;

			// Maximum age of a data before its being cleaned
			int _maxAge;

			// Maximum difference between a SCOM waiting time and the theorical waiting time for it to be considered
			boost::posix_time::time_duration _maxTimeDiff;

			// Mutex for data writing
			boost::signals2::mutex _mutex;


			// Cleanup any old data around here
			void _cleanup ();

			// Add the data to the vector in the right order
			// Is protected by a mutex
			void _append (const Data& data);

		};
	}
}

#endif // SYNTHESE_SCOMData_h__
