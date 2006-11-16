#ifndef SYNTHESE_INTERFACES_INTERFACE_H
#define SYNTHESE_INTERFACES_INTERFACE_H

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "04_time/HourPeriod.h"

#include <string>
#include <vector>
#include <iostream>
#include <map>

namespace synthese
{
    
	namespace interfaces
	{

		class InterfacePage;
		
		/** Interface.
			@author Hugues Romain
			@date 2001-2006
			@ingroup m11
		 
			Cette classe définit les différentes interfaces pour l'affichage de résultats issus de calculs SYNTHESE. Une interface est constituée de modèles d'affichage, permettant la d?finition du graphisme en sortie, fournis ? SYNTHESE en tant que donn?es. Des mod?les de fichiers de sortie doivent ?tre fournis au chargement d'apr?s un format de codage d?fini pour l'occasion.</p>
		 
			La liste des \ref InterfaceObjetsStandard d?finit les objets qui peuvent ?tre d?crits par un mod?le. La fourniture d'un mod?le pour chacun de ces objets est facultative. En cas de demande d'affichage d'un objet pour lequel aucun mod?le n'a ?t? fourni, une page vide sera retourn?e.
			 
			Les mod?les sont d?finis par la classe cElementInterface et ses d?riv?s, et sont ind?x?s dans le tableau de pointeurs vElement, selon une indexation par num?ro d'objet standard, selon la nomenclature des \ref InterfaceObjetsStandard .
			 
			Des constantes permettent de d?signer les index du tableau vElement et sont d?crits dans le fichier Parametres.h.
			 
			Les p?riodes de la journ?e correspondent ? des masques permettant de n'effectuer des calculs qu'entre deux heures de la journ?e (ex: fiche horaire de 10:00 ? 12:00)
			 
			@attention Par convention, la p?riode portant l'index 0 d?crit la journ?e enti?re. Elle doit donc ?tre d?crite obligatoirement par les donn?es pour que l'interface soit valide.
		*/
		class Interface : public synthese::util::Registrable<uid, Interface>
		{
			public:
				/** Période journée */
				static const size_t ALL_DAY_PERIOD;

			private:

				typedef std::map<std::string, InterfacePage*> PagesMap;
				typedef std::map<int, std::string> TextMap;

				PagesMap									_pages;
				std::vector<synthese::time::HourPeriod*>	_hourPeriods;        //!< Tableau des p?riodes de la journ?e
				TextMap										_weekDayNames; //!< Tableau des libell?s des jours de semaine
				TextMap										_monthNames;  //!< Tableau des libell?s des mois
				TextMap										_alertPrefixes;      //!< Pr?fixes de messages d'alerte


			public:

				//! \name Accesseurs
				//@{
					InterfacePage* getPage( const std::string& ) const;

					template <class T>
					const T* getPage() const
					{
						std::string key = Factory<InterfacePage>::getKey<T>();
						return dynamic_cast<const T*>( getPage(key) );
					}
					const synthese::time::HourPeriod* getPeriod( size_t index = ALL_DAY_PERIOD ) const;
					const std::string& getAlertPrefix( int ) const;
					const std::string& getWeekDayName( int ) const;
				//@}

				//! \name Calculateurs
				//@{

				/** Affichage d'une date selon les param?tres de l'interface
				@param __Date Date ? afficher
				@param __Textuel true pour obtenir une date en texte, false pour l'obtenir au format num?rique (d?faut = true)
				@param __JourDeSemaine true pour afficher le jour de la semaine (d?faut = true)
				@param __Annee true pour afficher l'ann?e (d?faut = true)
				@param __Flux stream to write
				*/
/*				void AfficheDate( std::ostream& __Flux, 
					const synthese::time::Date& __Date, 
					bool __Textuel = true, 
					bool __JourDeSemaine = true, 
					bool __Annee = true ) const
				{
					if ( __JourDeSemaine && __Textuel )
						__Flux << _LibelleJourSemaine[ __Date.getWeekDay() ] << " ";
					__Flux << __Date.getDay();
					if ( __Textuel )
						__Flux << " " << _LibelleMois[ __Date.getMonth () ] << " ";
					else
						__Flux << "/" << __Date.getMonth ();
					if ( __Annee )
					{
						if ( __Textuel )
							__Flux << " ";
						else
							__Flux << "/";
						__Flux << __Date.getYear ();
					}
				}
*/				//@}

				//! \name Modifiers
				//@{
					void	addPage(const std::string& key, InterfacePage* page );
					void	removePage( const std::string& page_code );
					void AddPeriode( synthese::time::HourPeriod* );
					bool SetLibelleJour( int, const std::string& );
					bool SetLibelleMois( int, const std::string& );
					void SetPrefixeAlerte( int, const std::string& );
				//@}

				Interface( const uid& id);
				~Interface();
		};


	}
}
/** @} */


#endif
