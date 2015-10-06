
/** DRTArea class header.
	@file DRTArea.hpp

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

#ifndef SYNTHESE_pt_DRTArea_hpp__
#define SYNTHESE_pt_DRTArea_hpp__

#include "Object.hpp"
#include "Vertex.h"

#include "PointersSetField.hpp"
#include "StopArea.hpp"
#include "StringField.hpp"

namespace synthese
{
	FIELD_POINTERS_VECTOR(Stops, pt::StopArea)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(Name),
		FIELD(Stops)
	> DRTAreaSchema;

	namespace pt
	{
		/** DRTArea class.
			@ingroup m35
		*/
		class DRTArea:
			public Object<DRTArea, DRTAreaSchema>,
			virtual public util::Registrable
		{
		public:
			static const std::string TAG_STOP;

			DRTArea(
				const util::RegistryKeyType id = 0,
				std::string name = std::string(),
				Stops::Type stops = Stops::Type()
			);
			virtual ~DRTArea();

			virtual graph::GraphIdType getGraphType() const;

			virtual std::string getRuleUserName() const;

			bool contains(const StopArea& stopArea) const;

			virtual std::string getName() const { return get<Name>(); }

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();

			virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix /* = std::string */) const;

			virtual void removeStopArea(pt::StopArea* stopArea);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_DRTArea_hpp__
