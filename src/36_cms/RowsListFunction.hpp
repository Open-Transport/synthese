
//////////////////////////////////////////////////////////////////////////////////////////
/// RowsListFunction class header.
///	@file RowsListFunction.hpp
///	@author Gael Sauvanet
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
#include "TransportWebsite.h"
#include "TransportWebsiteTypes.hpp"
#include "GeographyModule.h"

#include <string>

namespace synthese
{
	namespace cms
	{
		using namespace db;
		using namespace pt_website;
		using namespace geography;

		//////////////////////////////////////////////////////////////////////////
		/// Rows list query public function.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Rows_list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		/// @author Gaël Sauvanet
		class RowsListFunction:
			public util::FactorableTemplate<server::Function, RowsListFunction>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_TABLE;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_OUTPUT_FORMAT;
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
			boost::shared_ptr<const cms::Webpage>	_page;
			boost::shared_ptr<const cms::Webpage>	_itemPage;
			std::string _outputFormat;
			DBTableSync::TableId _table;

		protected:
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Cities_list#Request
			//////////////////////////////////////////////////////////////////////////
			util::ParametersMap _getParametersMap() const;



			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Cities_list#Request
			//////////////////////////////////////////////////////////////////////////
			void _setFromParametersMap(const util::ParametersMap& map);



			void _displayItems(
				std::ostream& stream,
				const util::ParametersMap& pm,
				const server::Request& request
			) const;

		public:
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Rows_list#Response
			//////////////////////////////////////////////////////////////////////////
			void run(std::ostream& stream, const server::Request& request) const;

			void setTextInput(const std::string& text){ _input = text; }
			void setNumber(boost::optional<std::size_t> number){ _n = number; }
			void setTable(db::DBTableSync::TableId table){ _table = table; }

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_RowsListFunction_H__
