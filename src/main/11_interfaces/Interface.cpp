
#include "Interface.h"
#include "InterfacePage.h"
#include <string>

namespace synthese
{
	using namespace std;

	namespace interfaces
	{

		const size_t Interface::ALL_DAY_PERIOD = 0;


		Interface::Interface( const uid& id )
			: synthese::util::Registrable<uid,synthese::interfaces::Interface> (id)
		{
		}


		Interface::~Interface()
		{
		}


		InterfacePage* Interface::getPage(const std::string& index) const
		{
			PagesMap::const_iterator it = _pages.find(index);
			return it == _pages.end() ? NULL : it->second;
		}



		/**
		* Modificateur des pr?fixes de messages d'alerte
		* \param __Niveau Niveau des messages sur lesquels seront appliqu?s le pr?fixe
		* \param __Message Pr?fixe ? ajouter aux messages du niveau sp?cifi?
		* \return true si le message a bien ?t? enregistr? (test sur la taille de texte enregistr? uniquement)
		*/
		void Interface::SetPrefixeAlerte( int __Niveau, const std::string& __Message )
		{
			_alertPrefixes.insert(std::pair<int, std::string>(__Niveau, __Message));
		}



		/** Accesseur préfixe de message d'alerte.
			@param __Niveau Niveau d'alerte du message
			@return Le préfixe à apposer au message d'alerte
		*/
		const std::string& Interface::getAlertPrefix( int __Niveau ) const
		{
			TextMap::const_iterator it = _alertPrefixes.find(__Niveau);
			return it == _alertPrefixes.end() ? "" : it->second;
		}


		/*! \brief Accesseur pointeur vers période de la journée avec contrôle
		\param __Index Numéro de la période
		\author Hugues Romain
		\date 2005
		*/
		const synthese::time::HourPeriod* Interface::getPeriod( size_t __Index ) const
		{
			return ( __Index < _hourPeriods.size() ) ? _hourPeriods.at( __Index ) : NULL;
		}


		void Interface::AddPeriode( synthese::time::HourPeriod* __Element )
		{
			_hourPeriods.push_back( __Element );
		}



		/*! \brief Modificateur libellé jour de semaine
		\param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
		\param __Libelle Libellé du jour de la semaine
		\return true si la modification a été effectuée avec succès
		\author Hugues Romain
		\date 2005
		*/
		bool Interface::SetLibelleJour( int __Index, const std::string& __Libelle )
		{
			if ( __Index < 0 || __Index >= synthese::time::DAYS_PER_WEEK )
				return false;

			_weekDayNames[ __Index ] = __Libelle;
			return true;
		}



		/*! \brief Modificateur libellé mois
		\param __Index Index du mois
		\param __Libelle Libellé du mois
		\return true si la modification a été effectuée avec succès
		\author Hugues Romain
		\date 2005
		*/
		bool Interface::SetLibelleMois( int __Index, const std::string& __Libelle )
		{
			if ( __Index < 0 || __Index > synthese::time::MONTHS_PER_YEAR )
				return false;

			_monthNames[ __Index ] = __Libelle;
			return true;
		}


		/** Accesseur libellé de jour de la semaine.
			\param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
			\return Libellé du jour demandé
			\author Hugues Romain
			\date 2005-2006
		*/
		const std::string& Interface::getWeekDayName(int __Index) const
		{
			TextMap::const_iterator it = _weekDayNames.find(__Index);
			return it == _weekDayNames.end() ? NULL : it->second;
		}

		void Interface::addPage(const string& code, InterfacePage* page )
		{
			_pages.insert(make_pair( code, page ));
		}

		void Interface::removePage( const std::string& page_code )
		{
			/** @todo Add a removal of each link to the page */
			PagesMap::const_iterator it = _pages.find(page_code);
			if (it != _pages.end())
			{
				delete it->second;
				_pages.erase( page_code );
			}
		}


	}
}
