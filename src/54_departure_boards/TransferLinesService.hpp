
//////////////////////////////////////////////////////////////////////////////////////////
///	TransferLinesService class header.
///	@file TransferLinesService.hpp
///	@author User
///	@date 2012
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

#ifndef SYNTHESE_TransferLinesService_H__
#define SYNTHESE_TransferLinesService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
		class StopArea;
	}

	namespace departure_boards
	{
		//////////////////////////////////////////////////////////////////////////
		///	54.15 Function : TransferLinesService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m54Functions refFunctions
		///	@author User
		///	@date 2012
		/// @since 3.5.0
		class TransferLinesService:
			public util::FactorableTemplate<server::Function,TransferLinesService>
		{
		public:
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_STOP_AREA_ID;
			static const std::string PARAMETER_TIME;
			static const std::string PARAMETER_DURATION;

			static const std::string TAG_LINE;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine> _line;
				boost::shared_ptr<const pt::StopArea> _stopArea;
				boost::posix_time::ptime _time;
				boost::posix_time::time_duration _duration;
			//@}

			typedef std::set<const pt::CommercialLine*> LinesSet;
			LinesSet _getLinesCallingAtStop(const pt::StopArea& stopAera) const;
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author User
			/// @date 2012
			/// @since 3.5.0
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author User
			/// @date 2012
			/// @since 3.5.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author User
			/// @date 2012
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author User
			/// @date 2012
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author User
			/// @date 2012
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_TransferLinesService_H__
