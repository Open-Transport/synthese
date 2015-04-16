
//////////////////////////////////////////////////////////////////////////////////////////
/// RowsListFunction class header.
///	@file RowsListFunction.hpp
///	@author Gael Sauvanet
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_RowsListFunction_H__
#define SYNTHESE_RowsListFunction_H__

#include "DBTableSync.hpp"
#include "DBModule.h"
#include "FunctionWithSite.h"
#include "FactorableTemplate.h"
#include "ParametersMap.h"
#include "GeographyModule.h"

#include <string>

namespace synthese
{
	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		/// Rows list query public function.
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		/// @author Gaël Sauvanet
		class RowsListFunction:
			public util::FactorableTemplate<server::Function, RowsListFunction>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_TABLE;
			static const std::string PARAMETER_OPTIONAL;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_SRID;

			static const std::string DATA_RESULTS_SIZE;
			static const std::string DATA_CONTENT;

			static const std::string DATA_ROW;
			static const std::string DATA_ROWS;
			static const std::string DATA_NAME;
			static const std::string DATA_RANK;

		private:
			std::string _input;
			boost::optional<std::size_t> _n;
			boost::optional<std::string> _optional;
			boost::shared_ptr<const cms::Webpage>	_page;
			boost::shared_ptr<const cms::Webpage>	_itemPage;
			util::RegistryTableType _table;

		protected:
			util::ParametersMap _getParametersMap() const;



			void _setFromParametersMap(const util::ParametersMap& map);



			void _displayItems(
				std::ostream& stream,
				const util::ParametersMap& pm,
				const server::Request& request
			) const;

		public:
			util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			void setTextInput(const std::string& text){ _input = text; }
			void setNumber(boost::optional<std::size_t> number){ _n = number; }
			void setOptional(boost::optional<std::string> optional){ _optional = optional; }
			void setTable(util::RegistryTableType table){ _table = table; }

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_RowsListFunction_H__
