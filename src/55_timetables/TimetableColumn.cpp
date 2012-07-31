
/** TimetableColumn class implementation.
	@file TimetableColumn.cpp

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

#include "TimetableColumn.h"

#include "CommercialLine.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "ParametersMap.h"
#include "Path.h"
#include "PTUseRule.h"
#include "RollingStock.hpp"
#include "SchedulesBasedService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "TimetableRow.h"
#include "TimetableGenerator.h"
#include "Vertex.h"

#include <list>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
	using namespace graph;
	using namespace pt;
	using namespace util;

	namespace timetables
	{
		const string TimetableColumn::TAG_NOTE = "note";
		const string TimetableColumn::TAG_SERVICE = "service";
		const string TimetableColumn::TAG_LINE = "line";
		const string TimetableColumn::TAG_TRANSPORT_MODE = "transport_mode";
		const string TimetableColumn::TAG_USE_RULE = "use_rule";



		TimetableColumn::TimetableColumn(
			const TimetableGenerator& timetablegenerator,
			const SchedulesBasedService& service
		):	_line(static_cast<const JourneyPattern*>(service.getPath())),
			_service(&service),
			_calendar(service)
		{
			_calendar &= timetablegenerator.getBaseCalendar();

			// Cells are populated with the most important intermediate places
			if(timetablegenerator.getAutoIntermediateStops())
			{
				// The departure place
				const TimetableGenerator::Rows& rows(timetablegenerator.getRows());
				TimetableGenerator::Rows::const_iterator itRow(rows.begin());
				if(!itRow->getPlace() || !itRow->getIsDeparture())
				{
					return;
				}

				// The departure edge
				const Path::Edges& edges(service.getPath()->getAllEdges());
				Path::Edges::const_iterator itEdge;
				for(itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
				{
					if(	dynamic_cast<const StopArea*>((*itEdge)->getFromVertex()->getHub())->getKey() == itRow->getPlace()->getKey() &&
						(*itEdge)->isDeparture()
					){
						break;
					}
				}
				if(itEdge == edges.end())
				{
					return;
				}
				_content.push_back(
					make_pair(
						dynamic_cast<const StopPoint*>((*itEdge)->getFromVertex()),
						service.getDepartureBeginScheduleToIndex(false, (*itEdge)->getRankInPath())
				)	);

				// Collecting arrival edges
				list<Edge*> bestEdges;
				list<Edge*>::iterator worseEdge(bestEdges.end());
				for(++itEdge; itEdge != edges.end() && (itEdge+1) != edges.end(); ++itEdge)
				{
					// Jump over departure only edges
					if(!(*itEdge)->isArrival())
					{
						continue;
					}

					// If best edges size has not reached the limit, the edge is always stored
					if(bestEdges.size() < *timetablegenerator.getAutoIntermediateStops())
					{
						list<Edge*>::iterator itInsertedEdge(
							bestEdges.insert(
								bestEdges.end(),
								*itEdge
						)	);

						// The worse edge
						if(bestEdges.size() == 1)
						{
							worseEdge = itInsertedEdge;
						}
						else
						{
							if(	dynamic_cast<const StopArea*>((*itEdge)->getFromVertex()->getHub())->getScore() <
								dynamic_cast<const StopArea*>((*worseEdge)->getFromVertex()->getHub())->getScore()
							){
								worseEdge = itInsertedEdge;
							}
						}
					}
					else // Edge is stored if the score of the place is better than the worse score of the list
					{
						if(	dynamic_cast<const StopArea*>((*itEdge)->getFromVertex()->getHub())->getScore() >
							dynamic_cast<const StopArea*>((*worseEdge)->getFromVertex()->getHub())->getScore()
						){
							bestEdges.erase(worseEdge);
							list<Edge*>::iterator itInsertedEdge(
								bestEdges.insert(
									bestEdges.end(),
									*itEdge
							)	);
							worseEdge = itInsertedEdge;
							for(list<Edge*>::iterator itBestEdge(bestEdges.begin());
								itBestEdge != bestEdges.end();
								++itBestEdge
							){
								if(	dynamic_cast<const StopArea*>((*itBestEdge)->getFromVertex()->getHub())->getScore() <
									dynamic_cast<const StopArea*>((*worseEdge)->getFromVertex()->getHub())->getScore()
								){
									worseEdge = itBestEdge;
								}
							}
						}
					}
				}

				// Generating the cells
				size_t toInsert(*timetablegenerator.getAutoIntermediateStops());
				for(list<Edge*>::const_iterator itEdge(bestEdges.begin());
					itEdge != bestEdges.end();
					++itEdge
				){
					_content.push_back(
						make_pair(
							dynamic_cast<const StopPoint*>((*itEdge)->getFromVertex()),
							service.getArrivalBeginScheduleToIndex(false, (*itEdge)->getRankInPath())
					)	);
					--toInsert;
				}

				for(;toInsert;--toInsert)
				{
					_content.push_back(make_pair<const StopPoint*, time_duration>(NULL, time_duration(not_a_date_time)));
				}

				Path::Edges::const_reverse_iterator itrEdge = edges.rbegin();
				_content.push_back(
					make_pair(
						dynamic_cast<const StopPoint*>((*itrEdge)->getFromVertex()),
						service.getArrivalBeginScheduleToIndex(false, (*itrEdge)->getRankInPath())
				)	);
			}
			else
			{ // Cells are populated according to the rows definitions
				const TimetableGenerator::Rows& rows(timetablegenerator.getRows());

				const Path::Edges& edges(service.getPath()->getAllEdges());
				Path::Edges::const_iterator itEdge(edges.begin());
				bool first(true);

				for(TimetableGenerator::Rows::const_iterator itRow(rows.begin()); itRow != rows.end(); ++itRow)
				{
					if(!itRow->getPlace())
					{
						continue;
					}

					Path::Edges::const_iterator itEdge2;

					// Arr / Dep in the same place
					if(	itRow != rows.begin() &&
						(itRow-1)->getPlace() &&
						itRow->getPlace() == (itRow-1)->getPlace() &&
						itRow->getIsDeparture() &&
						!(itRow-1)->getIsDeparture() &&
						itEdge != edges.begin() &&
						(*(itEdge-1))->isDeparture() &&
						dynamic_cast<const StopArea*>((*(itEdge-1))->getFromVertex()->getHub())->getKey() == (itRow-1)->getPlace()->getKey()
					){
						_content.push_back(
							make_pair(
								dynamic_cast<const StopPoint*>((*(itEdge-1))->getFromVertex()),
								service.getDepartureBeginScheduleToIndex(false, (itEdge-1) - edges.begin())
						)	);
						continue;
					}

					bool exit(false);
					for (itEdge2 = itEdge; itEdge2 != edges.end() && !exit; ++itEdge2)
					{
						if(	dynamic_cast<const StopArea*>((*itEdge2)->getFromVertex()->getHub())->getKey() == itRow->getPlace()->getKey()
							&&(	(*itEdge2)->isDeparture() && itRow->getIsDeparture()
								|| (*itEdge2)->isArrival() && itRow->getIsArrival()
							) &&
							(	!first ||
								timetablegenerator.getAuthorizedPhysicalStops().empty() ||
								timetablegenerator.getAuthorizedPhysicalStops().find(dynamic_cast<const StopPoint*>((*itEdge2)->getFromVertex())) != timetablegenerator.getAuthorizedPhysicalStops().end()
							)
						){
							// Attempt to find an other position which allow to output previous schedules
							for(TimetableGenerator::Rows::const_iterator itRow4(itRow + 1); itRow4 != rows.end() && !exit; ++itRow4)
							{
								Path::Edges::const_iterator itEdge3;
								for(itEdge3 = itEdge; itEdge3 != itEdge2 && !exit; ++itEdge3)
								{
									if(	dynamic_cast<const StopArea*>((*itEdge3)->getFromVertex()->getHub())->getKey() == itRow4->getPlace()->getKey() &&
										(	((*itEdge3)->isDeparture() && itRow4->getIsDeparture()) || ((*itEdge3)->isArrival() && itRow4->getIsArrival())
										) &&
									(	!first ||
										timetablegenerator.getAuthorizedPhysicalStops().empty() ||
										timetablegenerator.getAuthorizedPhysicalStops().find(dynamic_cast<const StopPoint*>((*itEdge3)->getFromVertex())) != timetablegenerator.getAuthorizedPhysicalStops().end()
										)
									){
										exit = true;
										break;
									}
								}
								if(exit)
								{
									exit = false;
									for(TimetableGenerator::Rows::const_iterator itRow5(itRow4 + 1); itRow5 != rows.end() && !exit; ++itRow5)
									{
										for(Path::Edges::const_iterator itEdge4 = itEdge3+1; itEdge4 != edges.end(); ++itEdge4)
										{
											if(	dynamic_cast<const StopArea*>((*itEdge4)->getFromVertex()->getHub())->getKey() == itRow5->getPlace()->getKey() &&
												(((*itEdge4)->isArrival() && itRow5->getIsArrival()) || ((*itEdge4)->isDeparture() && itRow5->getIsDeparture()))
											){
												exit = true;
												break;
											}
									}	}
								}
							}

							if(!exit)
							{
								// Record the schedule in the col
								first = false;
								_content.push_back(
									make_pair(
										dynamic_cast<const StopPoint*>((*itEdge2)->getFromVertex()),
										((*itEdge2)->isDeparture() && itRow->getIsDeparture()) ?
											service.getDepartureBeginScheduleToIndex(false, (*itEdge2)->getRankInPath()) :
											service.getArrivalBeginScheduleToIndex(false, (*itEdge2)->getRankInPath())
								)	);
								if (itEdge2 == edges.begin())
									_originType = Terminus;
								if (itEdge2 == edges.end() - 1)
									_destinationType = Terminus;
								itEdge = itEdge2 + 1;
								break;
							}
						}
					}
					if (itEdge2 == edges.end() || exit)
					{
						_content.push_back(make_pair<const StopPoint*, time_duration>(NULL, time_duration(not_a_date_time)));
					}
				}
			}
		}



		TimetableColumn::TimetableColumn(
			const TimetableGenerator& generator
		):	_line(NULL),
			_service(NULL),
			_originType(Indetermine),
			_destinationType(Indetermine)
		{
			const TimetableGenerator::Rows& rows(generator.getRows());
			for(TimetableGenerator::Rows::const_iterator itRow(rows.begin()); itRow != rows.end(); ++itRow)
			{
				if(!itRow->getPlace())
				{
					continue;
				}

				_content.push_back(make_pair<const StopPoint*, time_duration>(NULL, time_duration(not_a_date_time)));
			}
		}



		int TimetableColumn::operator<=( const TimetableColumn& other) const
		{
			assert(_content.size() == other._content.size());

			size_t rowsNumber(_content.size());
			size_t i;
			size_t j;

			// Tentative par ligne commune
			for (i=0; i<rowsNumber; ++i)
			{
				if (!_content[i].second.is_not_a_date_time() && !other._content[i].second.is_not_a_date_time())
				{
					if (_content[i].second < other._content[i].second)
						return true;
					if (other._content[i].second < _content[i].second)
						return false;
				}
			}

			// Tentative par succession 1
			for (i=0; i< rowsNumber; ++i)
			{
				if (!_content[i].second.is_not_a_date_time())
				{
					for(j = i+1; j< rowsNumber; ++j)
					{
						if(	!other._content[j].second.is_not_a_date_time() &&
							other._content[j].second < _content[i].second
						){
							return false;
						}

						if (i != 0)
						{
							for(size_t k = i-1; k>0; --k)
							{
								if(	!other._content[k].second.is_not_a_date_time() &&
									_content[i].second < other._content[k].second
								)	return true;
							}
						}
					}
				}
			}

			// Premiere heure
			for (i=0; i< rowsNumber; ++i)
				if (!_content[i].second.is_not_a_date_time())
					break;
			for (j=0; j< rowsNumber; ++j)
				if (!other._content[j].second.is_not_a_date_time())
					break;
			return _content[i].second < other._content[j].second;
		}



		void TimetableColumn::merge( const TimetableColumn& col )
		{
			if(_line && col._line)
			{
				if( _line->getOrigin()->getHub() != col._line->getOrigin()->getHub()
				){
					_originType = Indetermine;
				}
				if(	_line->getDestination()->getHub() != col._line->getDestination()->getHub()
				){
					_destinationType = Indetermine;
				}
			}

			_service = NULL;
			_calendar |= col._calendar;
		}



		bool TimetableColumn::operator==( const TimetableColumn& op ) const
		{
			assert(op._content.size() == _content.size());

			// Two empty columns are identical
			if(_line == NULL && op._line == NULL)
			{
				return true;
			}

			// An empty column is not identical to a non empty one
			if(	_line == NULL || op._line == NULL)
			{
				return false;
			}

			// Tests if the lines are different
			if(_line->getCommercialLine() != op._line->getCommercialLine())
			{
				return false;
			}

			// Search for different schedules
			for(Content::const_iterator it1(_content.begin()), it2(op._content.begin());
				it1 != _content.end();
				++it1, ++it2)
			{
				if(it1->second != it2->second)
				{
					return false;
				}
			}

			// Everything is OK the colums are identical
			return true;
		}



		bool TimetableColumn::includes( const TimetableColumn& op ) const
		{
			assert(op._content.size() == _content.size());

			if (_line == NULL || op._line == NULL || _line->getCommercialLine() != op._line->getCommercialLine())
			{
				return false;
			}
			for(Content::const_iterator it1(_content.begin()), it2(op._content.begin());
				it1 != _content.end();
				++it1, ++it2
			){
				if(	it1->first == NULL && it2->first != NULL ||
					it1->first != NULL && it2->first != NULL && it1->second != it2->second
				){
					return false;
				}
			}
			return true;
		}



		void TimetableColumn::toParametersMap(
			ParametersMap& pm,
			bool withSchedules
		) const	{

			// Note
			if(_warning)
			{
				shared_ptr<ParametersMap> notePM(new ParametersMap);
				_warning->toParametersMap(*notePM, false);
				pm.insert(TAG_NOTE, notePM);
			}

			// Service
			if(_service)
			{
				shared_ptr<ParametersMap> servicePM(new ParametersMap);
				_service->toParametersMap(*servicePM);
				pm.insert(TAG_SERVICE, servicePM);
			}

			// Line
			if(_line && _line->getCommercialLine())
			{
				shared_ptr<ParametersMap> linePM(new ParametersMap);
				_line->getCommercialLine()->toParametersMap(*linePM);
				pm.insert(TAG_LINE, linePM);
			}

			// Transport mode
			if(_line && _line->getRollingStock())
			{
				shared_ptr<ParametersMap> transportModePM(new ParametersMap);
				_line->getRollingStock()->toParametersMap(*transportModePM);
				pm.insert(TAG_TRANSPORT_MODE, transportModePM);
			}

			// Use rule
			if(_service)
			{
				const PTUseRule* ptUseRule(
					dynamic_cast<const PTUseRule*>(
						&_service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
				)	);
				if(ptUseRule)
				{
					shared_ptr<ParametersMap> useRulePM(new ParametersMap);
					ptUseRule->toParametersMap(*useRulePM);
					pm.insert(TAG_USE_RULE, useRulePM);
				}
			}

		}
}	}
