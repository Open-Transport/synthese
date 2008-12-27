////////////////////////////////////////////////////////////////////////////////
/// DisplayGetNagiosStatusFunction class header.
///	@file DisplayGetNagiosStatusFunction.h
///	@author Hugues Romain
///	@date 2008-12-15 0:01
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayGetNagiosStatusFunction_H__
#define SYNTHESE_DisplayGetNagiosStatusFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		////////////////////////////////////////////////////////////////////////
		///	DisplayGetNagiosStatusFunction public function class.
		/// @author Hugues Romain
		/// @date 2008
		/// @ingroup m54Functions refFunctions
		/// 
		/// This function displays monitoring informations about a specified
		/// display screen, according to the SYNTHESE nagios plugin format :
		///		- at the first line : the returning code of the nagios plugin :
		///		- from the second line : the nagios plugin standard output
		///			according to the Nagios norm (see 
		///			http://nagios.sourceforge.net/docs/3_0/pluginapi.html)
		///
		/// The Nagios output plug-in return code can be :
		///		- 0 = OK
		///		- 1 = Warning
		///		- 2 = Error
		///		- 3 = Check deactivated
		///
		/// The value is computed from the monitoring current status :
		///		(fill in here)
		///
		/// The Nagios plugin standard output follows this format :
		///	@codeTEXT OUTPUT | TEMPERATURE VALUE
		/// ERRORS DETAILS |
		/// @endcode
		///
		class DisplayGetNagiosStatusFunction
		:	public util::FactorableTemplate<server::Function,DisplayGetNagiosStatusFunction>
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
			///	@return Generated parameters map
			server::ParametersMap _getParametersMap() const;
			
			
			
			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			void _setFromParametersMap(const server::ParametersMap& map);
			
			
		public:
			////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			///	@param stream Stream to write the output on
			void _run(std::ostream& stream) const;

			
			
			////////////////////////////////////////////////////////////////////
			///	Defines the security policy of the action.
			///	@author Hugues Romain
			///	@date 2008
			/// At the time, the function can be used without restrictions.
			virtual void _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_DisplayGetNagiosStatusFunction_H__
