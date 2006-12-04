
#ifndef SYNTHESE_VinciBike_H__
#define SYNTHESE_VinciBike_H__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h

namespace synthese
{
	namespace vinci
	{
		class VinciBike : public util::Registrable<uid, VinciBike>
		{
		private:
			std::string _number;
			std::string _markedNumber;

		public:
			VinciBike(uid id=0);

			friend class SQLiteTableSync<VinciBike>;
		};
	}
}

#endif // SYNTHESE_VinciBike_H__
