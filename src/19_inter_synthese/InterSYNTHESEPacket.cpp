
/** InterSYNTHESEPacket class implementation.
	@file InterSYNTHESEPacket.cpp

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

#include "InterSYNTHESEPacket.hpp"

#include "Factory.h"
#include "InterSYNTHESESyncTypeFactory.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace inter_synthese
	{
		const string InterSYNTHESEPacket::FIELDS_SEPARATOR = ":";
		const string InterSYNTHESEPacket::SYNCS_SEPARATOR = "\r\n";



		InterSYNTHESEPacket::InterSYNTHESEPacket()
		{}



		InterSYNTHESEPacket::BadPacketException::BadPacketException():
			synthese::Exception("Malformed Inter-SYNTHESE packet")
		{}

			
			
		InterSYNTHESEPacket::InterSYNTHESEPacket(
			const std::string& content
		){
			size_t i(0);
			while(i < content.size())
			{
				Data::mapped_type item;

				// ID + Search for next :
				size_t l=i;
				for(; i < content.size() && content[i] != FIELDS_SEPARATOR[0]; ++i) ;
				if(i == content.size())
				{
					throw BadPacketException();
				}
				RegistryKeyType id(lexical_cast<RegistryKeyType>(content.substr(l, i-l)));
				++i;

				// Synchronizer + Search for next :
				l=i;
				for(; i < content.size() && content[i] != FIELDS_SEPARATOR[0]; ++i) ;
				if(i == content.size())
				{
					throw BadPacketException();
				}
				item.first = content.substr(l, i-l);
				++i;

				// Size + Search for next :
				l=i;
				for(; i < content.size() && content[i] != FIELDS_SEPARATOR[0]; ++i) ;
				if(i == content.size())
				{
					throw BadPacketException();
				}
				size_t contentSize = lexical_cast<size_t>(content.substr(l, i-l));
				++i;

				// Content
				if(i+contentSize > content.size())
				{
					throw BadPacketException();
				}
				item.second = content.substr(i, contentSize);
				i += contentSize + SYNCS_SEPARATOR.size();

				_data.insert(
					make_pair(
						id,
						item
				)	);
			}
		}



		/// @pre !this.empty()
		InterSYNTHESEPacket::IdRange InterSYNTHESEPacket::getIdRange() const
		{
			return make_pair(_data.begin()->first, _data.rbegin()->first);
		}



		bool InterSYNTHESEPacket::empty() const
		{
			return _data.empty();
		}



		size_t InterSYNTHESEPacket::size() const
		{
			return _data.size();
		}



		void InterSYNTHESEPacket::load(
			InterSYNTHESEIdFilter* idFilter
		) const	{
			// Local variables
			auto_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE;
			string lastFactoryKey;

			// Reading the content
			BOOST_FOREACH(const Data::value_type& item, _data)
			{
				try
				{
					const string& factoryKey(item.second.first);
					if(factoryKey != lastFactoryKey)
					{
						if(interSYNTHESE.get())
						{
							interSYNTHESE->closeSync();
						}
						interSYNTHESE.reset(
							Factory<InterSYNTHESESyncTypeFactory>::create(factoryKey)
						);
						lastFactoryKey = factoryKey;
						interSYNTHESE->initSync();
					}

					interSYNTHESE->sync(
						item.second.second,
						idFilter
					);
				}
				catch(...)
				{
					// Log
				}
			}
			if(interSYNTHESE.get())
			{
				interSYNTHESE->closeSync();
			}

		}
}	}
