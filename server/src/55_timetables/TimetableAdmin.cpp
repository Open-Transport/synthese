
/** TimetableAdmin class implementation.
	@file TimetableAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableAdmin.h"

#include "ActionResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"
#include "Calendar.h"
#include "CalendarModule.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLine.h"
#include "City.h"
#include "HTMLModule.h"
#include "JourneyPattern.hpp"
#include "LinePhysicalStop.hpp"
#include "LineStop.h"
#include "ModuleAdmin.h"
#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "RemoveObjectAction.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "Timetable.h"
#include "TimetableAddAction.h"
#include "TimetableGenerateFunction.h"
#include "TimetableModule.h"
#include "TimetableResult.hpp"
#include "TimetableRight.h"
#include "TimetableRow.h"
#include "TimetableRowAddAction.h"
#include "TimetableRowGroup.hpp"
#include "TimetableRowGroupItem.hpp"
#include "TimetableRowGroupItemAddAction.hpp"
#include "TimetableRowTableSync.h"
#include "TimetableSetLineAction.h"
#include "TimetableSetPhysicalStopAction.h"
#include "TimetableTableSync.h"
#include "TimetableTransferUpdateAction.hpp"
#include "TimetableUpdateAction.h"
#include "User.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace timetables;
	using namespace html;
	using namespace db;
	using namespace calendar;
	using namespace security;
	using namespace pt;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TimetableAdmin>::FACTORY_KEY("TimetableAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TimetableAdmin>::ICON("table.png");
		template<> const string AdminInterfaceElementTemplate<TimetableAdmin>::DEFAULT_TITLE("Fiches horaires");
	}

	namespace timetables
	{
		const string TimetableAdmin::PARAMETER_RANK("rk");
		const string TimetableAdmin::PARAMETER_TITLE("ty");
		const string TimetableAdmin::TAB_CONTENT("co");
		const string TimetableAdmin::TAB_PROPERTIES("pr");
		const string TimetableAdmin::TAB_RESULT("re");

		TimetableAdmin::TimetableAdmin()
			: AdminInterfaceElementTemplate<TimetableAdmin>()
		{ }

		void TimetableAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			if(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
			{
				try
				{
					_timetable = Env::GetOfficialEnv().get<Timetable>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<Timetable>&)
				{
					throw AdminParametersException("No such timetable");
				}
			}
			_requestParameters.setFromParametersMap(map, PARAMETER_RANK);
		}



		util::ParametersMap TimetableAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_timetable.get()) m.insert(Request::PARAMETER_OBJECT_ID, _timetable->getKey());
			return m;
		}



		void TimetableAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				// Requests
				AdminActionFunctionRequest<TimetableUpdateAction,TimetableAdmin> updateRequest(_request, *this);
				updateRequest.getAction()->setTimetable(const_pointer_cast<Timetable>(_timetable));

				// Display
				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable pt(updateRequest.getHTMLForm("update"));
				stream << pt.open();
				stream << pt.cell(
					"ID",
					lexical_cast<string>(_timetable->getKey())
				);

				stream << pt.cell(
					"Conteneur",
					pt.getForm().getSelectInput(
						TimetableUpdateAction::PARAMETER_CONTAINER_ID,
						TimetableModule::GetTimetableContainersLabels(0, string(), _timetable->getKey()),
						optional<RegistryKeyType>(_timetable->get<Book>() ? _timetable->get<Book>()->getKey() : 0)
				)	);

				stream << pt.cell(
					"Calendrier",
					pt.getForm().getSelectInput(
						TimetableUpdateAction::PARAMETER_BASE_CALENDAR_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
						optional<RegistryKeyType>(_timetable->get<BaseCalendar>() ? _timetable->get<BaseCalendar>()->getKey() : 0)
				)	);

				stream << pt.cell(
					"Titre",
					pt.getForm().getTextInput(
						TimetableUpdateAction::PARAMETER_TITLE,
						_timetable->get<Title>()
				)	);

				stream << pt.cell(
					"Format",
					pt.getForm().getRadioInputCollection(
						TimetableUpdateAction::PARAMETER_FORMAT,
						Timetable::GetFormatsList(),
						optional<Timetable::ContentType>(_timetable->getContentType()),
//						optional<Timetable::ContentType>(_timetable->get<Format>()),
						true
				)	);

				stream << pt.cell(
					"Ne pas afficher arrêts non desservis",
					pt.getForm().getOuiNonRadioInput(
						TimetableUpdateAction::PARAMETER_IGNORE_EMPTY_ROWS,
						_timetable->get<IgnoreEmptyRows>()
				)	);

				stream << pt.cell(
					"Compression",
					pt.getForm().getOuiNonRadioInput(
						TimetableUpdateAction::PARAMETER_FIELD_COMPRESSION,
						_timetable->get<Compression>()
				)	);


				stream << pt.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTENT
			if (openTabContent(stream, TAB_CONTENT))
			{
				if(!_timetable.get() || _timetable->getContentType() == Timetable::CONTAINER)
				{
					// Pages
					stream << "<h1>Contenus</h1>";

					// Search
					TimetableTableSync::SearchResult timetables(
						TimetableTableSync::Search(
							Env::GetOfficialEnv(),
							_timetable.get() ? _timetable->getKey() : RegistryKeyType(0),
							_requestParameters.orderField == PARAMETER_RANK,
							_requestParameters.orderField == PARAMETER_TITLE,
							_requestParameters.raisingOrder
					)	);

					AdminFunctionRequest<TimetableAdmin> searchRequest(_request, *this);
					AdminActionFunctionRequest<TimetableAddAction,TimetableAdmin> addTimetableRequest(_request, *this);
					addTimetableRequest.getAction()->setBook(_timetable);
					addTimetableRequest.setActionWillCreateObject();

					AdminActionFunctionRequest<TimetableAddAction,TimetableAdmin> copyTimetableRequest(_request, *this);
					copyTimetableRequest.setActionWillCreateObject();

					AdminActionFunctionRequest<RemoveObjectAction,TimetableAdmin> removeRequest(_request, *this);

					ActionResultHTMLTable::HeaderVector h3;
					h3.push_back(make_pair(PARAMETER_RANK, "Rang"));
					h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("/admin/img/arrow_up.png", "^")));
					h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "V")));
					h3.push_back(make_pair(PARAMETER_TITLE, "Titre"));
					h3.push_back(make_pair(PARAMETER_TITLE, "Titre"));
					h3.push_back(make_pair(string(), "Actions"));
					h3.push_back(make_pair(string(), "Actions"));
					h3.push_back(make_pair(string(), "Actions"));
					h3.push_back(make_pair(string(), "Actions"));
					ActionResultHTMLTable t3(
						h3,
						searchRequest.getHTMLForm(),
						_requestParameters,
						timetables,
						addTimetableRequest.getHTMLForm("addtimetable"),
						TimetableAddAction::PARAMETER_RANK
						);
					stream << t3.open();
					optional<size_t> lastRank;
					optional<size_t> maxRank(
						TimetableTableSync::GetMaxRank(_timetable.get() ? _timetable->getKey() : 0)
					);

					// Links to folders or timetable edition
					AdminFunctionRequest<TimetableAdmin> editTimetableRequest(_request, *this);
					BOOST_FOREACH(const boost::shared_ptr<Timetable>& tt, timetables)
					{
						editTimetableRequest.getPage()->setTimetable(tt);
						copyTimetableRequest.getAction()->setTemplate(const_pointer_cast<const Timetable>(tt));
						removeRequest.getAction()->setObjectId(tt->getKey());

						lastRank = tt->get<Rank>();

						stream << t3.row(lexical_cast<string>(*lastRank));
						stream << t3.col() << *lastRank;
						stream << t3.col();
						if (*lastRank > 1)
						{
							stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("/admin/img/arrow_up.png", "^"));
						}
						stream << t3.col();
						if (maxRank && *lastRank < *maxRank)
						{
							stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "V"));
						}
						stream <<
							t3.col() <<
							HTMLModule::getHTMLImage(
								"/admin/img/" + Timetable::GetIcon(tt->getContentType()),
								Timetable::GetFormatName(tt->getContentType()
							)
						);
						stream << t3.col() << tt->get<Title>();
						stream <<
							t3.col() <<
							HTMLModule::getLinkButton(
								editTimetableRequest.getURL(),
								"Ouvrir",
								string(),
								"/admin/img/table_edit.png"
							);
						stream << t3.col();
							copyTimetableRequest.getAction()->setReverse(false);
							stream << HTMLModule::getLinkButton(copyTimetableRequest.getURL(), "Dupliquer", string(), "/admin/img/table_add.png");

						stream << t3.col();
							copyTimetableRequest.getAction()->setReverse(true);
							stream << HTMLModule::getLinkButton(copyTimetableRequest.getURL(), "Copie inversée", string(), "/admin/img/table_add.png");

						stream <<
							t3.col() <<
							HTMLModule::getLinkButton(
								removeRequest.getURL(),
								"Supprimer",
								"Etes-vous sûr de vouloir supprimer la fiche horaire "+ tt->get<Title>() +" ?", "/admin/img/table_delete.png"
							);
					}
					lastRank = lastRank ? *lastRank + 1 : 0;
					stream << t3.row(lexical_cast<string>(*lastRank));
					vector<pair<optional<bool>, string> > booknotbook;
					booknotbook.push_back(make_pair(true, HTMLModule::getHTMLImage("/admin/img/table_multiple.png","Document")));
					booknotbook.push_back(make_pair(false, HTMLModule::getHTMLImage("/admin/img/table.png","Fiche horaire")));
					stream << t3.col() << *lastRank;
					stream << t3.col(3) << t3.getActionForm().getRadioInputCollection(TimetableAddAction::PARAMETER_IS_BOOK, booknotbook, optional<bool>(false));
					stream << t3.col() << t3.getActionForm().getTextInput(TimetableAddAction::PARAMETER_TITLE, string(), "(titre de la nouvelle fiche horaire)");
					stream << t3.col(4) << t3.getActionForm().getSubmitButton("Créer");

					stream << t3.close();
				}
				else if(_timetable->getContentType() == Timetable::CALENDAR)
				{

				}
				else if(_timetable->getContentType() == Timetable::LINE_SCHEMA)
				{

				}
				else if(
					_timetable->getContentType() == Timetable::TABLE_SERVICES_IN_COLS ||
					_timetable->getContentType() == Timetable::TABLE_SERVICES_IN_ROWS ||
					_timetable->getContentType() == Timetable::TIMES_IN_COLS ||
					_timetable->getContentType() == Timetable::TIMES_IN_ROWS
				){
					AdminActionFunctionRequest<TimetableRowAddAction,TimetableAdmin> addRowRequest(_request, *this);
					addRowRequest.getAction()->setTimetable(_timetable);

					AdminActionFunctionRequest<RemoveObjectAction,TimetableAdmin> deleteRowRequest(_request, *this);

					AdminFunctionRequest<TimetableAdmin> searchRequest(_request, *this);

					AdminActionFunctionRequest<TimetableTransferUpdateAction, TimetableAdmin> transferRequest(_request, *this);
					transferRequest.getAction()->setTimetable(const_pointer_cast<Timetable>(_timetable));

					stream << "<h1>Arrêts</h1>";

					// Transfer before
					transferRequest.getAction()->setBefore(true);
					PropertiesHTMLTable p1(transferRequest.getHTMLForm("t1"));
					stream << p1.open();
					stream << p1.cell(
						"Correspondance avant :",
						p1.getForm().getTextInput(
							TimetableTransferUpdateAction::PARAMETER_TRANSFER_TIMETABLE_ID,
							_timetable->getTransferTimetableBefore(1) ? lexical_cast<string>(_timetable->getTransferTimetableBefore(1)->getKey()) : string()
					)	);
					if(_timetable->getTransferTimetableBefore(1))
					{
						stream << p1.cell("Libellé tableau :", _timetable->getTransferTimetableBefore(1)->get<Title>());
					}
					stream << p1.close();

					// Search
					TimetableRowTableSync::SearchResult rows(
						TimetableRowTableSync::Search(
							Env::GetOfficialEnv(),
							_timetable->getKey()
							, _requestParameters.orderField == PARAMETER_RANK
							, _requestParameters.raisingOrder
							, _requestParameters.first
							, _requestParameters.maxSize,
							UP_DOWN_LINKS_LOAD_LEVEL
					)	);

					// Available lines
					set<const CommercialLine*> lines;

					if(!rows.empty()) // Old method
					{
						ActionResultHTMLTable::HeaderVector h;
						h.push_back(make_pair(string(), HTMLModule::getHTMLImage("/admin/img/arrow_up.png", "^")));
						h.push_back(make_pair(string(), HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "V")));
						h.push_back(make_pair(PARAMETER_RANK, "Rang"));
						h.push_back(make_pair(string(), "Commune"));
						h.push_back(make_pair(string(), "Arrêt"));
						h.push_back(make_pair(string(), "Arr"));
						h.push_back(make_pair(string(), "Dep"));
						h.push_back(make_pair(string(), "Obl"));
						h.push_back(make_pair(string(), "Sel"));
						h.push_back(make_pair(string(), "Aff"));
						h.push_back(make_pair(string(), "Action"));
						ActionResultHTMLTable t(
							h,
							searchRequest.getHTMLForm(),
							_requestParameters,
							rows,
							addRowRequest.getHTMLForm("addrow"),
							TimetableRowAddAction::PARAMETER_RANK
						);

						stream << t.open();

						optional<size_t> maxRank(TimetableRowTableSync::GetMaxRank(_timetable->getKey()));
						optional<size_t> lastRank;
						BOOST_FOREACH(const boost::shared_ptr<TimetableRow>& row, rows)
						{
							lastRank = row->getRank();
							deleteRowRequest.getAction()->setObjectId(row->getKey());

							if(row->getPlace())
							{
								const StopArea& place(*Env::GetOfficialEnv().get<StopArea>(row->getPlace()->getKey()));
								BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stop, place.getPhysicalStops())
								{
									BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop.second->getDepartureEdges())
									{
										// Prevent from junctionstop cases
										if(dynamic_cast<const LinePhysicalStop*>(edge.second))
										{
											JourneyPattern* jp(dynamic_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
											if(jp)
											{
												lines.insert(
															jp->getCommercialLine()
															);
											}
										}
									}
								}
							}

							stream << t.row(lexical_cast<string>(*lastRank));
							stream << t.col();
							if (*lastRank > 0)
								stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("/admin/img/arrow_up.png", "^"));
							stream << t.col();
							if (maxRank && *lastRank < *maxRank)
								stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "V"));;
							stream << t.col() << *lastRank;
							if(row->getPlace())
							{
								stream << t.col() << row->getPlace()->getCity()->getName();
								stream << t.col() << row->getPlace()->getName();
							}
							else
							{
								stream << t.col(2) << "Arrêt invalide.";
							}
							stream <<
								t.col() <<
								(	row->getIsArrival() ?
									HTMLModule::getHTMLImage("/admin/img/bullet_green.png","Arrivée possible") :
									HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Arrivée impossible")
								)
							;
							stream <<
								t.col() <<
								(	row->getIsDeparture() ?
									HTMLModule::getHTMLImage("/admin/img/bullet_green.png", "Départ possible") :
									HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Départ impossible")
								)
							;
							stream <<
								t.col() <<
								(	(row->getCompulsory() == PassageObligatoire) ?
									HTMLModule::getHTMLImage("/admin/img/bullet_green.png", "Obligatoire") :
									HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Non obligatoire")
								)
							;
							stream <<
								t.col() <<
								(	(row->getCompulsory() == PassageSuffisant) ?
									HTMLModule::getHTMLImage("/admin/img/bullet_green.png", "Suffisant") :
									HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Non suffisant")
								)
							;
							stream <<
								t.col() <<
								(	(false) ?
									HTMLModule::getHTMLImage("/admin/img/bullet_green.png", "Affiché") :
									HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Non affiché")
								)
							;
							stream << t.col() << HTMLModule::getLinkButton(deleteRowRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
						}
						lastRank = lastRank ? *lastRank + 1 : 0;
						stream << t.row(string("0"));
						stream << t.col();
						stream << t.col();
						stream << t.col() << *lastRank;
						stream << t.col() << t.getActionForm().getTextInput(TimetableRowAddAction::PARAMETER_CITY_NAME, string(), "(commune)");
						stream << t.col() << t.getActionForm().getTextInput(TimetableRowAddAction::PARAMETER_PLACE_NAME, string(), "(arrêt)");
						stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_ARRIVAL, string(), true);
						stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_DEPARTURE, string(), true);
						stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_COMPULSORY, string(), false);
						stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_SUFFICIENT, string(), true);
						stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_DISPLAYED, string(), true);
						stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");

						stream << t.close();
					}
					else // New method
					{
						// Row group creation request
						AdminActionFunctionRequest<ObjectCreateAction, TimetableAdmin> rowGroupCreationRequest(_request, *this);
						rowGroupCreationRequest.getAction()->setTable<TimetableRowGroup>();
						rowGroupCreationRequest.getAction()->set<Timetable>(const_cast<Timetable&>(*_timetable));
						rowGroupCreationRequest.getAction()->set<TimetableRowRule>(NecessaryRow);
						rowGroupCreationRequest.getAction()->set<IsDeparture>(true);
						rowGroupCreationRequest.getAction()->set<IsArrival>(true);
						rowGroupCreationRequest.getAction()->set<Display>(true);

						// Row group update request
						AdminActionFunctionRequest<ObjectUpdateAction, TimetableAdmin> rowGroupUpdateRequest(_request, *this);
						
						// Row group item creation request
						AdminActionFunctionRequest<TimetableRowGroupItemAddAction, TimetableAdmin> rowGroupItemCreationRequest(_request, *this);
						
						// Row group item update request
						AdminActionFunctionRequest<ObjectUpdateAction, TimetableAdmin> rowGroupItemUpdateRequest(_request, *this);

						// Deletion request
						AdminActionFunctionRequest<RemoveObjectAction, TimetableAdmin> removeRequest(_request, *this);

						// Table header
						HTMLTable::ColsVector c;
						c.push_back("Grp");
						c.push_back("Lieu");
						c.push_back("Arrivée");
						c.push_back("Départ");
						c.push_back("Rôle");
						c.push_back("Tri");
						c.push_back(string());
						c.push_back(string());
						HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
						stream << t.open();

						// Loop on row groups
						size_t rowGroupRank(0);
						BOOST_FOREACH(const Timetable::RowGroups::value_type& rowGroup, _timetable->getRowGroups())
						{
							//////////////////////////////////////////////////////////////////////////
							// Row group header
							stream << t.row();
							rowGroupRank = rowGroup->get<Rank>();
							rowGroupUpdateRequest.getAction()->setObject(*rowGroup);

							// Rank
							stream << t.col(1, string(), true);
							stream << rowGroup->get<Rank>();

							// Title
							stream << t.col(1, string(), true);
							stream << "Groupe";

							// Arrival
							stream << t.col(1, string(), true);
							rowGroupUpdateRequest.getAction()->set<IsArrival>(
								!rowGroup->get<IsArrival>()
							);
							stream << HTMLModule::getHTMLLink(
								rowGroupUpdateRequest.getURL(),
								rowGroup->get<IsArrival>() ? "A" : "-"
							);
							rowGroupUpdateRequest.getAction()->set<IsArrival>(
								rowGroup->get<IsArrival>()
							);

							// Departure
							stream << t.col(1, string(), true);
							rowGroupUpdateRequest.getAction()->set<IsDeparture>(
								!rowGroup->get<IsDeparture>()
							);
							stream << HTMLModule::getHTMLLink(
								rowGroupUpdateRequest.getURL(),
								rowGroup->get<IsDeparture>() ? "D" : "-"
							);
							rowGroupUpdateRequest.getAction()->set<IsDeparture>(
								rowGroup->get<IsDeparture>()
							);

							// Rule
							stream << t.col(1, string(), true);
							TimetableRowRule::Type val(rowGroup->get<TimetableRowRule>());
							string label;
							switch(val)
							{
							case FirstIsForbidden:
								val = NeutralRow;
								label = "Interdit si en premier";
								break;

							case NeutralRow:
								val = NecessaryRow;
								label = "Neutre";
								break;

							case NecessaryRow:
								val = SufficientRow;
								label = "Nécessaire";
								break;

							case SufficientRow:
								val = FirstIsForbidden;
								label = "Suffisant";
								break;
							}
							rowGroupUpdateRequest.getAction()->set<TimetableRowRule>(val);
							stream << HTMLModule::getHTMLLink(
								rowGroupUpdateRequest.getURL(),
								label
							);
							rowGroupUpdateRequest.getAction()->set<TimetableRowRule>(
								rowGroup->get<TimetableRowRule>()
							);

							// Sorting
							stream << t.col(1, string(), true);
							rowGroupUpdateRequest.getAction()->set<AutoRowsOrder>(
								!rowGroup->get<AutoRowsOrder>()
							);
							stream << HTMLModule::getHTMLLink(
								rowGroupUpdateRequest.getURL(),
								rowGroup->get<AutoRowsOrder>() ? "Auto" : "Fixe"
							);
							rowGroupUpdateRequest.getAction()->set<AutoRowsOrder>(
								rowGroup->get<AutoRowsOrder>()
							);

							// Insertion
							stream << t.col(1, string(), true);
							rowGroupCreationRequest.getAction()->set<Rank>(rowGroupRank);
							stream << HTMLModule::getLinkButton(rowGroupCreationRequest.getURL(), "Insérer");

							// Removal
							stream << t.col(1, string(), true);
							if(rowGroup->getItems().empty	())
							{
								removeRequest.getAction()->setObjectId(rowGroup->getKey());
								stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le groupe ?");
							}

							//////////////////////////////////////////////////////////////////////////
							// Items
							stream << t.row();
							stream << t.col(8);

							size_t itemRank(0);
							rowGroupItemCreationRequest.getAction()->setRowGroup(Env::GetOfficialEnv().getEditableSPtr(rowGroup));
							HTMLForm f(
								rowGroupItemCreationRequest.getHTMLForm(
									"addItem"+ lexical_cast<string>(rowGroup->get<Rank>())
								)
							);
							stream << f.open();

							// Table header
							HTMLTable::ColsVector ct2;
							ct2.push_back(string());
							ct2.push_back("Arrêt");
							ct2.push_back("Rang");
							ct2.push_back("Position d'insertion");
							ct2.push_back("Action");
							HTMLTable t2(ct2);
							stream << t2.open();
							BOOST_FOREACH(const TimetableRowGroup::Items::value_type& item, rowGroup->getItems())
							{
								itemRank = item->get<Rank>();

								// Lines
								if(item->get<StopArea>())
								{
									BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stop, item->get<StopArea>()->getPhysicalStops())
									{
										BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop.second->getDepartureEdges())
										{
											if(dynamic_cast<const LinePhysicalStop*>(edge.second))
											{
												lines.insert(
													(dynamic_cast<const LinePhysicalStop*>(edge.second))->getJourneyPattern()->getCommercialLine()
												);
											}
										}
									}
								}

								// New row
								stream << t2.row();

								// Row group
								stream << t2.col();
								stream << rowGroup->get<Rank>();

								// Place
								stream << t2.col();
								if(item->get<StopArea>())
								{
									stream << item->get<StopArea>()->getFullName();
								}

								// Sorting
								stream << t2.col();
								if(!rowGroup->get<AutoRowsOrder>())
								{
									stream << item->get<Rank>();
								}

								// Insertion
								stream << t2.col();
								stream << f.getRadioInput(
									TimetableRowGroupItemAddAction::PARAMETER_RANK,
									optional<size_t>(item->get<Rank>()),
									optional<size_t>(),
									string(),
									false,
									string()
								); 

								// Removal
								stream << t2.col();
								removeRequest.getAction()->setObjectId(item->get<Key>());
								stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
							}

							//////////////////////////////////////////////////////////////////////////
							// New item at the end
							stream << t2.row();

							// New row
							stream << t2.row();

							// Row group
							stream << t2.col();
							stream << rowGroup->get<Rank>();

							// Place
							stream << t2.col();
							stream << f.getTextInput(TimetableRowGroupItemAddAction::PARAMETER_PLACE_NAME, string());
							stream << f.getSubmitButton("Insérer");

							// Sorting
							stream << t2.col();

							// Insertion
							stream << t2.col();
							stream << f.getRadioInput(
								TimetableRowGroupItemAddAction::PARAMETER_RANK,
								optional<size_t>(itemRank+1),
								optional<size_t>(itemRank+1),
								string(),
								false,
								string()
							); 
							
							// Removal
							stream << t2.col();

							stream << t2.close();
							stream << f.close();
							
						}

						//////////////////////////////////////////////////////////////////////////
						// New group at the end
						stream << t.row();

						// Rank
						stream << t.col(1, string(), true);
						++rowGroupRank;
						stream << rowGroupRank;

						// Title
						stream << t.col(1, string(), true);
						stream << "Groupe";

						// Arrival
						stream << t.col(1, string(), true);

						// Departure
						stream << t.col(1, string(), true);

						// Rule
						stream << t.col(1, string(), true);

						// Sorting
						stream << t.col(1, string(), true);

						// Insertion
						stream << t.col(1, string(), true);
						rowGroupCreationRequest.getAction()->set<Rank>(rowGroupRank);
						stream << HTMLModule::getLinkButton(rowGroupCreationRequest.getURL(), "Insérer");

						// Removal
						stream << t.col(1, string(), true);

						stream << t.close();
					}

					// Transfer after
					transferRequest.getAction()->setBefore(false);
					PropertiesHTMLTable p2(transferRequest.getHTMLForm("t2"));
					stream << p2.open();
					stream << p2.cell(
						"Correspondance après :",
						p2.getForm().getTextInput(
							TimetableTransferUpdateAction::PARAMETER_TRANSFER_TIMETABLE_ID,
							_timetable->getTransferTimetableAfter(1) ? lexical_cast<string>(_timetable->getTransferTimetableAfter(1)->getKey()) : string()
					)	);
					if(_timetable->getTransferTimetableAfter(1))
					{
						stream << p2.cell("Libellé tableau :", _timetable->getTransferTimetableAfter(1)->get<Title>());
					}
					stream << p2.close();

					stream << "<h1>Lignes</h1>";


					HTMLTable::ColsVector c3;
					c3.push_back("Ligne");
					c3.push_back("Action");
					HTMLTable t3(c3, ResultHTMLTable::CSS_CLASS);
					stream << t3.open();

					AdminActionFunctionRequest<TimetableSetLineAction,TimetableAdmin> setLineRequest(_request, *this);
					setLineRequest.getAction()->setTimetable(const_pointer_cast<Timetable>(_timetable));

					BOOST_FOREACH(const CommercialLine* line, lines)
					{
						setLineRequest.getAction()->setLine(Env::GetOfficialEnv().getSPtr<CommercialLine>(line));

						stream << t3.row();
						stream << t3.col() <<
							"<span class=\"line " << line->getStyle() << "\">" <<
							line->getShortName() <<
							"</span>"
							;
						stream << t3.col() <<
							HTMLModule::getHTMLLink(
								setLineRequest.getURL(),
								_timetable->getAuthorizedLines().find(line) == _timetable->getAuthorizedLines().end() ? HTMLModule::getHTMLImage("/admin/img/cross.png", "Non sélectionnée") :  HTMLModule::getHTMLImage("/admin/img/tick.png", "Sélectionnée")
							)
						;

					}

					stream << t3.close();


					if(!rows.empty())
					{
						stream << "<h1>Quais de départ</h1>";


						HTMLTable::ColsVector c4;
						c4.push_back("Quai");
						c4.push_back("Action");
						HTMLTable t4(c4, ResultHTMLTable::CSS_CLASS);
						stream << t4.open();

						AdminActionFunctionRequest<TimetableSetPhysicalStopAction,TimetableAdmin> setStopRequest(_request, *this);
						setStopRequest.getAction()->setTimetable(const_pointer_cast<Timetable>(_timetable));

						StopPointTableSync::SearchResult stops(
							StopPointTableSync::Search(Env::GetOfficialEnv(), rows[0]->getPlace()->getKey())
						);
						BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& stop, stops)
						{
							setStopRequest.getAction()->setPhysicalStop(stop);

							stream << t4.row();
							stream << t4.col() <<
								stop->getName()
							;
							stream << t4.col() <<
								HTMLModule::getHTMLLink(
									setStopRequest.getURL(),
									_timetable->getAuthorizedPhysicalStops().find(stop.get()) == _timetable->getAuthorizedPhysicalStops().end() ? HTMLModule::getHTMLImage("/admin/img/cross.png", "Non sélectionné") :  HTMLModule::getHTMLImage("/admin/img/tick.png", "Sélectionné")
								)
							;

						}

						stream << t4.close();

					}
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB RESULT
			if (openTabContent(stream, TAB_RESULT))
			{
				if(
					_timetable->getContentType() == Timetable::TABLE_SERVICES_IN_COLS ||
					_timetable->getContentType() == Timetable::TABLE_SERVICES_IN_ROWS ||
					_timetable->getContentType() == Timetable::TIMES_IN_COLS ||
					_timetable->getContentType() == Timetable::TIMES_IN_ROWS
				){
					// Building the result
					date now(day_clock::local_day());
					auto_ptr<TimetableGenerator> g(
						_timetable->getGenerator(
							Env::GetOfficialEnv(),
							_timetable->isGenerable() ? _timetable->get<BaseCalendar>()->getResult() : Calendar(date(now.year(), 1, 1), date(now.year(), 12, 31))
					)	);
					const TimetableResult result(g->build(true, boost::shared_ptr<TimetableResult::Warnings>()));

					// Drawing the result
					stream << "<h1>Tableau</h1>";
					HTMLTable tf(0, ResultHTMLTable::CSS_CLASS);
					stream << tf.open();
					for(size_t depth(_timetable->getBeforeTransferTimetablesNumber()); depth > 0; --depth)
					{
						_drawTable(stream, tf, result.getBeforeTransferTimetable(depth), depth, true);
					}
					_drawTable(stream, tf, result);
					for(size_t depth(1); depth <= _timetable->getAfterTransferTimetablesNumber(); ++depth)
					{
						_drawTable(stream, tf, result.getAfterTransferTimetable(depth), depth, false);
					}
					stream << tf.close();

					// Drawing the warnings
					if(	!result.getWarnings().empty()
					){
						stream << "<h1>Renvois</h1>";

						HTMLTable::ColsVector v;
						v.push_back("Num");
						v.push_back("Texte");
						HTMLTable tw(v, ResultHTMLTable::CSS_CLASS);
						stream << tw.open();

						BOOST_FOREACH(const TimetableResult::Warnings::value_type& warn, result.getWarnings())
						{
							stream << tw.row();
							stream << tw.col() << warn.first;
							stream << tw.col() << warn.second->getText();
						}

						stream << tw.close();
					}
				}
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		bool TimetableAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TimetableRight>(READ);
		}



		std::string TimetableAdmin::getTitle() const
		{
			return _timetable.get() ? _timetable->get<Title>() : DEFAULT_TITLE;
		}

		void TimetableAdmin::setTimetable(boost::shared_ptr<Timetable> timetable)
		{
			_timetable = const_pointer_cast<const Timetable, Timetable>(timetable);
		}

		boost::shared_ptr<const Timetable> TimetableAdmin::getTimetable() const
		{
			return _timetable;
		}


		bool TimetableAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return
				(!_timetable.get() && !static_cast<const TimetableAdmin&>(other)._timetable.get()) ||
				(_timetable.get() && static_cast<const TimetableAdmin&>(other)._timetable.get() &&
				 _timetable->getKey() == static_cast<const TimetableAdmin&>(other)._timetable->getKey());
		}



		void TimetableAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			if(_timetable.get())
			{
				_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			}
			_tabs.push_back(Tab("Contenu", TAB_CONTENT, true));
			if(_timetable.get())
			{
				_tabs.push_back(Tab("Résultat", TAB_RESULT, true));
			}

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks TimetableAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const admin::AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const TimetableModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		AdminInterfaceElement::PageLinks TimetableAdmin::getSubPages( const admin::AdminInterfaceElement& currentPage, const server::Request& request ) const
		{
			AdminInterfaceElement::PageLinks links;

			// Subpages
			TimetableTableSync::SearchResult timetables(
				TimetableTableSync::Search(Env::GetOfficialEnv(), _timetable.get() ? _timetable->getKey() : 0)
			);
			BOOST_FOREACH(const boost::shared_ptr<Timetable>& tt, timetables)
			{
				boost::shared_ptr<TimetableAdmin> page(
					getNewPage<TimetableAdmin>()
				);
				page->setTimetable(tt);
				links.push_back(page);
			}

			return links;
		}



		void TimetableAdmin::_drawTable(
			ostream& stream,
			HTMLTable& tf,
			const TimetableResult& result,
			size_t depth,
			bool isBefore
		) const {
			// Title
			stream << tf.row();
			stream << tf.col(result.getColumns().size() + 1, string(), true);
			if(depth == 0)
			{
				stream << "Tableau principal";
			}
			else
			{
				stream << "Tableau de correspondance " << (isBefore ? "avant" : "après") << " #" << depth;
			}

			// Table
			stream << tf.row();
			stream << tf.col(1, string(), true) << "Lignes";

			BOOST_FOREACH(const CommercialLine* line, result.getRowLines())
			{
				if(line == NULL)
				{
					stream << tf.col();
				}
				else
				{
					stream <<
						tf.col(1, line->getStyle()) <<
						line->getShortName()
						;
				}
			}

			const Timetable* timetable(_timetable.get());
			if(depth != 0)
			{
				timetable = isBefore ? timetable->getTransferTimetableBefore(depth) : timetable->getTransferTimetableAfter(depth);
			}
			if(!timetable->getRows().empty())
			{
				BOOST_FOREACH(const Timetable::Rows::value_type& row, timetable->getRows())
				{
					if(!row.getPlace())
					{
						continue;
					}

					stream << tf.row();
					stream << tf.col(1, string(), true) << row.getPlace()->getFullName();
					const TimetableResult::RowTimesVector cols(result.getRowSchedules(row.getRank()));
					BOOST_FOREACH(const TimetableResult::RowTimesVector::value_type& col, cols)
					{
						stream << tf.col();
						if (!col.second.is_not_a_date_time())
						{
							stream << col.second.hours() << ":" << col.second.minutes();
						}
					}
				}
			}
			else // new method
			{
				size_t rk(0);
				BOOST_FOREACH(const pt::StopArea* place, result.getStopAreas())
				{
					stream << tf.row();
					stream << tf.col(1, string(), true) << place->getFullName();
					const TimetableResult::RowTimesVector cols(result.getRowSchedules(rk));
					BOOST_FOREACH(const TimetableResult::RowTimesVector::value_type& col, cols)
					{
						stream << tf.col();
						if (!col.second.is_not_a_date_time())
						{
							stream << col.second.hours() << ":" << col.second.minutes();
						}
					}
					rk++;
				}
			}

			// Notes
			if(depth==0)
			{
				stream << tf.row();
				stream << tf.col(1, string(), true) << "Renvois";
				BOOST_FOREACH(const TimetableResult::RowNotesVector::value_type& warn, result.getRowNotes())
				{
					stream << tf.col();
					if(warn.get())
					{
						stream << warn->getNumber();
					}
				}
			}
		}
	}
}
