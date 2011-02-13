
/** TimetableServiceColRowInterfacePage class implementation.
	@file TimetableServiceColRowInterfacePage.cpp
	@author Hugues Romain
	@date 2010

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

#include "TimetableServiceColInterfacePage.hpp"
#include "TimetableRow.h"
#include "StopArea.hpp"
#include "City.h"
#include "TimetableResult.hpp"
#include "Interface.h"
#include "CommercialLine.h"
#include "StaticFunctionRequest.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "RollingStock.h"
#include "JourneyPattern.hpp"
#include "PTObjectsCMSExporters.hpp"
#include "SchedulesBasedService.h"

#include <boost/date_time/time_duration.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace timetables;
	using namespace pt;
	using namespace server;

	namespace timetables
	{
		const std::string TimetableServiceColInterfacePage::TYPE_LINE("line");
		const std::string TimetableServiceColInterfacePage::TYPE_TIME("time");
		const std::string TimetableServiceColInterfacePage::TYPE_NOTE("note");
		const std::string TimetableServiceColInterfacePage::TYPE_BOOKING("booking");
		const std::string TimetableServiceColInterfacePage::TYPE_ROLLING_STOCK("rollingstock");

		const string TimetableServiceColInterfacePage::DATA_TYPE("type");
		const string TimetableServiceColInterfacePage::DATA_CELLS_CONTENT("cells_content");
		const string TimetableServiceColInterfacePage::DATA_ROW_RANK("row_rank");
		const string TimetableServiceColInterfacePage::DATA_ROW_RANK_IS_ODD("row_rank_is_odd");
		const string TimetableServiceColInterfacePage::DATA_CELL_RANK("cell_rank");
		const string TimetableServiceColInterfacePage::DATA_CITY_ID("city_id");
		const string TimetableServiceColInterfacePage::DATA_CITY_NAME("city_name");
		const string TimetableServiceColInterfacePage::DATA_PLACE_ID("place_id");
		const string TimetableServiceColInterfacePage::DATA_PLACE_NAME("place_name");
		const string TimetableServiceColInterfacePage::DATA_HOURS("hours");
		const string TimetableServiceColInterfacePage::DATA_MINUTES("minutes");
		const string TimetableServiceColInterfacePage::DATA_NOTE_NUMBER("note_number");
		const string TimetableServiceColInterfacePage::DATA_NOTE_TEXT("note_text");
		const string TimetableServiceColInterfacePage::DATA_ROLLING_STOCK_NAME("rolling_stock_name");
		const string TimetableServiceColInterfacePage::DATA_ROLLING_STOCK_ALIAS("rolling_stock_alias");
		const string TimetableServiceColInterfacePage::DATA_IS_BEFORE_TRANSFER("is_before_transfer");
		const string TimetableServiceColInterfacePage::DATA_TRANSFER_DEPTH("transfer_depth");
		const string TimetableServiceColInterfacePage::DATA_GLOBAL_RANK("global_rank");
		const string TimetableServiceColInterfacePage::DATA_BLOCK_MAX_RANK("block_max_rank");
		const string TimetableServiceColInterfacePage::DATA_IS_ARRIVAL("is_arrival");
		const string TimetableServiceColInterfacePage::DATA_IS_DEPARTURE("is_departure");
		const string TimetableServiceColInterfacePage::DATA_STOP_NAME_26("stop_name_26");
		const string TimetableServiceColInterfacePage::DATA_TRANSPORT_MODE_ID("transport_mode_id");
		const string TimetableServiceColInterfacePage::DATA_SERVICE_ID("service_id");



		void TimetableServiceColInterfacePage::DisplayLinesRow(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> cellPage,
			const server::Request& request,
			const TimetableResult::RowLinesVector& lines
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_LINE); //0
		
			if(cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const CommercialLine* line, lines)
				{
					if(line)
					{
						DisplayLineCell(content, cellPage, request, *line, colRank++);
					}
					else
					{
						DisplayEmptyLineCell(content, cellPage, request, colRank++);
					}
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayLineCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const pt::CommercialLine& object,
			std::size_t colRank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_LINE); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			pm.insert(DATA_ROW_RANK, 0); //2
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey()); //3
			PTObjectsCMSExporters::ExportLine(pm, object);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayEmptyLineCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			std::size_t colRank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_LINE); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			pm.insert(DATA_ROW_RANK, 0); //2
			
			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplaySchedulesRow(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> cellPage,
			const server::Request& request,
			const TimetableRow& place,
			const TimetableResult::RowTimesVector& times,
			const TimetableResult::RowServicesVector& services,
			std::size_t globalRank,
			bool isBeforeTransfer,
			std::size_t depth
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);


			pm.insert(DATA_TYPE, TYPE_TIME); //0
			pm.insert(DATA_GLOBAL_RANK, globalRank);
			pm.insert(DATA_IS_BEFORE_TRANSFER, isBeforeTransfer);
			pm.insert(DATA_TRANSFER_DEPTH, depth);
		
			if(cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(time_duration duration, times)
				{
					DisplayScheduleCell(
						content,
						cellPage,
						request,
						duration,
						place.getRank(),
						colRank,
						services[colRank]
					);
					++colRank;
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			pm.insert(DATA_ROW_RANK, place.getRank()); //2

			pm.insert(DATA_ROW_RANK_IS_ODD, place.getRank() % 2); //3
			pm.insert(DATA_IS_ARRIVAL, place.getIsArrival());
			pm.insert(DATA_IS_DEPARTURE, place.getIsDeparture());

			pm.insert(DATA_CITY_ID, place.getPlace()->getCity()->getKey()); //4
			pm.insert(DATA_PLACE_ID, place.getPlace()->getKey()); //5
			pm.insert(DATA_CITY_NAME, place.getPlace()->getCity()->getName()); //6
			pm.insert(DATA_PLACE_NAME, place.getPlace()->getName()); //7
			if(dynamic_cast<const StopArea*>(place.getPlace()))
			{
				pm.insert(DATA_STOP_NAME_26, static_cast<const StopArea*>(place.getPlace())->getTimetableName());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayScheduleCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			boost::posix_time::time_duration object,
			std::size_t rowRank,
			std::size_t colRank,
			const pt::SchedulesBasedService* service
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_TIME); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			pm.insert(DATA_ROW_RANK, rowRank); //2
			if(!object.is_not_a_date_time())
			{
				pm.insert(DATA_HOURS, object.hours()); //3
				pm.insert(DATA_MINUTES, object.minutes()); //4
			}

			if(service)
			{
				pm.insert(DATA_SERVICE_ID, service->getKey());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayNotesRow(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> cellPage,
			const server::Request& request,
			const TimetableResult::RowNotesVector& notes,
			const TimetableResult::Columns& columns
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_NOTE); //0
		
			if(cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const TimetableResult::Columns::value_type& col, columns)
				{
					DisplayNoteCell(content, cellPage, request, colRank++, col);
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayNoteCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			std::size_t colRank,
			const TimetableColumn& column
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_NOTE); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			if(column.getWarning().get())
			{
				pm.insert(DATA_NOTE_NUMBER, column.getWarning()->getNumber()); //3
				pm.insert(DATA_NOTE_TEXT, column.getWarning()->getText()); //4
			}
			if(column.getLine() && column.getLine()->getRollingStock())
			{
				pm.insert(DATA_TRANSPORT_MODE_ID, column.getLine()->getRollingStock()->getKey());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void TimetableServiceColInterfacePage::DisplayRollingStockRow(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> cellPage,
			const server::Request& request,
			const TimetableResult::RowRollingStockVector& rollingStock
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_ROLLING_STOCK); //0

			if(cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const RollingStock* object, rollingStock)
				{
					DisplayRollingStockCell(content, cellPage, request, object, colRank++);
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}


		void TimetableServiceColInterfacePage::DisplayRollingStockCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const pt::RollingStock* object,
			std::size_t colRank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TYPE, TYPE_ROLLING_STOCK); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			if(object)
			{
				pm.insert(Request::PARAMETER_OBJECT_ID, object->getKey()); //3
				pm.insert(DATA_ROLLING_STOCK_ALIAS, object->getIndicator()); //4
				pm.insert(DATA_ROLLING_STOCK_NAME, object->getName()); //5
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
}	}
