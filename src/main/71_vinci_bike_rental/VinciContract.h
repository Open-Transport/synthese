
#ifndef SYNTHESE_VinciContract_H__
#define SYNTHESE_VinciContract_H__

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace db
	{
		class SQLiteThreadExec;
	}

	namespace security
	{
		class User;
	}

	namespace vinci
	{
		/** Vinci bike rent contract.

			@warning the cached user belongs to the contract and is deleted with the contract
		*/
		class VinciContract : public util::Registrable<uid, VinciContract>
		{
		private:
			uid	_userId;
			security::User* _user;

		public:
			VinciContract(uid id=0);
			~VinciContract();

			void setUserId(uid id);

			uid getUserId() const;
			security::User* getUser() const;

			friend class db::SQLiteTableSyncTemplate<VinciContract>;
			friend class VinciContractTableSync;
		};
	}
}

#endif // SYNTHESE_VinciContract_H__

