
/** DisplayScreenSupervisionFunction class header.
	@file DisplayScreenSupervisionFunction.h

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

#ifndef SYNTHESE_DisplayScreenSupervisionRequest_H__
#define SYNTHESE_DisplayScreenSupervisionRequest_H__

#include "DBLogEntry.h"
#include "Function.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		/** DisplayScreenSupervisionFunction class.
			@ingroup m54Functions refFunctions
		*/
		class DisplayScreenSupervisionFunction:
			public util::FactorableTemplate<server::Function,DisplayScreenSupervisionFunction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN_ID;
			static const std::string PARAMETER_STATUS;

		private:
			static const std::string DATA_TEXT;


			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreen>	_displayScreen;
				std::string								_text;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);


			//////////////////////////////////////////////////////////////////////////
			/// Launches the parsing.
			/// Parameters sent to the CMS :
			///	<dl>
			///	<dt>text</dt><dd>text to parse</dd>
			/// </dl>
			///
			///	The CMS page is supposed to return a standardized string composed
			/// by fields separated by |.
			///
			///	The fields must be ordered like this :
			///		-# general OK : the requested display has been done
			///		-# memory OK
			///		-# clock OK
			///		-# EEPROM OK
			///		-# temperature sensor OK
			///		-# light OK
			///		-# light detail
			///		-# display OK
			///		-# display detail
			///		-# sound OK
			///		-# temperature OK
			///		-# temperature warning
			///		-# temperature value
			///		-# communication OK
			///
			///	The OK fields are tribools :
			///		- 1 = true
			///		- 0 = false
			///		- -1 = unknown (i.e. the monitored display cannot control the ckeck point)
			///
			///	Example (verify it):
			///	@code {{parse_display_return 0230310620202020202020202020202003}}@endcode
			///	outputs :
			///	@code 1|1|0|1|0|1|0000|1|0000 @endcode
			//////////////////////////////////////////////////////////////////////////
			/// @param stream stream to write the result on
			/// @param request request which has called the display
			/// @param page CMS template to use to display each destination of each destination of each service
			/// @param textToParse text to parse
			void _display(
				std::ostream& stream,
				const server::Request& request,
				boost::shared_ptr<const cms::Webpage> page,
				const std::string& textToParse
			) const;

		public:
			DisplayScreenSupervisionFunction();

			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DisplayScreenSupervisionRequest_H__
