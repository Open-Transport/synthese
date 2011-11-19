
/** GraphModule class header.
	@file GraphModule.h
	@author Hugues
	@date 2010

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

#ifndef SYNTHESE_GraphModule_H__
#define SYNTHESE_GraphModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 18 Graph Module namespace.
	/// @author Hugues Romain
	/// @ingroup m18
	namespace graph
	{
		/**	@defgroup m18Actions 18.15 Actions
			@ingroup m18

			@defgroup m18Pages 18.11 Pages
			@ingroup m18

			@defgroup m18Functions 18.15 Functions
			@ingroup m18

			@defgroup m18Exceptions 18.01 Exceptions
			@ingroup m18

			@defgroup m18Alarm 18.17 Messages recipient
			@ingroup m18

			@defgroup m18LS 18.10 Table synchronizers
			@ingroup m18

			@defgroup m18Admin 18.14 Administration pages
			@ingroup m18

			@defgroup m18Rights 18.12 Rights
			@ingroup m18

			@defgroup m18Logs 18.13 DB Logs
			@ingroup m18

			@defgroup m18 18 Graph
			@ingroup m1

			The generic graph module is a collection of interfaces designed to implement
			an extended concept of graph, that can be used by algorithms.

			The UML diagram shows the conceptual model behind the model :

			@image html uml_generic_graph.png

			@image html topology_example.png

			The main classes consituting the graph topology are :
			<ul>
			<li>@ref Hub : Group of points defining the policy of transfer between them</li>
			<li>@ref HubPoint : Point that can be present in a hub</li>
			<li>@ref Vertex : Point that can be present in a hub and be linked to another by an edge</li>
			<li>@ref Edge : Oriented link between two vertices</li>
			<li>@ref Path : Serie of edges served successively by a service</li>
			<li>@ref PathClass : Categories of path that can be filtered on</li>
			<li>@ref PathGroup : Group of path used to share several properties</li>
			<li>@ref Service : Definition of a possible use of a path</li>
			</ul>

			The accessibility of the graph is handled by :
			<ul>
			<li>@ref UseRule : Rules to respect for using a service</li>
			<li>@ref RuleUser : Parent of each class that is allowed to define use rules</li>
			<li>@ref AccessParameters : List of accessibility parameters to compare with the rules</li>
			</ul>

			The following classes can be used to implement graph uses (result of algorithms) :
			<ul>
			<li>@ref ServicePointer : Use of a service specified by a day and departure and arrival vertices</li>
			<li>@ref Journey : Serie of service uses</li>
			<li>@ref VertexAccessMap : List of vertices with access times</li>
			</ul>

			@{
		*/

		//////////////////////////////////////////////////////////////////////////
		/// 18 Graph Module class.
		///	@author Hugues Romain
		class GraphModule : public util::ModuleClass
		{
		private:

		public:
			/** Initialization of the 18 Graph module after the automatic database loads.
				@author Hugues
				@date 2010
			*/
			void initialize();

			// static Object::Registry& getObjects();
		};

		/** @} */
	}
}

#endif // SYNTHESE_GraphModule_H__
