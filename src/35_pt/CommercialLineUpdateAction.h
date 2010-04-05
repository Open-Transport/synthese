
//////////////////////////////////////////////////////////////////////////
/// CommercialLineUpdateAction class header.
///	@file CommercialLineUpdateAction.h
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace pt
	{
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
		///		<li>actionParamci : creator id</li>
		///	</ul>
		class CommercialLineUpdateAction:
			public util::FactorableTemplate<server::Action, CommercialLineUpdateAction>
		{
		public:
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SHORT_NAME;
			static const std::string PARAMETER_LONG_NAME;
			static const std::string PARAMETER_COLOR;
			static const std::string PARAMETER_STYLE;
			static const std::string PARAMETER_IMAGE;
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_RESERVATION_CONTACT_ID;
			static const std::string PARAMETER_CREATOR_ID;

		private:
			boost::shared_ptr<pt::CommercialLine> _line;
			std::string			_name;		//!< Name (code)
			std::string			_shortName;	//!< Name (cartouche)
			std::string			_longName;	//!< Name for schedule card
			boost::optional<util::RGBColor>		_color;		//!< Line color
			std::string			_style;		//!< CSS style (cartouche)
			std::string			_image;		//!< Display image (cartouche)
			boost::shared_ptr<const TransportNetwork>	_network;	//!< Network
			boost::shared_ptr<const pt::ReservationContact>	_reservationContact;	//!< Reservation contact
			std::string _creatorId;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

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
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLineUpdateAction_H__
