
/** tridentexport class header.
	@file tridentexport.h

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


#ifndef SYNTHESE_IMPEX_TRIDENTEXPORT_H
#define SYNTHESE_IMPEX_TRIDENTEXPORT_H

#include "FactorableTemplate.h"
#include "FileFormat.h"

#include <iostream>

#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}
	
	namespace env
	{
		class ServiceDate;
	}

	namespace pt
	{

		//////////////////////////////////////////////////////////////////////////
		/// Trident/Chouette file format.
		/// @ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class TridentFileFormat
		:	public util::FactorableTemplate<impex::FileFormat,TridentFileFormat>
		{
		public:

		    
		private:
			util::RegistryKeyType	_commercialLineId;
			const bool				_withTisseoExtension;
			std::vector<boost::shared_ptr<env::ServiceDate> > _serviceDates;
		
		protected:
				
			
			virtual void _parse(
				const std::string& text,
				std::ostream& os
			);

	
		
		public:

			TridentFileFormat(
				util::Env* env = NULL,
				util::RegistryKeyType lineId = UNKNOWN_VALUE,
				bool withTisseoExtension = false
			);
			~TridentFileFormat();

			/** -> ChouettePTNetwork
			 */
			virtual void build(std::ostream& os);
			
			virtual void save(std::ostream& os) const;

		private:
		    
			static std::string TridentId (const std::string& peer, const std::string clazz, const uid& id);
			static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
			static std::string TridentId (const std::string& peer, const std::string clazz, const util::Registrable& obj);
		};
	}
}

#endif
