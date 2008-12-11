
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

#include "Env.h"

#include <iostream>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace impex
	{

		//////////////////////////////////////////////////////////////////////////
		/// Trident/Chouette export.
		/// @ingroup m16
		//////////////////////////////////////////////////////////////////////////
		class TridentExport
		{
		public:

		    
		private:
			util::Env				_env;
			util::RegistryKeyType	_commercialLineId;
			const bool				_withTisseoExtension;
		    
		public:

			TridentExport(
				util::Env& env,
				util::RegistryKeyType lineId,
				bool withTisseoExtension
			);
			~TridentExport();

			/** -> ChouettePTNetwork
			 */
			void run(std::ostream& os);

		private:
		    
			static std::string TridentId (const std::string& peer, const std::string clazz, const uid& id);
			static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
			static std::string TridentId (const std::string& peer, const std::string clazz, const util::Registrable& obj);
		};
	}
}

#endif
