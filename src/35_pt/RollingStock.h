
/** RollingStock class header.
	@file RollingStock.h

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

#ifndef SYNTHESE_ENV_ROLLING_STOCK_H
#define SYNTHESE_ENV_ROLLING_STOCK_H

#include "Registrable.h"
#include "Registry.h"
#include "PathClass.h"
#include "Named.h"

#include <string>

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Rolling stock.
		/// TRIDENT = VehicleType
		/// @ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class RollingStock:
			public virtual util::Registrable,
			public graph::PathClass,
			public util::Named
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<RollingStock>	Registry;

		protected:
			std::string _article;
			std::string _indicator;
			std::string _tridentKey;
			bool _isTridentKeyReference;

		public:

			RollingStock(util::RegistryKeyType key = 0);

			~RollingStock();


			//! @name Getters
			//@{
				const std::string& getArticle() const { return _article; }
				const std::string& getIndicator() const { return _indicator; }
				const std::string& getTridentKey() const { return _tridentKey; }
				bool getIsTridentKeyReference() const { return _isTridentKeyReference; }
			//@}


			//! @name Setters
			//@{
				void setArticle(const std::string& value) { _article = value; }
				void setIndicator(const std::string& value) { _indicator = value; }
				void setTridentKey(const std::string& value) { _tridentKey = value; }
				void setIsTridentKeyReference(bool value) { _isTridentKeyReference = value; }
			//@}

			//! @name Services
			//@{
				virtual PathClass::Identifier getIdentifier() const;
			//@}
		};
	}
}

#endif
