
/** PTModule class header.
	@file PTModule.h

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

#ifndef SYNTHESE_PTModule_H__
#define SYNTHESE_PTModule_H__


#include "GraphModuleTemplate.h"
#include "12_security/Types.h"
#include "Registry.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace graph
	{
		class Journey;
	}

	/**	@defgroup m35Actions 35 Actions
		@ingroup m35

		@defgroup m35Pages 35 Pages
		@ingroup m35

		@defgroup m35Library 35 Interface library
		@ingroup m35

		@defgroup m35Functions 35 Functions
		@ingroup m35

		@defgroup m35LS 35 Table synchronizers
		@ingroup m35

		@defgroup m35Admin 35 Administration pages
		@ingroup m35

		@defgroup m35Rights 35 Rights
		@ingroup m35

		@defgroup m35Logs 35 DB Logs
		@ingroup m35

		@defgroup m35File 35.16 File formats
		@ingroup m35

		@defgroup m35 35 Transport
		@ingroup m3

		The pt module contains the graph of public transport lines and all associated objects.

		@image hml uml_pt_graph.png

		The pt graph is implemented by :
		<table>
		<tr><th>graph interface</th><th>road implementation</th></tr>
		<tr><td>Service</td><td>ScheduledService and ContinuousService</td></tr>
		<tr><td>PathGroup</td><td>CommercialLine</td></tr>
		<tr><td>Path</td><td>Line</td></tr>
		<tr><td>Edge</td><td>LineStop</td></tr>
		<tr><td>Vertex</td><td>PhysicalStop</td></tr>
		<tr><td>Hub</td><td>PublicTransportStopZoneConnectionPlace</td></tr>
		</table>

		A specific graph is defined by the junctions (links between stops qualified for use
		inside a transfer) :

		@image hml uml_junction_graph.png

		The junction graph is implemented by :
		<table>
		<tr><th>graph interface</th><th>road implementation</th></tr>
		<tr><td>Service</td><td>PermanentService</td></tr>
		<tr><td>PathGroup</td><td>none</td></tr>
		<tr><td>Path</td><td>Junction</td></tr>
		<tr><td>Edge</td><td>JunctionStop</td></tr>
		<tr><td>Vertex</td><td>PhysicalStop</td></tr>
		<tr><td>Hub</td><td>PublicTransportStopZoneConnectionPlace</td></tr>
		</table>

	@{
	*/

	/** 35 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace pt
	{
		/** 35 Transport network module class.
		*/
		class PTModule:
			public graph::GraphModuleTemplate<PTModule>
		{
		public:
			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

			static void RTDataCleaner();

			//////////////////////////////////////////////////////////////////////////
			/// Gets the labels of each PT use rule, including undefined value.
			/// @return The use rule labels
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			//////////////////////////////////////////////////////////////////////////
			/// Alphabetical order.
			static Labels GetPTUseRuleLabels();

			//////////////////////////////////////////////////////////////////////////
			/// Gets the labels of each PT use rule, including undefined value.
			/// @return The use rule labels
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			//////////////////////////////////////////////////////////////////////////
			/// Alphabetical order.
			static Labels getCommercialLineLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel 
				, bool withAll=false
			);

			static void getNetworkLinePlaceRightParameterList(security::ParameterLabelsVector& m);

			static int GetMaxAlarmLevel(
				const graph::Journey&
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
