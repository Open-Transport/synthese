////////////////////////////////////////////////////////////////////////////////
/// InterfacePage class header.
///	@file InterfacePage.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#ifndef SYNTHESE_INTERFACES_PAGE_H
#define SYNTHESE_INTERFACES_PAGE_H

#include <vector>
#include <utility>
#include <string>

#include "Registrable.h"
#include "Registry.h"
#include "FactoryBase.h"
#include "11_interfaces/Types.h"
#include "LibraryInterfaceElement.h"

/** @defgroup refPages Interface Pages
	@ingroup refInt
*/

namespace synthese
{
	namespace interfaces
	{
		class Interface;
		class SimplePageFunction;
		class IncludePageInterfaceElement;
		class RequestWithInterfaceAndRequiredSession;

		////////////////////////////////////////////////////////////////////
		/// Definition of a page, coming from the database. Page are elements of an interface.
		///
		///	Two types of pages can be defined :
		///		- standard pages : the page is registered in the Factory<InterfacePage>. Its key in database corresponds to the key of registration.
		///		- additional pages : the page is not registerd in the Factory<InterfacePage>. It is saved directly as a InterfacePage object.
		///	@ingroup m11
		class InterfacePage
		:	public util::FactoryBase<InterfacePage>,
			public virtual util::Registrable
		{
			friend class SimplePageFunction;
			friend class IncludePageInterfaceElement;
			friend class RequestWithInterfaceAndRequiredSession;

		public:
			typedef std::vector<boost::shared_ptr<LibraryInterfaceElement> >	Components;

			/// Chosen registry class.
			typedef util::Registry<InterfacePage>	Registry;
			typedef InterfacePage FactoryClass;

		private:
			//! @name Database attributes
			//@{
				std::string			_pageCode;
				std::string			_mimeType;
				const Interface*	_interface;
				bool				_directDisplayAllowed;
				std::string			_source;
			//@}

			//! @name Run attributes
			//@{
				Components			_components;
			//@}

		protected:
			////////////////////////////////////////////////////////////////////
			/// Generic display method.
			/// @param stream Stream to write the output in
			/// @param parameters Parameters vector
			/// @return Name of the next line to display (empty = next line)
			void _display(
				std::ostream& stream,
				const ParametersVector& parameters,
				VariablesMap& variables,
				const void* object = NULL,
				const server::Request* request = NULL
			) const;


		public:
			void parse();

			InterfacePage(
				util::RegistryKeyType key = 0
			);
			virtual ~InterfacePage();

			void clear();

			/// @name Setters
			//@{
				void				setInterface(const Interface*);
				void				setPageCode(const std::string& code);
				void				setDirectDisplayAllowed(bool value);
				void				setMimeType(const std::string& value);
				void				setSource(const std::string& value);
			//@}

			/// @name Getters
			//@{
				const Interface*	getInterface()				const;
				const std::string&	getPageCode()				const;
				bool				getDirectDisplayAllowed()	const;
				const std::string&	getMimeType()				const;
				const std::string&	getSource()					const;
			//@}

			std::string getValue(
				const ParametersVector& parameters
				, VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;

			std::string _fakeFactoryKey;
			virtual const std::string& getFactoryKey() const { return _fakeFactoryKey; }
			InterfacePage* clone() const { return NULL; }

		};
	}
}

#endif
