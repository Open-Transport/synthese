

//////////////////////////////////////////////////////////////////////////
/// PTStopsImportWizardAdmin class header.
///	@file PTStopsImportWizardAdmin.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_PTStopsImportWizardAdmin_H__
#define SYNTHESE_PTStopsImportWizardAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "GeoPoint.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace pt
	{
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		/// 35.14 Admin : Data source edition and use.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		/// @todo Move streets import screen into road module
		//////////////////////////////////////////////////////////////////////////
		/// Key : PTStopsImportWizardAdmin
		/// General parameters :
		///	<ul>
		///		<li>roid : id of the datasource to display</li>
		///	</ul>
		/// The stops import wizard opens a file and shows the differences
		/// between the database and allow to solve them by several action buttons.
		///
		class PTStopsImportWizardAdmin:
			public admin::AdminInterfaceElementTemplate<PTStopsImportWizardAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_BAHNHOF_FILE_NAME;
				static const std::string PARAM_KOORDS_FILE_NAME;
				static const std::string TAB_PROPERTIES;
				static const std::string TAB_IMPORT;
				static const std::string TAB_IMPORT_STOPS;
			//@}

			struct Bahnhof 
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				geography::GeoPoint coords;
				boost::shared_ptr<StopPoint> stop;
			};
			typedef std::map<std::string, Bahnhof> Bahnhofs;

		private:
			/// @name Search parameters
			//@{
				std::string				_bahnhofFileName;
				std::string				_koordsFileName;
			//@}

			boost::shared_ptr<const impex::DataSource> _dataSource;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			/// @author Hugues
			/// @date 2009
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2010
			PTStopsImportWizardAdmin();
			
			
			void setDataSource(boost::shared_ptr<const impex::DataSource> value){ _dataSource = value; }
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2010
			void setFromParametersMap(
				const server::ParametersMap& map
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2010
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param variables Environment variables defined by the interface
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2010
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
				const admin::AdminRequest& _request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2010
			bool isAuthorized(
				const security::User& user
			) const;


			
			
			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues Romain
			///	@date 2010
			virtual std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_PTStopsImportWizardAdmin_H__
