
/** InterSYNTHESEConfigItem class header.
	@file InterSYNTHESEConfigItem.hpp

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

#ifndef SYNTHESE_inter_synthese_InterSYNTHESEConfigItem_hpp__
#define SYNTHESE_inter_synthese_InterSYNTHESEConfigItem_hpp__

#include "InterSYNTHESEConfig.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "Object.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESESyncTypeFactory;

		FIELD_STRING(SyncPerimeter)
		FIELD_BOOL(NonPersistent)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(InterSYNTHESEConfig),
			FIELD(SyncType),
			FIELD(SyncPerimeter),
			FIELD(NonPersistent)
		> InterSYNTHESEConfigItemRecord;



		/** InterSYNTHESEConfigItem class.
			@ingroup m19
		*/
		class InterSYNTHESEConfigItem:
			public Object<InterSYNTHESEConfigItem, InterSYNTHESEConfigItemRecord>
		{
		public:
			typedef util::Registry<InterSYNTHESEConfigItem> Registry;

		private:
			mutable boost::shared_ptr<InterSYNTHESESyncTypeFactory> _interSYNTHESE;

		public:
			InterSYNTHESEConfigItem(
				util::RegistryKeyType id = 0
			);

			//! @name Services
			//@{
				const InterSYNTHESESyncTypeFactory& getInterSYNTHESE() const;

				bool mustBeEnqueued(
					const InterSYNTHESESyncTypeFactory& type,
					const std::string& contentPerimeter
				) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_inter_synthese_InterSYNTHESEConfigItem_hpp__

