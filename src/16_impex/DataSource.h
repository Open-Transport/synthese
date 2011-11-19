////////////////////////////////////////////////////////////////////////////////
///	DataSource class header.
///	@file DataSource.h
///	@author Hugues Romain (RCS)
///	@date sam fév 21 2009
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

#ifndef SYNTHESE_DataSource_h__
#define SYNTHESE_DataSource_h__

#include "Registrable.h"
#include "Registry.h"

#include <string>
#include <boost/foreach.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace util
	{
		class Env;
	}

	namespace impex
	{
		class Importer;
		class Importable;

		////////////////////////////////////////////////////////////////////////
		/// DataSource class.
		/// @ingroup m16
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// A data source identifies where an object come from.
		/// Two types of data integration are available :
		///	<ul>
		///		<li>Manually data typing</li>
		///		<li>Automated import </li>
		///	</ul>
		///
		/// Sources with automated import refer to a FileFormat instance.
		///
		class DataSource:
			public virtual util::Registrable
		{
		public:
			typedef util::Registry<DataSource> Registry;
			
			// Typedefs
			typedef std::map<
				std::string, // Class name
				std::map<
					std::string, // Code
					Importable* // Object
			>	> Links;


		private:
			// Attributes
			std::string	_name;
			std::string _format;
			std::string _icon;
			std::string	_charset;
			const CoordinatesSystem* _coordinatesSystem;
			mutable Links _links;

		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			DataSource(
				util::RegistryKeyType id = 0
			);

			//! @name Getters
			//@{
				const std::string& getName() const { return _name; }
				const std::string& getFormat() const { return _format; }
				const std::string& getIcon() const { return _icon; }
				const std::string& getCharset() const { return _charset; }
				const CoordinatesSystem* getCoordinatesSystem() const { return _coordinatesSystem; }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& value) { _name = value;	}
				void setFormat(const std::string& value) { _format = value; }
				void setIcon(const std::string& value) { _icon = value; }
				void setCharset(const std::string& value) { _charset = value; }
				void setCoordinatesSystem(const CoordinatesSystem* value){ _coordinatesSystem = value; }
			//@}

			//! @name Modifiers
			//@{
				template<class T>
				void addLinks(T& object) const
				{
					BOOST_FOREACH(const std::string& code, object.getCodesBySource(*this))
					{
						addLink(object, code);
					}
				}

				template<class T>
				void addLink(T& object, const std::string& code) const
				{
					_links[T::Registry::KEY].insert(
						std::make_pair(code, static_cast<Importable*>(&object))
					);
				}

				template<class T>
				void removeLinks(T& object) const
				{
					std::vector<std::string> codes(object.getCodesBySource(*this));
					BOOST_FOREACH(const std::string& code, codes)
					{
						removeLink(object, code);
					}
				}

				template<class T>
				void removeLink(T& object, const std::string& code) const
				{
					Links::iterator it(_links.find(T::Registry::KEY));
					if(it != _links.end())
					{
						_links.erase(code);
					}
				}
			//@}

			//! @name Queries
			//@{
				template<class T>
				T* getObjectByCode(const std::string& code) const
				{
					Links::const_iterator it(_links.find(T::Registry::KEY));
					if(it == _links.end())
					{
						return NULL;
					}
					Links::mapped_type::const_iterator it2(it->second.find(code));
					if(it2 == it->second.end())
					{
						return NULL;
					}
					else
					{
						return dynamic_cast<T*>(it2->second);
					}
				}

				boost::shared_ptr<Importer> getImporter(util::Env& env) const;
				bool canImport() const;
				const CoordinatesSystem& getActualCoordinateSystem() const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};
}	}

#endif // SYNTHESE_DataSource_h__
