
/** ChronologicalServicesCollection class header.
	@file ChronologicalServicesCollection.hpp

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

#ifndef SYNTHESE_graph_ChronologicalServicesCollection_hpp__
#define SYNTHESE_graph_ChronologicalServicesCollection_hpp__

#include <set>

namespace synthese
{
	namespace graph
	{
		class Service;

		struct cmpService
		{
		    bool operator() (const Service* s1, const Service* s2) const;
		};

		typedef std::set<Service*, cmpService> ServiceSet;

		/** ChronologicalServicesCollection class.
			@ingroup m18
		*/
		class ChronologicalServicesCollection
		{
		private:
			ServiceSet _services;

		public:
			ChronologicalServicesCollection();

			ServiceSet&	getServices() { return _services; }
			const ServiceSet& getServices() const { return _services; }

			bool isCompatible(
				const graph::Service& service
			) const;
		};
	}
}

#endif // SYNTHESE_graph_ChronologicalServicesCollection_hpp__
