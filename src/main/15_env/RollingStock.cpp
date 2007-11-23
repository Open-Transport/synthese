
/** RollingStock class implementation.
	@file RollingStock.cpp

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

#include "RollingStock.h"

namespace synthese
{

	namespace util
	{
		template<> typename Registrable<uid,env::RollingStock>::Registry Registrable<uid,env::RollingStock>::_registry;
	}

	namespace env
	{


		RollingStock::RollingStock ()
			: synthese::util::Registrable<uid,RollingStock> ()
			
		{
		}



		RollingStock::~RollingStock()
		{

		}


		const std::string& 
		RollingStock::getName() const
		{
			return _name;
		}


		const std::string& 
		RollingStock::getArticle() const
		{
			return _article;
		}



		const std::string& 
		RollingStock::getIndicator() const
		{
			return _indicator;
		}

		void RollingStock::setName( const std::string& value )
		{
			_name = value;
		}

		void RollingStock::setArticle( const std::string& value )
		{
			_article = value;
		}

		void RollingStock::setIndicator( const std::string& value )
		{
			_indicator = value;
		}
	}
}
