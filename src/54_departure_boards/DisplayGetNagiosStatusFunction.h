////////////////////////////////////////////////////////////////////////////////
/// DisplayGetNagiosStatusFunction class header.
///	@file DisplayGetNagiosStatusFunction.h
///	@author Hugues Romain
///	@date 2008-12-15 0:01
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayGetNagiosStatusFunction_H__
#define SYNTHESE_DisplayGetNagiosStatusFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;

		////////////////////////////////////////////////////////////////////////
		///	54.15 Service : Nagios monitoring informations about a departure board.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_boards_Nagios_connector
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		/// @date 2008
		/// @ingroup m54Functions refFunctions
		class DisplayGetNagiosStatusFunction:
			public util::FactorableTemplate<server::Function,DisplayGetNagiosStatusFunction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreen>	_screen;
			//@}


			////////////////////////////////////////////////////////////////////
			///	Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_boards_Nagios_connector#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_boards_Nagios_connector#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			void _setFromParametersMap(const util::ParametersMap& map);


		public:
			////////////////////////////////////////////////////////////////////
			/// Output generation.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_boards_Nagios_connector#Response
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write the output on
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			////////////////////////////////////////////////////////////////////
			///	Defines the security policy of the action.
			///	@author Hugues Romain
			///	@date 2008
			/// At the time, the function can be used without restrictions.
			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DisplayGetNagiosStatusFunction_H__
