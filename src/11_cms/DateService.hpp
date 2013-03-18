
//////////////////////////////////////////////////////////////////////////////////////////
/// DateService class header.
///	@file DateService.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_DateService_H__
#define SYNTHESE_DateService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		///	36.15 Function : DateService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Date
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m36Functions refFunctions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		class DateService:
			public util::FactorableTemplate<server::Function,DateService>
		{
		public:
			static const std::string PARAMETER_OFFSET;
			static const std::string PARAMETER_WITH_TIME;
			static const std::string PARAMETER_BASE;
			static const std::string PARAMETER_UNIT;
			static const std::string PARAMETER_STRFTIME_FORMAT;
			static const std::string PARAMETER_LANG;

		protected:
			//! \name Page parameters
			//@{
				enum OffsetUnit {SECOND, MINUTE, HOUR, DAY};
				boost::posix_time::ptime _time;
				std::string _offset;
				bool _withTime;
				OffsetUnit _unit;
				std::string _strftimeFormat;
				std::string _strftimeLang;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Date#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Date#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			DateService();

			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2011
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2011
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2011
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DateService_H__
