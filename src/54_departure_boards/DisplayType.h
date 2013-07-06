////////////////////////////////////////////////////////////////////
/// DisplayType class header.
///	@file DisplayType.h
///	@author Hugues Romain
///	@date 2008
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
////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayType_H__
#define SYNTHESE_DisplayType_H__

#include "Registrable.h"
#include "Registry.h"

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace interfaces
	{
		class Interface;
	}

	namespace messages
	{
		class MessageType;
	}

	namespace departure_boards
	{
		////////////////////////////////////////////////////////////////////
		/// Type of display screen, describing the input and output facilities.
		///	@ingroup m54
		///
		/// NULL pointers on interface attributes seems that the display type
		/// is not able to use the corresponding media :
		///		- display : No display can be produced
		///		- audio : No audio reading can be produced
		///		- monitoring : The display type cannot be monitored
		////////////////////////////////////////////////////////////////////
		class DisplayType:
			public virtual util::Registrable
		{
		public:
			static const std::string ATTR_ID;
			static const std::string ATTR_NAME;
			static const std::string ATTR_MESSAGE_TYPE_ID;
			static const std::string ATTR_ROWS_NUMBER;
			static const std::string ATTR_DISPLAY_PAGE;
			static const std::string ATTR_MONITORING_PAGE_ID;
			static const std::string ATTR_MESSAGE_IS_DISPLAYED_PAGE_ID;

			/// Chosen registry class.
			typedef util::Registry<DisplayType>	Registry;

		private:
			const interfaces::Interface*	_displayInterface; // DEPRECATED				//!< On screen display Interface
			const interfaces::Interface*	_audioInterface; // DEPRECATED		//!< Interface providing exchange with a vocal reading system
			const interfaces::Interface*	_monitoringInterface; // DEPRECATED	//!< Interface used to parse monitoring outputs (see DisplayMonitoringStatus)
			size_t								_rowNumber;
			boost::optional<size_t>				_maxStopsNumber;
			boost::posix_time::time_duration	_timeBetweenChecks;		//!< Time between monitoring checks (0 = no value)

			const cms::Webpage*	_displayMainPage;
			const cms::Webpage* _displayRowPage; // DEPRECATED
			const cms::Webpage* _displayDestinationPage; // DEPRECATED
			const cms::Webpage* _displayTransferDestinationPage; // DEPRECATED
			const cms::Webpage* _monitoringParserPage;
			const cms::Webpage* _messageIsDisplayedPage;

			messages::MessageType* _messageType;

			std::string _name;

		public:


			////////////////////////////////////////////////////////////////////
			///	DisplayType constructor.
			///	@param key ID of the object
			///	@author Hugues Romain
			///	@date 2008
			///
			/// Initializes all interface pointers to NULL
			////////////////////////////////////////////////////////////////////
			DisplayType(util::RegistryKeyType key = 0);

			//! @name Getters
			//@{
				const interfaces::Interface*	getDisplayInterface()		const;
				const interfaces::Interface*	getAudioInterface()			const;
				const interfaces::Interface*	getMonitoringInterface()	const;
				size_t								getRowNumber()				const { return _rowNumber; }
				const boost::optional<size_t>&			getMaxStopsNumber()			const { return _maxStopsNumber; }
				const boost::posix_time::time_duration&	getTimeBetweenChecks()		const;
				const cms::Webpage*	getDisplayMainPage() const { return _displayMainPage; }
				const cms::Webpage* getDisplayRowPage() const { return _displayRowPage; }
				const cms::Webpage* getDisplayDestinationPage() const { return _displayDestinationPage; }
				const cms::Webpage* getDisplayTransferDestinationPage() const { return _displayTransferDestinationPage; }
				const cms::Webpage* getMonitoringParserPage() const { return _monitoringParserPage; }
				const cms::Webpage* getMessageIsDisplayedPage() const { return _messageIsDisplayedPage; }
				messages::MessageType* getMessageType() const { return _messageType; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void setDisplayInterface(const interfaces::Interface* interf);
				void setAudioInterface(const interfaces::Interface* value);
				void setMonitoringInterface(const interfaces::Interface* value);
				void setRowNumber(size_t number){ _rowNumber = number; }
				void setMaxStopsNumber(const boost::optional<size_t>& number){ _maxStopsNumber = number; }
				void setTimeBetweenChecks(const boost::posix_time::time_duration& value);
				void setDisplayMainPage(const cms::Webpage* value){ _displayMainPage = value; }
				void setDisplayRowPage(const cms::Webpage* value){ _displayRowPage = value; }
				void setDisplayDestinationPage(const cms::Webpage* value){ _displayDestinationPage = value; }
				void setDisplayTransferDestinationPage(const cms::Webpage* value){ _displayTransferDestinationPage = value; }
				void setMonitoringParserPage(const cms::Webpage* value){ _monitoringParserPage = value; }
				void setMessageIsDisplayedPage(const cms::Webpage* value){ _messageIsDisplayedPage = value; }
				void setMessageType(messages::MessageType* value){ _messageType = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			//////////////////////////////////////////////////////////////////////////
			/// Exports the object to a parameters map.
			/// @param pm the parameters map to populate
			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;
		};
	}
}

#endif // SYNTHESE_DisplayType_H__
