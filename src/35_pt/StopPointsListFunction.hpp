
//////////////////////////////////////////////////////////////////////////////////////////
/// StopPointsListFunction class header.
///	@file StopPointsListFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PTPhysicalStopsListFunction_H__
#define SYNTHESE_PTPhysicalStopsListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		///	35.15 Function : Physical Stops Search.
		///	@ingroup m35Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		//////////////////////////////////////////////////////////////////////////
		/// Key : StopPointsListFunction
		///
		/// <h2>Usage</h2>
		///	<h3>Search by id</h3>
		///
		/// Parameters :
		///	<ul>
		///		<li>fonction=StopPointsListFunction</li>
		///		<li>roid : id of the physical stop to return</li>
		///		<li>p : id of the template to use for the display of each stop (will be used by PTPhysicalStopInterfacePage).
		///			If not defined, the output respects the standard XML format defined below.</li>
		///		<li>o : output format [xml|csv] (ignored if p is defined, default is xml)</li>
		///	</ul>
		///
		/// <h3>Search by operator code</h3>
		///
		/// The comparison between code and entered text is done by LIKE operator.
		/// 
		/// <ul>
		///		<li>fonction=StopPointsListFunction</li>
		///		<li>code : operator code</li>
		///		<li>n : max number of stops to return</li>
		///		<li>p : id of the template to use for the display of each stop (will be used by PTPhysicalStopInterfacePage).
		///			If not defined, the output respects the standard XML format defined below.</li>
		///		<li>o : output format [xml|csv] (ignored if p is defined, default is xml)</li>
		///	</ul>
		///
		/// <h3>Search by route</h3>
		///
		///	<ul>
		///		<li>fonction=StopPointsListFunction</li>
		///		<li>roid : id of the route to read</li>
		///		<li>p : id of the template to use for the display of each stop (will be used by PTPhysicalStopInterfacePage).
		///			If not defined, the output respects the standard XML format defined below.</li>
		///		<li>o : output format [xml|csv] (ignored if p is defined, default is xml)</li>
		///	</ul>
		/// 
		/// <h3>Search by commercial stop area</h3>
		///
		///	<ul>
		///		<li>fonction=StopPointsListFunction</li>
		///		<li>roid : id of the commercial stop area to read</li>
		///		<li>ni (optional) : id of a transport network which must serve the stop</li>
		///		<li>p : id of the template to use for the display of each stop (will be used by PTPhysicalStopInterfacePage).
		///			If not defined, the output respects the standard XML format defined below.</li>
		///		<li>o : output format [xml|csv] (ignored if p is defined, default is xml)</li>
		///	</ul>
		///
		/// <h3>Search by coordinates (bounding box)</h3>
		///	<ul>
		///		<li>fonction=StopPointsListFunction</li>
		///		<li>bbox : x1,y1,x2,y2 : bounding box (lambert II Etendu projection)</li>
		///		<li>ni (optional) : id of a transport network which must serve the stop</li>
		///		<li>p : id of the template to use for the display of each stop (will be used by PTPhysicalStopInterfacePage).
		///			If not defined, the output respects the standard XML format defined below.</li>
		///		<li>o : output format [xml|csv] (ignored if p is defined, default is xml)</li>
		///	</ul>
		///
		///
		///	<h2>Standard XML Output</h2>
		///
		/// This chapter applies only if no display template is used.
		///
		///	<h3>Description</h3>
		/// @image html StopPointsListFunction.png
		///
		///	<h3>Download</h3>
		///
		///	<ul>
		///		<li><a href="include/35_pt/StopPointsListFunction.xsd">XML output schema</a></li>
		///	</ul>
		class StopPointsListFunction:
			public util::FactorableTemplate<server::Function,StopPointsListFunction>
		{
		public:
			/// @todo request parameter names declaration
			// eg : static const std::string PARAMETER_xxx;
			
		protected:
			//! \name Page parameters
			//@{
				/// @todo Parsed parameters declaration
				// eg : const void*	_object;
				// eg : ParametersMap			_parameters;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_PTPhysicalStopsListFunction_H__
