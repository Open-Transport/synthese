#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

#include "04_time/Date.h"
#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include <iostream>
#include <string>

#define TEMPS_MIN_CIRCULATIONS 'r'
#define TEMPS_MAX_CIRCULATIONS 'R'

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
//			const std::string& getURLClient() const;
			bool onlineBookingAllowed() const;
//			bool getSolutionsPassees() const;
			//@}

			//! \name Calculateurs
			//@{
			bool dateControl() const;

				/** Date interpr�t�e en fonction d'un texte descriptif et des donn�es de l'environnement.
					@param text Texte contenant la date ou la commande
					@return La date interpr�t�e d'apr�s le texte
					@author Hugues Romain
					@date 2005-2006
					@warning Les param�tres ne sont pas contr�l�s

					La date retourn�e est la suivante, selon le texte entr� :
						- date au format texte interne : date transcrite (pas de contr�le) (ex : 20070201 => 1/2/2007)
						- commande de date classique (synthese::time::TIME_MIN ('m'), synthese::time::TIME_MAX ('M'), synthese::time::TIME_CURRENT ('A'), synthese::time::TIME_UNKNOWN ('?')) : la date correspondante (voir synthese::time::Date::setDate())
						- texte vide : identique � synthese::time::TIME_CURRENT
						- synthese::time::TIME_MIN_CIRCULATIONS ('r') : Premi�re date o� circule au moins un service (voir cEnvironnement::DateMinReelle())
						- TEMPS_MAX_CIRCULATIONS ('R') : Derni�re date o� circule au moins un service (voir cEnvironnement::DateMaxReelle())

					The following assertion is always assumed : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
				*/
				synthese::time::Date dateInterpretee( const std::string& text ) const;

			//@}


		};
	}
}

#endif
