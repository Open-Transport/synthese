////////////////////////////////////////////////////////////////////////////////
/// AlarmRecipientTemplate class header.
///	@file AlarmRecipientTemplate.h
///	@author Hugues Romain
///	@date 2008-12-26 21:19
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

#ifndef SYNTHESE_AlarmRecipientTemplate_h__
#define SYNTHESE_AlarmRecipientTemplate_h__

#include <set>
#include <map>

#include "AlarmObjectLink.h"
#include "AlarmRecipient.h"
#include "FactorableTemplate.h"
#include "SentScenario.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace messages
	{
		/** AlarmRecipientTemplate class.
			@ingroup m17
		*/
		template<class C>
		class AlarmRecipientTemplate:
			public util::FactorableTemplate<AlarmRecipient, C>
		{
		private:

		protected:
			virtual const std::string& getTitle() const;

		public:
			static const std::string TITLE;


			static void GetParametersLabels(security::ParameterLabelsVector& m);
			virtual void getParametersLabels(security::ParameterLabelsVector& m) const { GetParametersLabels(m); }

			static util::RegistryKeyType GetObjectIdBySource(
				const impex::DataSource& source,
				const std::string& key,
				util::Env& env
			);

			virtual util::RegistryKeyType getObjectIdBySource(
				const impex::DataSource& source,
				const std::string& key,
				util::Env& env
			) const;
		};



		template<class C>
		util::RegistryKeyType synthese::messages::AlarmRecipientTemplate<C>::getObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		) const {
			return GetObjectIdBySource(source, key, env);
		}



		template<class C>
		const std::string& AlarmRecipientTemplate<C>::getTitle() const
		{
			return TITLE;
		}



}	}

#endif // SYNTHESE_AlarmRecipientTemplate_h__
