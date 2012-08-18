
/** Webpage class implementation.
	@file Webpage.cpp

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

#include "Webpage.h"

#include "ParametersMap.h"
#include "DynamicRequest.h"
#include "FunctionWithSite.h"
#include "ServerModule.h"
#include "WebPageDisplayFunction.h"
#include "CMSModule.hpp"
#include "Website.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace cms;
	using namespace db;
	using namespace util;
	using namespace server;

	CLASS_DEFINITION(Webpage, "t063_web_pages", 63)
	FIELD_DEFINITION_OF_TYPE(SpecificTemplate, "specific_template_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Abstract, "abstract", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ImageURL, "image", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(WebpageLinks, "links", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DoNotUseTemplate, "do_not_use_template", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(HasForum, "has_forum", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(SmartURLPath, "smart_url_path", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SmartURLDefaultParameterName, "smart_url_default_parameter_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(RawEditor, "raw_editor", SQL_BOOLEAN)

	template<> const Field ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[] = { Field("site_id", SQL_INTEGER), Field("up_id", SQL_INTEGER), Field("rank", SQL_INTEGER), Field() };
	


	template<> void ComplexObjectField<WebpageTreeNode, WebpageTreeNode::Type>::GetLinkedObjectsIds(
		LinkedObjectsIds& list,
		const Record& record
	){
		RegistryKeyType id(record.getDefault<RegistryKeyType>(FIELDS[0].name, 0));
		if (id > 0)
		{
			list.push_back(id);
		}

		RegistryKeyType up_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
		if (up_id > 0)
		{
			list.push_back(up_id);
		}
	}



	template<> void ComplexObjectField<WebpageTreeNode, WebpageTreeNode::Type>::LoadFromRecord(
		WebpageTreeNode::Type& fieldObject,
		ObjectBase& object,
		const Record& record,
		const Env& env
	){
		assert(dynamic_cast<Webpage*>(&object));
		Webpage& webpage(static_cast<Webpage&>(object));

		if(record.isDefined(FIELDS[2].name))
		{
			webpage.setRank(record.getDefault<size_t>(FIELDS[2].name, 0));
		}

		if(record.isDefined(FIELDS[1].name))
		{
			RegistryKeyType up_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
			if(up_id > 0)
			{
				try
				{
					webpage.setParent(env.getEditable<Webpage>(up_id).get());
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in on web page " + lexical_cast<string>(webpage.get<Key>()) +" : up web page " +
						lexical_cast<string>(up_id) + " not found"
					);
				}
			}
			else
			{
				webpage.setParent(NULL);
			}
		}

		if(record.isDefined(FIELDS[0].name))
		{
			RegistryKeyType id(record.getDefault<RegistryKeyType>(FIELDS[0].name, 0));
			if(id > 0)
			{
				try
				{
					webpage.setRoot(env.getEditable<Website>(id).get());
				}
				catch(ObjectNotFoundException<Website>&)
				{
					Log::GetInstance().warn(
						"Data corrupted in on web page " + lexical_cast<string>(webpage.get<Key>()) +" : website " +
						lexical_cast<string>(id) + " not found"
					);
				}
			}
		}
	}



	template<>
	void ComplexObjectField<WebpageTreeNode, WebpageTreeNode::Type>::SaveToParametersMap(
		const WebpageTreeNode::Type& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix,
		boost::logic::tribool withFiles
	){
		if(withFiles == true)
		{
			return;
		}

		assert(dynamic_cast<const Webpage*>(&object));
		const Webpage& webpage(static_cast<const Webpage&>(object));

		map.insert(prefix + FIELDS[0].name, webpage.getRoot() ? webpage.getRoot()->getKey() : RegistryKeyType(0));
		map.insert(prefix + FIELDS[1].name, webpage.getParent(true) ? webpage.getParent()->getKey() : RegistryKeyType(0));
		map.insert(prefix + FIELDS[2].name, webpage.getRank());
	}



	template<>
	void ComplexObjectField<WebpageTreeNode, WebpageTreeNode::Type>::SaveToFilesMap(
		const WebpageTreeNode::Type& fieldObject,
		const ObjectBase& object,
		FilesMap& map
	){
	}



	namespace cms
	{
		const string Webpage::DATA_PUBLICATION_DATE("date");
		const string Webpage::DATA_FORUM("forum");
		const string Webpage::DATA_DEPTH("depth");
	}

	namespace cms
	{
		Webpage::Webpage( util::RegistryKeyType id  ):
			Registrable(id),
			Object<Webpage, WebpageRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(WebpageTreeNode),
					FIELD_DEFAULT_CONSTRUCTOR(Title),
					FIELD_DEFAULT_CONSTRUCTOR(WebpageContent),
					FIELD_VALUE_CONSTRUCTOR(StartTime, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(EndTime, posix_time::not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(Abstract),
					FIELD_DEFAULT_CONSTRUCTOR(ImageURL),
					FIELD_DEFAULT_CONSTRUCTOR(WebpageLinks),
					FIELD_VALUE_CONSTRUCTOR(DoNotUseTemplate, false),
					FIELD_VALUE_CONSTRUCTOR(HasForum, false),
					FIELD_DEFAULT_CONSTRUCTOR(SmartURLPath),
					FIELD_DEFAULT_CONSTRUCTOR(SmartURLDefaultParameterName),
					FIELD_VALUE_CONSTRUCTOR(RawEditor, false),
					FIELD_DEFAULT_CONSTRUCTOR(SpecificTemplate)
			)	)
		{
		}



		bool Webpage::mustBeDisplayed( boost::posix_time::ptime now /*= boost::posix_time::second_clock::local_time()*/ ) const
		{
			return
				(get<StartTime>().is_not_a_date_time() || get<StartTime>() <= now) &&
				(get<EndTime>().is_not_a_date_time() || get<EndTime>() >= now)
			;
		}



		void Webpage::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap
		) const	{
			ParametersMap variables;
			display(stream, request, additionalParametersMap, variables);
		}



		void Webpage::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			util::ParametersMap& variables
		) const	{
			get<WebpageContent>().display(
				stream,
				request,
				additionalParametersMap,
				*this,
				variables
			);
		}



		std::string Webpage::getMimeType() const
		{
			return get<WebpageContent>().getMimeType();
		}



		Webpage* Webpage::getTemplate() const
		{
			if(get<DoNotUseTemplate>())
			{
				return NULL;
			}
			if(get<SpecificTemplate>() && get<SpecificTemplate>().get_ptr() != this)
			{
				return this->get<SpecificTemplate>().get_ptr();
			}
			if(getRoot()->get<DefaultTemplate>().get_ptr() != this)
			{
				return getRoot()->get<DefaultTemplate>().get_ptr();
			}
			return NULL;
		}



		std::size_t Webpage::getDepth() const
		{
			size_t depth(0);
			for(Webpage* page(getParent()); page; page = page->getParent())
			{
				++depth;
			}
			return depth;
		}



		std::string Webpage::getFullName() const
		{
			string result;
			for(const Webpage* page(this); page; page = page->getParent())
			{
				result = page->get<Title>() + (result.empty() ? "/" : string()) + result;
			}
			if(getRoot())
			{
				result = getRoot()->getName() + "/" + result;
			}
			return result;
		}



		void Webpage::addAdditionalParameters(
			util::ParametersMap& pm,
			const std::string& prefix
		) const {

			// Deprecated : for backward compatibility, use start_time instead
			pm.insert(
				prefix + DATA_PUBLICATION_DATE,
				get<StartTime>().is_not_a_date_time() ? string() : lexical_cast<string>(get<StartTime>())
			);

			// Deprecated : for backward compatibility, use id instead
			pm.insert(prefix + Request::PARAMETER_OBJECT_ID, get<Key>());

			// Deprecated : for backward compatibility, use has_forum instead
			pm.insert(prefix + DATA_FORUM, get<HasForum>());

			// Depth in the pages tree
			pm.insert(prefix + DATA_DEPTH, getDepth());
		}



		SubObjects Webpage::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(const ChildrenType::value_type& page, getChildren())
			{
				r.push_back(page.second);
			}
			return r;
		}


		void Webpage::link(
			util::Env& env,
			bool withAlgorithmOptimizations
		){
			if(withAlgorithmOptimizations)
			{
				registerInParentOrRoot();
				getRoot()->addPage(*this);
			}
		}



		void Webpage::unlink()
		{
			unregisterInParentOrRoot();
			getRoot()->removePage(get<SmartURLPath>());
			this->setParent(NULL);
		}
}	}
