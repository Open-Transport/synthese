////////////////////////////////////////////////////////////////////////////////
/// StopAreasListFunction class header.
///	@file StopAreasListFunction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_StopAreasListFunction_H__
#define SYNTHESE_StopAreasListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/shared_ptr.hpp>
#include <geos/geom/Envelope.h>

namespace synthese
{
	class CoordinatesSystem;

	namespace pt
	{
		class CommercialLine;

		////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Stop Area list public Function class.
		/// @author Xavier Raffin
		///	@date 2008
		///	@ingroup m35Functions refFunctions
		//////////////////////////////////////////////////////////////////////////
		/// <h2>Usage</h2>
		///	Key : StopAreasListFunction
		///
		///	Parameters :
		///	<ul>
		///		<li>fonction=StopAreasListFunction</li>
		///		<li>roid : id of the line of which stop areas will be displayed</li>
		///	</ul>
		///
		/// <h2>Standard XML output</h2>
		///	<h3>Description</h3>
		///
		///	@image html StopAreasListFunction.png
		///
		///	<h3>Download</h3>
		///
		///	<ul>
		///		<li><a href="include/35_pt/StopAreasListFunction.xsd">XML output schema</a></li>
		///	</ul>
		class StopAreasListFunction:
			public util::FactorableTemplate<server::Function,StopAreasListFunction>
		{
		public:
			static const std::string PARAMETER_BBOX;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_OUTPUT_LINES;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine> _commercialLine;
				boost::optional<geos::geom::Envelope> _bbox;
				const CoordinatesSystem* _coordinatesSystem;
				bool _outputLines;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			StopAreasListFunction();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_StopAreasListFunction_H__
