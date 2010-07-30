
/** Website class header.
	@file Website.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_cms_Website_hpp__
#define SYNTHESE_cms_Website_hpp__

#include "Named.h"
#include "Registry.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// Website class.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.2.0
		class Website:
			public util::Named
		{
		private:
			//! \name Properties
			//@{
				boost::gregorian::date		_startValidityDate;
				boost::gregorian::date		_endValidityDate;
				std::string					_clientURL;
				Webpage*					_defaultTemplate;
			//@}

		public:
			Website():
			  _startValidityDate(boost::gregorian::not_a_date_time),
			  _endValidityDate(boost::gregorian::not_a_date_time),
			  _defaultTemplate(NULL)
			{}

			//! @name Setters
			//@{
				void setStartDate ( const boost::gregorian::date& value){ _startValidityDate = value; }
				void setEndDate ( const boost::gregorian::date& value){ _endValidityDate = value; }
				void setClientURL(const std::string& value) { _clientURL = value; }
				void setDefaultTemplate(Webpage* value){ _defaultTemplate = value; }
			//@}

			//! @name Getters
			//@{
				const boost::gregorian::date& getStartDate() const { return _startValidityDate; }
				const boost::gregorian::date& getEndDate() const { return _endValidityDate; }
				const std::string& getClientURL() const { return _clientURL; }
				Webpage* getDefaultTemplate() const { return _defaultTemplate; }
			//@}
			
			//! @name Services
			//@{
				virtual util::RegistryKeyType getKey() const = 0;
			//@}
		};
	}
}

#endif // SYNTHESE_cms_Website_hpp__
