//
// C++ Implementation: Hub
//
// Description: 
//
//
// Author: Hugues Romain (RCS) <hugues.romain@reseaux-conseil.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "Hub.h"

namespace synthese
{
	using namespace env;
	
	namespace graph
	{
		const MinutesDuration Hub::FORBIDDEN_TRANSFER_DELAY(99);
		
		MinutesDuration Hub::getMinTransferDelay(
		) const {
			return 0;
		}
				
		bool Hub::isConnectionAllowed(
			const Vertex* fromVertex
			, const Vertex* toVertex
		) const {
			return false;
		}

		MinutesDuration Hub::getTransferDelay(
			const Vertex* fromVertex
			, const Vertex* toVertex
		) const {
			return 0;
		}

		HubScore Hub::getScore(
		) const {
			return 1;
		}
	}
}
