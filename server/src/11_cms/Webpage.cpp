
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
#include "StaticFunctionRequest.h"
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
	FIELD_DEFINITION_OF_TYPE(MaxAge, "max_age", SQL_INTEGER)

	template<> const Field ComplexObjectFieldDefinition<WebpageTreeNode>::FIELDS[] = {
		Field("site_id", SQL_INTEGER),
		Field("up_id", SQL_INTEGER),
		Field("rank", SQL_INTEGER),
	Field() };



	namespace cms
	{
		const string Webpage::DATA_PUBLICATION_DATE("date");
		const string Webpage::DATA_FORUM("forum");
		const string Webpage::DATA_DEPTH("depth");

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
					FIELD_VALUE_CONSTRUCTOR(MaxAge, posix_time::not_a_date_time),
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

		Webpage::~Webpage()
		{
			// Unlink children before delete
			BOOST_FOREACH(const ChildrenType::value_type& page, getChildren())
			{
				page.second->unlink();
			}
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
			get<WebpageContent>().getCMSScript().display(
				stream,
				request,
				additionalParametersMap,
				*this,
				variables
			);
		}



		void Webpage::display(
			std::ostream& stream,
			util::ParametersMap& additionalParametersMap
		) const	{
			StaticFunctionRequest<WebPageDisplayFunction> fakeRequest;

			// This version of 'display' is invoked with a parameters map that does not contain webpage properties
			if(false == additionalParametersMap.isDefined("id"))
			{
				additionalParametersMap.insert("id", get<Key>());
			}
			if(false == additionalParametersMap.isDefined("title"))
			{
				additionalParametersMap.insert("title", get<Title>());
			}

			display(stream, fakeRequest, additionalParametersMap);
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


		boost::posix_time::time_duration Webpage::getMaxAge() const
		{
			return get<MaxAge>();
		}

		void Webpage::addAdditionalParameters(
			util::ParametersMap& pm,
			std::string prefix
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
				if(hasRoot())
				{
					getRoot()->addPage(*this);
				}
				else
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the load of the " +
						this->getClassName() +" object " + boost::lexical_cast<std::string>(this->getKey())
					);
				}
			}
		}



		void Webpage::unlink()
		{
			unregisterInParentOrRoot();
			getRoot()->removePage(get<SmartURLPath>());
			if(hasRoot())
			{
				this->setParent(NULL);
			}
			else
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the load of the " +
					this->getClassName() +" object " + boost::lexical_cast<std::string>(this->getKey())
				);
			}
		}



		void Webpage::getPagesList(
			Website::WebpagesList& result,
			const std::string prefix
		) const	{
			BOOST_FOREACH(const SubObjects::value_type& so, getSubObjects())
			{
				const Webpage& page(dynamic_cast<const Webpage&>(*so));
				result.push_back(make_pair(page.getKey(), prefix + page.getName()));
				page.getPagesList(result, prefix + "   ");
			}
		}
}	}
