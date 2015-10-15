
//////////////////////////////////////////////////////////////////////////
/// CommercialLineUpdateAction class header.
///	@file CommercialLineUpdateAction.h
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_CommercialLineUpdateAction_H__
#define SYNTHESE_CommercialLineUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "RGBColor.h"
#include "BaseImportableUpdateAction.hpp"

namespace synthese
{
	namespace tree
	{
		class TreeFolder;
	}

	namespace pt
	{
		class CommercialLine;
		class ReservationContact;
		class TransportNetwork;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Update of properties of a CommercialLine object.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the commercial line to update</li>
		///		<li>actionParamna : name (for the menu)</li>
		///		<li>actionParamsn : short name (for the cartouche)</li>
		///		<li>actionParamln : long name (for the road map)</li>
		///		<li>actionParamco : color code (@ref RGBColor)</li>
		///		<li>actionParamst : CSS style class (for the cartouche)</li>
		///		<li>actionParamim : image url (for the cartouche)</li>
		///		<li>actionParamni : network id</li>
		///		<li>actionParamri : reservation contact id</li>
		///		<li>actionParamdsl : data source links</li>
		///	</ul>
		class CommercialLineUpdateAction:
			public util::FactorableTemplate<server::Action, CommercialLineUpdateAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SHORT_NAME;
			static const std::string PARAMETER_LONG_NAME;
			static const std::string PARAMETER_COLOR;
			static const std::string PARAMETER_FOREGROUND_COLOR;
			static const std::string PARAMETER_STYLE;
			static const std::string PARAMETER_IMAGE;
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_RESERVATION_CONTACT_ID;
			static const std::string PARAMETER_MAP_URL;
			static const std::string PARAMETER_DOC_URL;
			static const std::string PARAMETER_TIMETABLE_ID;
			static const std::string PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE;
			static const std::string PARAMETER_WEIGHT_FOR_SORTING;

		private:
			boost::shared_ptr<pt::CommercialLine> _line;
			boost::optional<std::string>	_name;		//!< Name (code)
			boost::optional<std::string>	_shortName;	//!< Name (cartouche)
			boost::optional<std::string>	_longName;	//!< Name for schedule card
			boost::optional<boost::optional<util::RGBColor> >		_color;		//!< CommercialLine color
			boost::optional<boost::optional<util::RGBColor> >		_fgColor;		//!< CommercialLine foreground color
			boost::optional<std::string>			_style;		//!< CSS style (cartouche)
			boost::optional<std::string>			_image;		//!< Display image (cartouche)
			boost::optional<boost::shared_ptr<TransportNetwork> >	_network;	//!< Network
			boost::optional<boost::shared_ptr<tree::TreeFolder> >	_folder;	//!< Folder
			boost::optional<boost::shared_ptr<pt::ReservationContact> >	_reservationContact;	//!< Reservation contact
			boost::optional<std::string> _mapURL;
			boost::optional<std::string> _docURL;
			boost::optional<util::RegistryKeyType> _timetableId;
			boost::optional<boost::posix_time::time_duration> _displayDurationBeforeFirstDeparture;
			boost::optional<int> _weightForSorting;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;


			//! @name Setters
			//@{
				void setLine(boost::shared_ptr<pt::CommercialLine> value) { _line = value; }
				void setNetwork(boost::optional<boost::shared_ptr<TransportNetwork> > value){ _network = value; }
				void setFolder(boost::optional<boost::shared_ptr<tree::TreeFolder> > value){ _folder = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLineUpdateAction_H__
