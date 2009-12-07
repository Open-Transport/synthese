
/** AddressablePlace class header.
	@file AddressablePlace.h

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

#ifndef SYNTHESE_ENV_ADDRESSABLEPLACE_H
#define SYNTHESE_ENV_ADDRESSABLEPLACE_H

#include <vector>

#include "Place.h"
#include "Hub.h"

namespace synthese
{
	namespace graph
	{
		class Vertex;
	}

	namespace road
	{
		class Address;
		
		/** Addressable place base class.

			AddressablePlace is the base for any place which can provide
			addresses, i.e. it contains vertices which could be found at a path
			traversing.

			@ingroup m34
		*/
		class AddressablePlace
		:	public virtual geography::Place,
			public graph::Hub
		{
		public:
			typedef std::vector<const Address*> Addresses;

			typedef std::map<
				std::pair<
					util::RegistryKeyType,
					util::RegistryKeyType
				>, boost::posix_time::time_duration
			> TransferDelaysMap;

		protected:
			//! @name Content
			//@{
				Addresses _addresses; 
			//@}

			//! @name Transfer parameters
			//@{
				bool					_allowedConnection;
				TransferDelaysMap		_transferDelays; //!< Transfer delays between vertices (in minutes)
				boost::posix_time::time_duration	_defaultTransferDelay;
			//@}

			//! @name Caching
			//@{
				mutable boost::posix_time::time_duration _minTransferDelay;
			//@}

			AddressablePlace(
				bool allowedConnection= false,
				boost::posix_time::time_duration defaultTransferDelay = boost::posix_time::time_duration()
			);

		public:

			virtual ~AddressablePlace ();

			//! @name Getters
			//@{
				/** Gets addresses of this place.
				 */
				const Addresses&				getAddresses () const;
				boost::posix_time::time_duration	getDefaultTransferDelay() const;
				virtual boost::posix_time::time_duration	getMinTransferDelay() const;
				bool	getAllowedConnection() const;
				const TransferDelaysMap& getTransferDelays() const;
			//@}

			//! @name Setters
			//@{
				void setDefaultTransferDelay(
					boost::posix_time::time_duration defaultTransferDelay
				);
				void	setAllowedConnection(bool value);
			//@}

			//! @name Update methods.
			//@{
				void addTransferDelay(
					TransferDelaysMap::key_type::first_type departure,
					TransferDelaysMap::key_type::second_type arrival,
					boost::posix_time::time_duration transferDelay
				);
				void addForbiddenTransferDelay(
					TransferDelaysMap::key_type::first_type departure,
					TransferDelaysMap::key_type::second_type arrival
				);
				void clearTransferDelays ();
			//@}

			//! @name Query methods
			//@{
				virtual bool isConnectionAllowed(
					const graph::Vertex& fromVertex
					, const graph::Vertex& toVertex
				) const;

				virtual boost::posix_time::time_duration getTransferDelay(
					const graph::Vertex& fromVertex
					, const graph::Vertex& toVertex
				) const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					graph::GraphIdType whatToSearch,
					const graph::Vertex& origin
				) const;

				virtual graph::HubScore getScore() const;

				virtual const geometry::Point2D& getPoint() const;

				virtual bool containsAnyVertex(graph::GraphIdType graphType) const;
			//@}



			//! @name Update methods.
			//@{
				/** Adds an address to this place.
					@param address Address to add
					This methods cancels the caching of the isobarycentre
				 */
				virtual void addAddress (const Address* address);
			//@}
			
			static const AddressablePlace* GetPlace(const graph::Hub* hub);
		};
	}
}

#endif 	    
