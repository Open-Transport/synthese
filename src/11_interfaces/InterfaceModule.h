
/** InterfaceModule class header.
	@file InterfaceModule.h

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

#ifndef SYNTHESE_InterfaceModule_H__
#define SYNTHESE_InterfaceModule_H__

#include <vector>
#include <utility>
#include <boost/foreach.hpp>

#include "ValueElementList.h"
#include "StaticValueInterfaceElement.h"
#include "ModuleClassTemplate.hpp"

#include "11_interfaces/Types.h"

namespace synthese
{
	/** @defgroup refInt Interface Reference
		@ingroup ref

		@defgroup m11Pages Interface Pages
		@ingroup m11

		@defgroup m11Library Interface Library
		@ingroup m11

		@defgroup m11LS Table sync
		@ingroup m11

		@defgroup m11Functions Functions
		@ingroup m11

		@defgroup m11Exceptions Exceptions
		@ingroup m11

		@defgroup m11 11 Interface
		@ingroup m1

		Classes of the interface module :
			- Interface class : defines an interface entirely : each defined pages is stored in a map page => definition
			- DisplayableElement (abstract) : specifies virtual methods for all types of output element, making up each interface page
			- Page : definition of a page, coming from the database. Page are elements of an interface
			- LibraryInterfaceElement (abstract) : element which produce only a display (cannot be used as value producer because of the complexity of the output)
			- ValueElement (abstract) : element which contains a string value, which can be displayed or used by another element (eg "if" element)
			- StaticValueInterfaceElement : the value is defined at the load of the interface
			- DynamicValueInterfaceElement (abstract) : the value is defined at the display of the containing page
			- Parameter : a selected parameter of the page will be displayed
			- IfThenElseInterfaceElement : element which decides his behavior according to a value element
			- GotoInterfaceElement : branching element : the next element in page display is changed to then specified named line
			- PageLineName : name of a line
			- PageInclusion : inclusion of an other customized page, with parameters
			- ... other library elements in other modules

		Several complex LibraryElement objects follow a structured template. Subclasses of LibraryElement specify the frame of these elements :
			- AdminInterfaceElement (abstract) : administration page, with right control of content, navigation pane at the left, and title bar with navigation positon.
			- UpdatesLogAdminInterfaceElement (abstract) : administration page which contains a log of the administrative modifications done in the database

		Each page compound is used in the page definition using a text code, which is stored statically in each corresponding class. 		When a command needs precision to specify the subclass to use or then index of the parameter, then the code follows the command.

		<table>
		<tr><th>Element class</th><th>Command</th><th>Parameters</th></tr>
		<tr><td>LibraryInterfaceElement</td><td>O[name]</td><td>Depending on the subclass (see doc)</td></tr>
		<tr><td>StaticValueInterfaceElement</td><td>T</td><td>The text</td></tr>
		<tr><td>DynamicValueInterfaceElement</td><td>A[name]</td><td>Depending on the subclass (see doc)</td></tr>
		<tr><td>Parameter</td><td>P[index]</td><td>No parameter</td></tr>
		<tr><td>IfThenElseElement</td><td>?</td><td>Value to test, action to do if test is ok, action to do else</tr>
		<tr><td>GotoInterfaceElement</td><td>G</td><td>Name of line to go</td></tr>
		<tr><td>PageLineName</td><td>L</td><td>Name of the line</td></tr>
		<tr><td>PageInclusion</td><td>>[name]</td><td>Depending on the page (see doc)</td></tr>
		</table>

		The code defined page components are :
		- LibraryInterfaceElement : complex screen part, which cannot be customized by other ways than parameters
		- DynamicValueInterfaceElement : single value, which is most of time directly read in the synthese environment

		Their implementation are provided by subclasses of the component class, which are defined in the corresponding modules, in a dedicated subdirectory called by the name of the mother class.
		These subclasses are registered by a static code in order to be selected dynamically by the display method of the caller page.
		@todo More documentation to do about this mechanism. Do a util class "FactorableClass".
		Use this mechanism for the loaders too.

		The available page codes are defined by module too. Static string constants named INTERFACE_PAGE_[name] are defined in the module.h file of each module.

		@{
	*/

	/** 11 interface module namespace.
		The interface keyword is reserved, so the namespace is named interfaces.
	*/
	namespace interfaces
	{
		/** Interface module class.
		*/
		class InterfaceModule:
			public server::ModuleClassTemplate<InterfaceModule>
		{
		public:
			static std::string getVariableFromMap(const VariablesMap& variables, const std::string& varName);






			//////////////////////////////////////////////////////////////////////////
			/// Gets directly a display from a subclass, without mounting the object into an interface.
			/// @param stream stream to write the display on
			/// @param parameters list of strings equivalent from the interface definition code. It can be only text, there will be no interpretation : it is not execution parameters.
			///	@param request The source request (read only)
			template<class InterfaceElement>
			static void SimpleDisplay(
				std::ostream& stream,
				std::vector<std::string> parameters,
				const server::Request* request
			){
				ValueElementList vel;
				BOOST_FOREACH(const std::string& parameter, parameters)
				{
					vel.push_back(boost::shared_ptr<StaticValueInterfaceElement>(new StaticValueInterfaceElement(parameter)));
				}
				boost::shared_ptr<InterfaceElement> element(new InterfaceElement);
				element->storeParameters(vel);
				ParametersVector params;
				VariablesMap vars;
				element->display(stream, params, vars, request);
			}
		};
	}
	/** @} */
}

#endif // SYNTHESE_InterfaceModule_H__

