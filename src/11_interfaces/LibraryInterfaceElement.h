
/** LibraryInterfaceElement class header.
	@file LibraryInterfaceElement.h

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

#ifndef SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H

#include<boost/shared_ptr.hpp>

#include "FactoryBase.h"
#include "11_interfaces/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		class InterfacePage;
		class ValueElementList;

		/** @defgroup refLibrary Library Interface Elements.
			@ingroup refInt
		*/

		/** element which can produce a single display.
			@ingroup m11
		*/
		class LibraryInterfaceElement
		:	public util::FactoryBase<LibraryInterfaceElement>
		{
		public:
			/// Optional arguments for factory.
			struct Args
			{

			};

		protected:
			const InterfacePage*	_page;
			std::string				_label;

			virtual void storeParameters(ValueElementList& vel) = 0;

			friend class ValueElementList;

		public:
			void setPage(const InterfacePage* page);

			virtual std::string getLabel() const { return std::string(); }
			
			void parse( const std::string& text );

			/** Library page element creation.
				@param text Text to parse
				@param page Page to link
				@return Pointer to the created element. If the line is empty, then a null pointer is returned.
				@exception InterfacePageException if the specified key was not found in the library
				@author Hugues Romain
				@date 2007
			*/
			static boost::shared_ptr<interfaces::LibraryInterfaceElement> create(
				const std::string & text
				, const InterfacePage* page
			);

			template<class T>
			boost::shared_ptr<interfaces::LibraryInterfaceElement> copy( const ParametersVector& parameters )
			{
				return boost::shared_ptr<T>(new T(*((T*) this)));
			}


			/** Virtual display method.
				This method must be implemented in each subclass. It defines the behavior of the interface library element in three ways :
					- it can write outputs on the stream : parameter stream
					- it can write (and read) local variables : parameter variables
					- it can return a label to go after the display
				
				@param stream Stream to write the output on
				@param parameters Execution parameters
				@param variables Execution variables
				@param object Object pointer to read
				@param request The source request (read only)
				@return Label to go after the display
			*/
			virtual std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const = 0;

			const std::string getValue(
				const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;

			bool isFalse(
				const ParametersVector&
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif
