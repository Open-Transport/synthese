#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

#include "04_time/Date.h"
#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include <iostream>
#include <string>

namespace synthese
{
	namespace env
	{
		class Environment;
	}
	namespace interfaces
	{
		class Interface;
	}

	namespace server
	{

		/** Access site.
			@ingroup m11
			@author Hugues Romain
			@date 2005-2006
		*/
		class Site : public synthese::util::Registrable<uid, Site>
		{
			//! \name Properties
			//@{
				std::string				_name;  //!< Name of the site
				synthese::time::Date	_startValidityDate;   
				synthese::time::Date	_endValidityDate;   
				std::string				_clientURL;   //!< URL du binaire client (pour fabrication de liens) : see if it cannot be provided by the client itself
			//@}

			//! \name Parameters
			//@{
				const synthese::env::Environment*	_env;  
				const interfaces::Interface*			_interface;
			//@}

			//! \name Filters
			//@{
				bool _onlineBookingAllowed;
				bool _pastSolutionsDisplayed;
			//@}

		public:
			//! \name Constructeur
			//@{
			Site( const uid& uid);
			//@}

			//! \name Modificateurs
			//@{
				void setEnvironment ( synthese::env::Environment* environment);
				void setInterface ( const synthese::interfaces::Interface* interf);
				void setStartDate ( const synthese::time::Date& dateDebut );
				void setEndDate ( const synthese::time::Date& dateFin );
				void setOnlineBookingAllowed ( const bool valeur );
				void setClientURL ( const std::string& clientURL);
				void setPastSolutionsDisplayed ( bool );
				void setName(const std::string& name);
			//@}

			//! \name Accesseurs
			//@{
//			const std::string& getClef() const;
			const synthese::env::Environment* getEnvironment() const;
//			const std::string& getIdentifiant() const;
			const interfaces::Interface* getInterface() const;
//			const synthese::time::Date& getDateDebut() const;
//			const synthese::time::Date& getDateFin() const;
			const std::string& getClientURL() const;
			bool onlineBookingAllowed() const;
//			bool getSolutionsPassees() const;
			//@}

			//! \name Calculateurs
			//@{
			bool dateControl() const;
			//@}


		};
	}
}

#endif
