
#include "FunctionAPI.h"

using namespace std;

namespace synthese
{

	namespace server
	{

		// APIParam

		FunctionAPIParam::FunctionAPIParam(std::string key, std::string description, bool mandatory):
			_key(key),
			_description(description),
			_mandatory(mandatory)
		{}

		std::string FunctionAPIParam::getKey() const
		{
			return _key;
		}

		std::string FunctionAPIParam::getDescription() const
		{
			return _description;
		}

		bool FunctionAPIParam::isMandatory() const
		{
			return _mandatory;
		}

		// API description

		FunctionAPI::FunctionAPI():
		_deprecated(false)
		{}

		FunctionAPI::FunctionAPI(string group, std::string title, std::string description):
			_group(group),
			_title(title),
			_description(description),
			_deprecated(false),
			_currentParamGroupName("")
		{}

		string FunctionAPI::getGroup() const
		{
			return _group;
		}

		string FunctionAPI::getTitle() const
		{
			return _title;
		}

		string FunctionAPI::getDescription() const
		{
			return _description;
		}

		bool FunctionAPI::isDeprecated() const
		{
			return _deprecated;
		}

		void FunctionAPI::setDeprecated(bool deprecated)
		{
			_deprecated = deprecated;
		}

		void FunctionAPI::openParamGroup(std::string paramGroupName)
		{
			_currentParamGroupName = paramGroupName;
		}

		void FunctionAPI::addParams(std::string key, std::string description, bool mandatory)
		{
			if(_params.find(_currentParamGroupName) == _params.end())
			{
				_params[_currentParamGroupName] = vector<FunctionAPIParam>();
			}
			_params[_currentParamGroupName].push_back(FunctionAPIParam(key, description, mandatory));
		}

		const FunctionAPI::APIParamsMap &FunctionAPI::getParams() const
		{
			return _params;
		}

	}
}
