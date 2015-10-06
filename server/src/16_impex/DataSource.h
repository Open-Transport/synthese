////////////////////////////////////////////////////////////////////////////////
///	DataSource class header.
///	@file DataSource.h
///	@author Hugues Romain
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

#include "Object.hpp"

#include "CoordinatesSystem.hpp"
#include "PointerField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "SchemaMacros.hpp"
#include "StringField.hpp"

#include <vector>
#include <string>
#include <boost/date_time/time_duration.hpp>
#include <boost/foreach.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace util
	{
		class Env;
	}

	FIELD_STRING(Icon)
	FIELD_STRING(Charset)

	namespace impex
	{
		class Importer;
		class Importable;

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Icon),
			FIELD(Charset),
			FIELD(CoordinatesSystem)
		> DataSourceRecord;


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
			public Object<DataSource, DataSourceRecord>
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
			typedef std::map<
				std::string,
				std::set<Importable*>
			> LinksWithoutCode;


		private:
			mutable Links _links;
			mutable LinksWithoutCode _linksWithoutCode;

		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			DataSource(
				util::RegistryKeyType id = 0
			);

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
					if(code.empty())
					{
						_linksWithoutCode[T::Registry::KEY].insert(static_cast<Importable*>(&object));
					}
					else
					{
						_links[T::Registry::KEY].insert(
							std::make_pair(code, static_cast<Importable*>(&object))
						);
					}
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
					if(code.empty())
					{
						LinksWithoutCode::iterator it(_linksWithoutCode.find(T::Registry::KEY));
						if(it != _linksWithoutCode.end())
						{
							it->second.erase(&object);
						}
					}
					else
					{
						Links::iterator it(_links.find(T::Registry::KEY));
						if(it != _links.end())
						{
							it->second.erase(code);
						}
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

				typedef std::vector<std::pair<std::string, Importable*> > LinkedObjects;

				template<class T>
				LinkedObjects getLinkedObjects() const
				{
					LinkedObjects result;

					// Adding objects with codes
					Links::const_iterator it(_links.find(T::Registry::KEY));
					if(it != _links.end())
					{
						result.insert(
							result.end(),
							it->second.begin(),
							it->second.end()
						);
					}

					// Adding objects without code
					LinksWithoutCode::const_iterator itWithoutCode(_linksWithoutCode.find(T::Registry::KEY));
					if(itWithoutCode != _linksWithoutCode.end())
					{
						BOOST_FOREACH(Importable* obj, itWithoutCode->second)
						{
							result.push_back(std::make_pair(std::string(), obj));
						}
					}

					return result;
				}

				const CoordinatesSystem& getActualCoordinateSystem() const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_DataSource_h__
