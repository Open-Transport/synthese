////////////////////////////////////////////////////////////////////////////////
/// ModuleClass class header.
///	@file ModuleClass.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ModuleClass_H__
#define SYNTHESE_ModuleClass_H__

#include <map>
#include <string>

#include "FactoryBase.h"

#include <boost/thread.hpp>

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace server
	{
		class Request;

		////////////////////////////////////////////////////////////////////
		/// Module class.
		///	@ingroup m15
		///
		///	Each module must implement a ModuleClassTemplate subclass and register it by the Generic Factory.
		///	Chosen key indicates the order of loading and can be important in several cases.
		///
		class ModuleClass:
			public util::FactoryBase<ModuleClass>
		{
		public:
			typedef std::map<std::string, std::string> Parameters;

		private:

			typedef void (*PtrCallback) (const std::string& name, const std::string& value);

			static std::map<std::string, PtrCallback> _Callbacks;
			static Parameters _DefaultParameters;
			static Parameters _Parameters;


		public:
			static void SetDefaultParameters(
				const Parameters& parameters
			);
			static bool HasParameter(
				const std::string& name
			);
			static std::string GetParameter(
				const std::string& name,
				const std::string defaultValue = ""
			);
			static const Parameters &GetParameters();
			static void SetParameter(
				const std::string& name,
				const std::string& value,
				bool runCallback = true
			);

		protected:

			static void RegisterParameter(
				const std::string& name,
				const std::string& defaultValue,
				PtrCallback cb
			);
			static void UnregisterParameter(
				const std::string& name
			);


		public:
			virtual void preInit() const = 0;
			virtual void init() const = 0;
			virtual void start() const = 0;
			virtual void end() const = 0;
			virtual const std::string& getName() const = 0;
			virtual void initThread() const = 0;
			virtual void closeThread() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Adds parameters to send to the display template for admin pages.
			/// The default implementation does nothing. Overload the method to
			/// add such a feature.
			/// @param map the map to update
			/// @param request current admin request
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.0
			virtual void addAdminPageParameters(
				util::ParametersMap& map,
				const server::Request& request
			) const;



			virtual void setAdminFromParametersMap(
				const util::ParametersMap& map
			);

			virtual util::ParametersMap getAdminParametersMap(
			) const;

			virtual void displayAdmin(
				std::ostream& stream,
				const server::Request& request,
				const admin::AdminInterfaceElement& currentPage
			) const;
		};
}	}
#endif // SYNTHESE_ModuleClass_H__
