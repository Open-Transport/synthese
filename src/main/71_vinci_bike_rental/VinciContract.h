
#ifndef SYNTHESE_VinciContract_H__
#define SYNTHESE_VinciContract_H__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciContract : public util::Registrable<uid, VinciContract>
		{
		private:
			uid	_userId;

		public:
			VinciContract(uid id=0);

			void setUserId(uid id);

			uid getUserId() const;

			friend class db::SQLiteTableSyncTemplate<VinciContract>;
		};
	}
}

#endif // SYNTHESE_VinciContract_H__
