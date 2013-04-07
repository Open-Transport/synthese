#ifndef FUNCTIONAPI_H
#define FUNCTIONAPI_H

#include <map>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////
/// @defgroup refFunctions Published services
///	@ingroup ref

namespace synthese
{

	namespace server
	{

		//////////////////////////////////////////////////////////////////////////
		/// \brief The APIParam class handles the API definition of a service
		///
		class FunctionAPIParam
		{
		private:
			std::string _key;
			std::string _description;
			bool _mandatory;
		public:
			FunctionAPIParam(std::string key, std::string description, bool mandatory);
			std::string getKey() const;
			std::string getDescription() const;
			bool isMandatory() const;
		};

		class FunctionAPI
		{
		public:
			/// Key is the paramGroupName
			typedef std::map<std::string, std::vector<FunctionAPIParam> > APIParamsMap;

		private:
			std::string _group;
			std::string _title;
			std::string _description;
			APIParamsMap _params;
			bool _deprecated;
			std::string _currentParamGroupName;
		public:
			FunctionAPI();
			FunctionAPI(std::string group, std::string title, std::string description);
			std::string getGroup() const;
			std::string getTitle() const;
			std::string getDescription() const;
			bool isDeprecated() const;
			void setDeprecated(bool deprecated);
			///
			/// \brief openParamGroup sets the current parameter group for next call to addPrams
			/// \param paramGroupName
			///
			void openParamGroup(std::string paramGroupName);
			///
			/// \brief addParams in the group set in a previous call to openParamGroup
			/// \param key
			/// \param description
			/// \param mandatory
			///
			void addParams(std::string key, std::string description, bool mandatory);
			const APIParamsMap &getParams() const;
		};
}	}
#endif // FUNCTIONAPI_H
