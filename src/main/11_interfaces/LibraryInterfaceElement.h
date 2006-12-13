#ifndef SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H

#include "DisplayableElement.h"
#include "01_util/Factorable.h"

using synthese::util::Factory;

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;

		/** element which can produce a single display.
			@ingroup m11
		*/
		class LibraryInterfaceElement : public DisplayableElement, public util::Factorable
		{
		protected:

			/** Constructor.
			*/
			LibraryInterfaceElement()
				: DisplayableElement() 
			{ }

			virtual void storeParameters(ValueElementList& vel) = 0;

		public:

			void parse( const std::string& text );

			static LibraryInterfaceElement* create( const std::string & text );

			template<class T>
			LibraryInterfaceElement* copy( const ParametersVector& parameters )
			{
				return new T(*((T*) this));
			}

		};
	}
}


/*	case ELEMENTINTERFACELienPhotosArretPhysiques:
			/*   // Initialisation des paramï¿½tres
			const ConnectionPlace* __ArretLogique = (const ConnectionPlace*) __Objet;
			std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesDebut]->Texte(__Parametres);
			std::string __TexteLibelle = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesLibelleDefaut]->Texte(__Parametres);
			std::string __TextePied = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesFin]->Texte(__Parametres);

			for (int __ArretPhysique = 1; __ArretPhysique <= __ArretLogique->getPhysicalStops().size(); __ArretPhysique++)
			{
			pCtxt << __TexteEnTete << "<a href=\"javascript:showdiv('Q" << __ArretPhysique << "');\">";
			if (__ArretLogique->getArretPhysique(__ArretPhysique)->getNom().size () == 0)
			pCtxt << __TexteLibelle << __ArretPhysique;
			else
			pCtxt << __ArretLogique->getArretPhysique(__ArretPhysique)->getNom();
			pCtxt << "</a>" << __TextePied;
			}
			*/  

/*	case ELEMENTINTERFACELienAutresPhotos:
			/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
			std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienAutresPhotosOuverture]->Texte(__Parametres);
			std::string __TexteLienDebut = _Parametres[ELEMENTINTERFACELienAutresPhotosDebut]->Texte(__Parametres);
			std::string __TexteLienFin = _Parametres[ELEMENTINTERFACELienAutresPhotosFin]->Texte(__Parametres);
			std::string __TextePied = _Parametres[ELEMENTINTERFACELienAutresPhotosFermeture]->Texte(__Parametres);

			if (ArretLogique->NombrePhotos())
			{
			pCtxt << __TexteEnTete;
			for (int iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
			pCtxt << __TexteLienDebut << "<a href=\"javascript:showdiv('P" << iArretPhysique << "');\">" << ArretLogique->getPhoto(iArretPhysique)->DescriptionLocale() << "</a>" << __TexteLienFin;
			pCtxt << __TextePied;
			}
			*/  

/*	case ELEMENTINTERFACELienServices:
		{
			/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
			std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienServicesOuverture]->Texte(__Parametres);
			std::string __TexteLienDebut = _Parametres[ELEMENTINTERFACELienServicesDebut]->Texte(__Parametres);
			std::string __TexteLienFin = _Parametres[ELEMENTINTERFACELienServicesFin]->Texte(__Parametres);
			std::string __TextePied = _Parametres[ELEMENTINTERFACELienServicesFermeture]->Texte(__Parametres);

			if (ArretLogique->GetService(0))
			{
			pCtxt << __TexteEnTete;
			for (int iArretPhysique=0; ArretLogique->GetService(iArretPhysique); iArretPhysique++)
			{
			pCtxt << __TexteLienDebut;
			if (ArretLogique->GetService(iArretPhysique)->getPhoto())
			pCtxt << "<a href=\"javascript:showdiv('S" << TXT(iArretPhysique) << "');\">";
			pCtxt << ArretLogique->GetService(iArretPhysique)->Designation();
			if (ArretLogique->GetService(iArretPhysique)->getPhoto())
			pCtxt << "</a>" << __TexteLienFin;
			}
			pCtxt << __TextePied;
			}
			*/  


/*	case ELEMENTINTERFACEDivArretPhysiques:
			/*   // Collecte des paramï¿½tres
			const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
			const Environment* __Environnement = __Site->getEnvironnement();

			// Affichage
			const cPhoto* curPhoto;
			int iArretPhysique2;
			for (int iArretPhysique=1; iArretPhysique <= ArretLogique->NombreArretPhysiques(); iArretPhysique++)
			{
			pCtxt << "<div id=\"Q" << iArretPhysique << "\" class=\"Q\">";
			curPhoto = ArretLogique->getArretPhysique(iArretPhysique)->getPhoto();
			if (curPhoto)
			{
			pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
			pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
			pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MQ" << TXT(iArretPhysique) << "\" />";
			//! \todo METTRE A JOUR
			//      pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
			pCtxt << "<map name=\"MQ" << iArretPhysique << "\">";
			for (int iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
			{
			pCtxt << "<area shape=\"rect\" alt=\"";
			pCtxt << curPhoto->Map(iMapPhoto).Alt();
			pCtxt << "\" coords=\"";
			pCtxt << curPhoto->Map(iMapPhoto).Coords();
			if (curPhoto->Map(iMapPhoto).LienPhoto() == 0)
			pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map(iMapPhoto).URL() << "\">";
			else
			{
			pCtxt << "\"";
			for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreArretPhysiques(); iArretPhysique2++)
			if (ArretLogique->getArretPhysique(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('Q" << iArretPhysique2 << "')\"";
			break;
			}
			for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos(); iArretPhysique2++)
			if (ArretLogique->getPhoto(iArretPhysique2) == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).
			LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('P" << iArretPhysique2 << "')\"";
			break;
			}
			for (iArretPhysique2=0; ArretLogique->GetService(iArretPhysique2); iArretPhysique2++)
			if (ArretLogique->GetService(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('S" << iArretPhysique2 << "')\"";
			break;
			}
			pCtxt << " />";
			}
			}
			pCtxt << "</map>";
			}
			pCtxt << "</div>";
			}
			*/  

/*	case ELEMENTINTERFACEDivPhotos:
			/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
			const Environment* __Environnement = __Site->getEnvironnement();

			const cPhoto* curPhoto;
			int iArretPhysique2;
			for (int iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
			{
			pCtxt << "<div id=\"P" << TXT(iArretPhysique) << "\" class=\"P\">";
			curPhoto = ArretLogique->getPhoto(iArretPhysique);
			if (curPhoto)
			{
			pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
			pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
			pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MP" << TXT(iArretPhysique) << "\" />";
			//! \todo METTRE A JOUR
			//     pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
			pCtxt << "<map name=\"MP" << TXT(iArretPhysique) << "\">";
			for (int iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
			{
			pCtxt << "<area shape=\"rect\" alt=\"";
			pCtxt << curPhoto->Map(iMapPhoto).Alt();
			pCtxt << "\" coords=\"";
			pCtxt << curPhoto->Map(iMapPhoto).Coords();
			if (curPhoto->Map(iMapPhoto).LienPhoto() == 0)
			pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map(iMapPhoto).URL() << "\">";
			else
			{
			pCtxt << "\"";
			for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreArretPhysiques(); iArretPhysique2++)
			if (ArretLogique->getArretPhysique(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('Q" << iArretPhysique2 << "')\"";
			break;
			}
			for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos(); iArretPhysique2++)
			if (ArretLogique->getPhoto(iArretPhysique2) == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('P" << iArretPhysique2 << "')\"";
			break;
			}
			for (iArretPhysique2=0; ArretLogique->GetService(iArretPhysique2); iArretPhysique2++)
			if (ArretLogique->GetService(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
			{
			pCtxt << " href=\"javascript:showdiv('S" << iArretPhysique2 << "')\"";
			break;
			}
			pCtxt << " />";
			}
			}
			pCtxt << "</map>"; 
			}
			pCtxt << "</div>";
			}
			*/  


/*	case ELEMENTINTERFACEDivServices:
		ConnectionPlace* ArretLogique = (ConnectionPlace*) ObjetAAfficher;
		cPhoto* curPhoto;
		int iArretPhysique2;
		if (ArretLogique->Service != NULL)
		{         
		for (int iArretPhysique=0; ArretLogique->Service[iArretPhysique]!=NULL; iArretPhysique++)
		{
		pCtxt << "<div id=\"S" << (short int) iArretPhysique << "\" class=\"S\">";
		curPhoto = ArretLogique->Service[iArretPhysique]->Photo;
		if (curPhoto != NULL)
		{
		pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
		pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale << "</nobr></th></tr></table>";
		pCtxt << "<img src=\"/photosynthese/" << curPhoto->NomFichier << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MS" << (short int) iArretPhysique << "\" />";
		pCtxt << "<img src=\"/photosynthese/legendephotos.gif\" />";
		pCtxt << "<map name=\"MS" << (short int) iArretPhysique << "\">";
		for (size_t iMapPhoto=0; curPhoto->Map[iMapPhoto]!=NULL; iMapPhoto++)
		{
		pCtxt << "<area shape=\"rect\" alt=\"";
		pCtxt << curPhoto->Map[iMapPhoto]->Alt;
		pCtxt << "\" coords=\"";
		pCtxt << curPhoto->Map[iMapPhoto]->Coords;
		if (curPhoto->Map[iMapPhoto]->LienPhoto==0)
		pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map[iMapPhoto]->URL << "\">";
		else
		{
		pCtxt << "\"";
		for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreVoies; iArretPhysique2++)
		if (ArretLogique->vArretPhysique[iArretPhysique2]->Photo == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
		{
		pCtxt << " href=\"javascript:showdiv('Q" << (short int) iArretPhysique2 << "')\"";
		break;
		}
		for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos; iArretPhysique2++)
		if (ArretLogique->Photo[iArretPhysique2] == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
		{
		pCtxt << " href=\"javascript:showdiv('P" << (short int) iArretPhysique2 << "')\"";
		break;
		}
		if (ArretLogique->Service != NULL)
		for (iArretPhysique2=0; ArretLogique->Service[iArretPhysique2]!=NULL; iArretPhysique2++)
		if (ArretLogique->Service[iArretPhysique2]->Photo == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
		{
		pCtxt << " href=\"javascript:showdiv('S" << (short int) iArretPhysique2 << "')\"";
		break;
		}
		pCtxt << ">";
		}
		}
		pCtxt << "</map>"; 
		}
		pCtxt << "</div>";
		}
		}
		}
		*/

/*	case ELEMENTINTERFACEFicheArretScript:
		{ // A VIRER
			/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
			pCtxt << "<script> function showdiv(s) {";
			//pCtxt << "if (s=='PS') document.all.PS.style.visibility = 'visible'; else document.all.PS.style.visibility = 'hidden';";
			pCtxt << "for (var k=1; k<=" << ArretLogique->NombreArretPhysiques() << "; k++) if (s=='Q'+k) eval(\"document.all.Q\" + k.toInternalString() + \".style.visibility = 'visible';\"); else eval(\"document.all.Q\" + k.toString() + \".style.visibility = 'hidden';\");";
			if (ArretLogique->NombrePhotos())
			pCtxt << "for (k=0; k<" << ArretLogique->NombrePhotos() << "; k++) if (s=='P'+k) eval(\"document.all.P\" + k.toString() + \".style.visibility = 'visible';\"); else eval(\"document.all.P\" + k.toString() + \".style.visibility = 'hidden';\");";
			for (int iService=0; ArretLogique->GetService(iService); iService++)
			if (ArretLogique->GetService(iService)->getPhoto())
			pCtxt << "if (s=='S" << iService << "') eval(\"document.all.S" << iService << ".style.visibility = 'visible';\"); else eval(\"document.all.S" << iService << ".style.visibility = 'hidden';\");";
			pCtxt << "} </script>";
			*/  

/*	case EI_BIBLIOTHEQUE_PERIODE:
		pCtxt << __Site->getInterface() ->GetPeriode( _Parametres[ EI_BIBLIOTHEQUE_PERIODE_INDEX ] ->Nombre( __Parametres ) ) ->getCaption ();
*/


/*	case EI_BIBLIOTHEQUE_Date:
			synthese::time::Date curDate;
			curDate = _Parametres[ EI_BIBLIOTHEQUE_Date_Date ] ->Texte( __Parametres );
			synthese::time::Date& refDate = curDate;
			__Site->getInterface() ->AfficheDate( pCtxt, refDate );
*/
/*	case ELEMENTINTERFACEListeArrets:
			/*   const Environment* __Environnement = __Site->getEnvironnement();
			int n = _Parametres[ELEMENTINTERFACEListeArretsNombre]->Nombre(__Parametres);
			cAccesPADe** tbAccesPADe = __Environnement->getCommune(_Parametres[ELEMENTINTERFACEListeArretsCommune]->Nombre(__Parametres))
			->textToPADe(_Parametres[ELEMENTINTERFACEListeArretsEntree]->Texte(__Parametres),n);
			if (tbAccesPADe[1]==NULL)
			pCtxt << _Parametres[ELEMENTINTERFACEListeArretsMessErreur]->Texte(__Parametres);
			else
			{
			pCtxt << "<script>Nom = new Array; NumPA = new Array; NumDes = new Array;</script>";
			for (int i = 1; i <= n; i++)
			if (tbAccesPADe[i]!=NULL)
			{
			pCtxt << "<script>Nom[" << i << "]=\"" << tbAccesPADe[i]->getNom() << "\";NumDes[" << i << "]=" << tbAccesPADe[i]->numeroDesignation() << "; NumPA[" << i << "]=" << tbAccesPADe[i]->numeroArretLogique() << ";</script>"
			<< _Parametres[ELEMENTINTERFACEListeArretsOuverture]->Texte(__Parametres)
			<< "<a href=\"javascript:MAJ(" << i << ")\">" << tbAccesPADe[i]->getNom() << "</a>"
			<< _Parametres[ELEMENTINTERFACEListeArretsFermeture]->Texte(__Parametres);
			}
			else
			pCtxt << _Parametres[ELEMENTINTERFACEListeArretsTexteSiVide]->Texte(__Parametres);
			}
			*/  

/*		// 26
	case ELEMENTINTERFACETbDepGare:
			/*	// Lecture des parametres
			// mettre une assertion pour detecter qu'on a bien un vector<PassThroughDescription*>
			const std::vector<const PassThroughDescription*>* ptds = reinterpret_cast<const std::vector<const PassThroughDescription*>*> (__Objet);
			assert (ptds != 0);

			int __MultiplicateurRangee = _Parametres[ 0 ] ->Nombre( __Parametres ) ? 
			_Parametres[ 0 ] ->Nombre( __Parametres ) : 1;

			const std::string& __Pages = _Parametres[ 1 ] ->Texte( __Parametres );
			const std::string& __SeparateurPage = _Parametres[ 2 ] ->Texte( __Parametres );

			// Gestion des pages
			int __NombrePages = 1;
			if ( (__Pages == "intermediate" ) || 
			(__Pages == "destination" ) )
			{
			for (int i=0; i<ptds->size (); ++i)
			{
			const PassThroughDescription * ___DP = (*ptds)[i];
			if ( ___DP->getDisplayedConnectionPlaces ().size () - 2 > __NombrePages )
			__NombrePages = ___DP->getDisplayedConnectionPlaces ().size () - 2;
			}
			}

			if (__Pages == "destination" ) __NombrePages++;

			// Boucle sur les pages
			for ( int __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++ )
			{
			// Separateur de page
			if ( __NumeroPage > 1 )
			pCtxt << __SeparateurPage;

			// Boucle sur les rangï¿½es
			int __Rangee = __MultiplicateurRangee;
			for (int i=0; i<ptds->size (); ++i)
			{
			const PassThroughDescription * ___DP = (*ptds)[i];

			// Preparation des paramï¿½tres
			cInterface_Objet_Connu_ListeParametres __ParametresColonne;
			__ParametresColonne << __Rangee;  // 0 : Numero de rangee

			int __NombrePagesRangee = ___DP->getDisplayedConnectionPlaces ().size () - 2 + ( __Pages == "destination" ? 1 : 0 );
			if ( !__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * ( __NombrePages / __NombrePagesRangee ) )
			__ParametresColonne << __NumeroPage;
			else
			__ParametresColonne << 1 + __NumeroPage % __NombrePagesRangee;     // 1 : Numï¿½ro de page

			// Lancement de l'affichage de la rangee
			__Site->Affiche( pCtxt, INTERFACELigneTableauDepart, __ParametresColonne, ( const void* ) ___DP );

			// Incrementation du numï¿½ro de rangï¿½e
			__Rangee += __MultiplicateurRangee;
			}
			}
			*/
/*			// 29 A mettre dans 41
	case ELEMENTINTERFACESelecteurVelo:
			tBool3 VeloDefaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACESelecteurVeloDefaut ] ->Nombre( __Parametres );

			if ( VeloDefaut == Vrai )
				pCtxt << " checked>";
			else
				pCtxt << " >";
*/
/*		// 31 A mettre dans 41
	case ELEMENTINTERFACESelecteurTaxiBus:
			tBool3 TaxiBusDefaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACESelecteurTaxiBusDefaut ] ->Nombre( __Parametres );

			if ( TaxiBusDefaut == Vrai )
				pCtxt << " checked>";
			else
				pCtxt << " >";
*/
		// 34
/*	case ELEMENTINTERFACEChampsReservation:
			// affichage du champ nom
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNom ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_NAME << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOM << "\"/>";
			pCtxt << "</td></tr>";
			/* MJ TODO à laisser en commentaire jusqu'à ecriture de la resa

			// affichage du champ prï¿½nom
			cModaliteReservationEnLigne::FieldNeed bPrenom = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationPrenomObligatoire ] ->Nombre( __Parametres );
			if ( bPrenom == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bPrenom == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationPrenom ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_FIRST_NAME << "\" class=\"frmField\" size=\"35\">";
			if ( bPrenom == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_PRENOM << "\"/>";
			}
			pCtxt << "</td></tr>";
			}

			// affichage du champ adresse
			cModaliteReservationEnLigne::FieldNeed bAdresse = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseObligatoire ] ->Nombre( __Parametres );
			if ( bAdresse == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bAdresse == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresse ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_ADDRESS << "\" class=\"frmField\" size=\"35\">";
			if ( bAdresse == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE << "\"/>";
			}
			pCtxt << "</td></tr>";
			}

			// affichage du champ email
			cModaliteReservationEnLigne::FieldNeed bEmail = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationEmailObligatoire ] ->Nombre( __Parametres );
			if ( bEmail == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bEmail == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationEmail ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_EMAIL << "\" class=\"frmField\" size=\"35\">";
			if ( bEmail == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_EMAIL << "\"/>";
			}
			pCtxt << "</td></tr>";
			}

			// affichage du champ tï¿½lï¿½phone
			cModaliteReservationEnLigne::FieldNeed bTelephone = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationTelephoneObligatoire ] ->Nombre( __Parametres );
			if ( bTelephone == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bTelephone == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationTelephone ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_PHONE << "\" class=\"frmField\" size=\"35\">";
			if ( bTelephone == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_TELEPHONE << "\"/>";
			}
			pCtxt << "</td></tr>";
			}

			// affichage du champ numï¿½ro d'abonnï¿½
			cModaliteReservationEnLigne::FieldNeed bNumAbonne = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationNumeroAbonneObligatoire ] ->Nombre( __Parametres );
			if ( bNumAbonne == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bNumAbonne == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNumeroAbonne ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_REGISTRATION_NUMBER << "\" class=\"frmField\" size=\"35\">";
			if ( bNumAbonne == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NUMERO_ABONNE << "\"/>";
			}
			pCtxt << "</td></tr>";
			}

			// affichage du champ adresse dï¿½part
			cModaliteReservationEnLigne::FieldNeed bAdresseDepart = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseDepartObligatoire ] ->Nombre( __Parametres );
			if ( bAdresseDepart == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresseDepart ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_DEPARTURE_ADDRESS << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_DEPART << "\"/></td></tr>";
			}

			// affichage du champ adresse arrivï¿½e
			cModaliteReservationEnLigne::FieldNeed bAdresseArrivee = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseArriveeObligatoire ] ->Nombre( __Parametres );
			if ( bAdresseArrivee == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
			{
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresseArrivee ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_ARRIVAL_ADDRESS << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_ARRIVEE << "\"/></td></tr>";
			}

			// affichage du champ nombre de places
			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNbPlaces ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_RESERVATION_COUNT << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOMBRE_PLACES << "\"/>";
			pCtxt << "</td></tr>";

			pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationWarning ] ->Texte( __Parametres );
			pCtxt << "</tr>";
			*/


/*	case ELEMENTINTERFACEFeuillesRoute:
			// Collecte des paramï¿½tres
			const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

			// Affichage de chaque feuille de route
			for ( int __n = 0; __n < __Trajets->Taille(); __n++ )
			{
				// Crï¿½ation de la liste de paramï¿½tres
				cInterface_Objet_Connu_ListeParametres __ParametresColonne;

				// 0 : Index du trajet
				__ParametresColonne << __n + 1;

				// 1 : Etat filtre handicapï¿½
				__ParametresColonne << _Parametres[ ELEMENTINTERFACEFeuillesRouteFiltreHandicape ] ->Nombre( __Parametres );

				// 2 : Etat filtre vï¿½lo
				__ParametresColonne << _Parametres[ ELEMENTINTERFACEFeuillesRouteFiltreVelo ] ->Nombre( __Parametres );

				// 3 : Index trajet prï¿½cï¿½dent
				__ParametresColonne << ( __n > 0 ? __n : 0 );

				// 4 : Index trajet suivant
				__ParametresColonne << ( __n < __Trajets->Taille() - 1 ? __n + 2 : 0 );

				// 5 : Date du dï¿½part (format interne)
				__ParametresColonne << __Trajets->operator [] ( __n ).getDepartureTime().getDate().toInternalString ();

				// Lancement de l'affichage
				__Site->Affiche( pCtxt, INTERFACEFeuilleRoute, __ParametresColonne, ( const void* ) & ( __Trajets->operator[] ( __n ) ) );
			}
*/

/*	case EI_BIBLIOTHEQUE_Trajet_Duree:
			// Collecte des paramï¿½tres
			const Journey* __Trajet = ( const Journey* ) __Objet;

			cInterface_Objet_Connu_ListeParametres __Parametres;

			//0 : Durï¿½e du trajet
			__Parametres << __Trajet->getDuration ();

			//1 : Durï¿½e < 1h ?
			__Parametres << ( __Trajet->getDuration () < synthese::time::MINUTES_PER_HOUR ? "1" : "" );

			//2 : Nombre d'heures
			__Parametres << ( __Trajet->getDuration () / synthese::time::MINUTES_PER_HOUR );

			//3 : Nombre de minutes
			__Parametres << ( __Trajet->getDuration () % synthese::time::MINUTES_PER_HOUR );

			//4 : Nombre de minutes superieur a 10 ou inferieur a 60 ?
			__Parametres << ( __Trajet->getDuration () < synthese::time::MINUTES_PER_HOUR || __Trajet->getDuration () % synthese::time::MINUTES_PER_HOUR >= 10 ? "1" : "" );

			// Lancement de l'affichage
			__Site->Affiche( pCtxt, INTERFACEDuree, __Parametres, NULL );
*/
/*	case EI_BIBLIOTHEQUE_FicheHoraire_LigneDurees:
			// Collecte des paramï¿½tres
			const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

			// Affichage de chaque feuille de route
			for ( int __n = 0; __n < __Trajets->Taille(); __n++ )
			{
				cInterface_Objet_Connu_ListeParametres __ParametresCase;
				__ParametresCase << __n + 1;

				__Site->Affiche( pCtxt, INTERFACECaseDuree, __ParametresCase, ( const void* ) & ( __Trajets->operator[] ( __n ) ) );
			}
*/
/*	case ELEMENTINTERFACEURLFormulaire:
			// Initialisation des paramï¿½tres
			const std::string& __TypeSortie = _Parametres[ ELEMENTINTERFACEURLFormulaireTypeSortie ] ->Texte( __Parametres );
			const std::string& __Fonction = _Parametres[ ELEMENTINTERFACEURLFormulaireFonction ] ->Texte( __Parametres );

			// Fabrication de la requï¿½te
			synthese::server::Request request;

			// Site d'affichage
			request.addParameter( synthese::server::PARAMETER_SITE, __Site->getClef() );

			// Parametres cas validation fiche horaire
			if ( __Fonction == "timetable validation" )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_SCHEDULE_SHEET_VALIDATION );

				request.addParameter( synthese::server::PARAMETER_DEPARTURE_CITY_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_CITY_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
			}
			// Pour fiche horaire seulement
			else if ( __Fonction == "timetable" )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, 
					synthese::server::FUNCTION_SCHEDULE_SHEET );

				request.addParameter( synthese::server::PARAMETER_DATE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHDate ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_PERIOD
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHPeriode ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_BIKE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHVelo ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_HANDICAPPED
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHHandicape ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_TAXIBUS
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHResa ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_PRICE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHTarif ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
			}
			else if ( __Fonction == ( "from city list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
			}
			else if ( __Fonction == ( "to city list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
			}
			else if ( __Fonction == ( "from station list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
			}
			else if ( __Fonction == ( "to station list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
			}

			// Affichage de la requï¿½te au format voulu
			if ( __TypeSortie == ( "url" ) )
			{
				// Partie exï¿½cutable de l'url
				pCtxt << __Site->getURLClient() << "?";

				// Champ fonction
				pCtxt << request.toInternalString ();
			}
			else if ( __TypeSortie.substr (0, 4) == "form" )
			{
				// Tag d'ouverture du formulaire
				pCtxt << "<form method=\"get\" action=\"" << __Site->getURLClient() << "\" "
					<< __TypeSortie.substr (5) << ">";

				for ( std::map<std::string, std::string>::const_iterator iter = request.getParameters().begin();
					iter != request.getParameters().end(); 
					++iter )
				{
					pCtxt << "<input type=\"hidden\" name=\"" << iter->first << "\" value=\"" << iter->second << "\" />";
				}
			}
*/
/*	case ELEMENTINTERFACEInputHTML:  //41
			// Initialisation des paramï¿½tres
			const std::string& __Champ = _Parametres[ ELEMENTINTERFACEInputHTMLChamp ] ->Texte( __Parametres );
			const std::string& __Type = _Parametres[ ELEMENTINTERFACEInputHTMLType ] ->Texte( __Parametres );

			// Balise d'ouverture
			std::string __Balise;
			if ( !__Type.size () )
			{
				if ( __Champ == ( "date" ) || __Champ == ( "period" ) )
					__Balise = "select";
				else if ( __Champ == ( "handicap filter" ) )
					__Balise = "input type=\"checkbox\"";
				else
					__Balise = "input type=\"text\"";
			}
			else
			{
				if ( __Type == ( "text" ) )
					__Balise = "input type=\"text\"";
				else if ( __Type == ( "select" ) )
					__Balise = "select";
				else if ( __Type.substr (0, 8) == "checkbox" )
					__Balise = "input type=\"checkbox\"";
			}

			// Ecriture de l'ouverture
			pCtxt << "<" << __Balise << " " << _Parametres[ ELEMENTINTERFACEInputHTMLSuite ] ->Texte( __Parametres );

			// Ecriture du nom du champ
			pCtxt << " name=\"";
			if ( __Champ == "date" )
				pCtxt << synthese::server::PARAMETER_DATE;
			else if ( __Champ == ( "period" ) )
				pCtxt << synthese::server::PARAMETER_PERIOD;
			else if ( __Champ == ( "from city" ) )
				pCtxt << synthese::server::PARAMETER_DEPARTURE_CITY;
			else if ( __Champ == ( "from station" ) )
				pCtxt << synthese::server::PARAMETER_DEPARTURE_STOP;
			else if ( __Champ == ( "to city" ) )
				pCtxt << synthese::server::PARAMETER_ARRIVAL_CITY;
			else if ( __Champ == ( "to station" ) )
				pCtxt << synthese::server::PARAMETER_ARRIVAL_STOP;
			else if ( __Champ == ( "handicap filter" ) )
				pCtxt << synthese::server::PARAMETER_HANDICAPPED;
			else if ( __Champ == ( "tariff" ) )
				pCtxt << synthese::server::PARAMETER_PRICE;
			pCtxt << "\"";

			// Cas champ checkbox
			if ( __Balise == ( "input type=\"checkbox\"" ) )
			{
				// Etat cochï¿½
				tBool3 __Bool3Defaut = Faux;

				// Chackbox filtre sur valeur
				if ( __Type.size () > 9 )
				{
					if ( _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) ==  __Type.substr (9) )
						__Bool3Defaut = Vrai;
					pCtxt << " value=\"" << __Type.substr( 9 ) << "\"";
				}
				else //Checkbox boolï¿½en
				{
					__Bool3Defaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Nombre( __Parametres );
					pCtxt << " value=\"1\"";
				}
				if ( __Bool3Defaut == Vrai )
					pCtxt << " checked=\"1\"";
				pCtxt << " />";
			}
			if ( __Balise == ( "input type=\"text\"" ) )  // Cas champ input texte
			{
				pCtxt << " value=\"" << _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) << "\" />";
			}
			else if ( __Balise == ( "select" ) )  // Cas champ select
			{
				pCtxt << ">";

				// Si select auto alors fourniture des choix
				if ( !__Type.size () )
				{
					if ( __Champ == ( "date" ) )
					{
						// Collecte des paramï¿½tres spï¿½cifiques
						const Environment * __Environnement = __Site->getEnvironnement();
						synthese::time::Date DateDefaut = 
							dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) );
						synthese::time::Date DateMin = dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLListeDatesMin ] ->Texte( __Parametres ) );
						synthese::time::Date __DateJour;
						__DateJour.updateDate();
						if ( !__Site->getSolutionsPassees() && DateMin < __DateJour )
							DateMin = __DateJour;
						synthese::time::Date DateMax = dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLListeDatesMax ] ->Texte( __Parametres ) );

						// Construction de l'objet HTML
						for ( synthese::time::Date iDate = DateMin; iDate <= DateMax; iDate++ )
						{
							pCtxt << "<option ";
							if ( iDate == DateDefaut )
								pCtxt << "selected=\"1\" ";
							pCtxt << "value=\"" << iDate.toInternalString() << "\">";
							__Site->getInterface() ->AfficheDate( pCtxt, iDate );
							pCtxt << "</option>";
						}
					}
					else if ( __Champ == ( "period" ) )
					{
						int __IndexPeriodeDefaut = _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Nombre( __Parametres );

						for ( int iPeriode = 0; __Site->getInterface() ->GetPeriode( iPeriode ); iPeriode++ )
						{
							pCtxt << "<option ";
							if ( iPeriode == __IndexPeriodeDefaut )
								pCtxt << "selected ";
							pCtxt << "value=\"" << iPeriode << "\">" << __Site->getInterface() ->GetPeriode( iPeriode ) ->getCaption () << "</option>";
						}
					}
					pCtxt << "</select>";
				}
			}
*/
/*	case EI_BIBLIOTHEQUE_Interface_PrefixeAlerte:  //42
			// Lecture des paramï¿½tres
			int __Niveau = _Parametres[ EI_BIBLIOTHEQUE_Interface_PrefixeAlerte_Niveau ] ->Nombre( __Parametres );

			// Sortie
			if ( __Niveau > 0 )
				pCtxt << __Site->getInterface() ->getPrefixeAlerte( __Niveau );
*/

/*	case EI_BIBLIOTHEQUE_Ligne_Destination:  //43
			// Lecture des paramï¿½tres
			const Line* __Ligne = ( const Line* ) __Objet;

			// Affichage de la girouette
			if ( __Ligne->getDirection().size () )
			{
				pCtxt << __Ligne->getDirection ();
			}
			else // Affichage du terminus
			{
				// Crï¿½ation d'un objet de paramï¿½tres
				cInterface_Objet_Connu_ListeParametres __ParametresLigne;

				// 0: Dï¿½signation principale
				__ParametresLigne << "0";

				// Lancement de l'affichage
				__Site->Affiche( pCtxt, INTERFACENomArret, __ParametresLigne, ( const void* ) __Ligne->getEdges ().back()->getFromVertex ()->getConnectionPlace() );
			}
*/
/*	case EI_BIBLIOTHEQUE_DescriptionPassage_Ligne:  //44
			/*	const Line* __Ligne = dynamic_cast<const Line*> (( ( const PassThroughDescription* ) __Objet ) ->getLineStop () ->getParentPath ());

			cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
			__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLDebutLigne ] ->Texte( __Parametres );
			__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLFinLigne ] ->Texte( __Parametres );
			__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_LargeurCase ] ->Texte( __Parametres );
			__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HauteurCase ] ->Texte( __Parametres );

			__Site->Affiche( pCtxt, INTERFACECartoucheLigne, __ParametresCaseLigne, ( const void* ) __Ligne );
			*/

			// 45
/*	case EI_BIBLIOTHEQUE_Nombre_Formate:
		{
			// Lecture des paramï¿½tres
			int __Nombre = _Parametres[ 0 ] ->Nombre( __Parametres ) + _Parametres[ 2 ] ->Nombre( __Parametres );
			int __Chiffres = _Parametres[ 1 ] ->Nombre( __Parametres );


			for ( ; __Chiffres; __Chiffres-- )
			{
				if ( __Nombre < pow( 10.0, __Chiffres - 1 ) )
					pCtxt << "0";
			}
			pCtxt << __Nombre;
*/

/*	case EI_BIBLIOTHEQUE_DescriptionPassage_Destination:  //46
			/*	// Lecture des paramï¿½tres
			const PassThroughDescription* __DP = ( const PassThroughDescription* ) __Objet;
			std::string __DestinationsAAfficher = _Parametres[ 0 ] ->Texte( __Parametres );
			bool __AfficherTerminus = ( _Parametres[ 1 ] ->Nombre( __Parametres ) == 1 );
			std::string __TypeAffichage = _Parametres[ 2 ] ->Texte( __Parametres );
			std::string __SeparateurEntreArrets = _Parametres[ 3 ] ->Texte( __Parametres );
			std::string __AvantCommune = _Parametres[ 4 ] ->Texte( __Parametres );
			std::string __ApresCommune = _Parametres[ 5 ] ->Texte( __Parametres );

			const City* __DerniereCommune = __DP->getDisplayedConnectionPlaces () [0]->getCity ();

			for ( int __i = 1; __i < __DP->getDisplayedConnectionPlaces ().size (); __i++ )
			{
			if ( __DestinationsAAfficher == ( "all" ) && ( __i < __DP->getDisplayedConnectionPlaces ().size () - 1 || __AfficherTerminus )
			|| __DestinationsAAfficher == ( "terminus" ) && __i == __DP->getDisplayedConnectionPlaces ().size () - 1
			|| synthese::util::Conversion::ToInt(__DestinationsAAfficher) == __i && ( __i < __DP->getDisplayedConnectionPlaces ().size () - 1 || __AfficherTerminus )
			)
			{
			if ( __i > 1 )
			pCtxt << __SeparateurEntreArrets;

			// Affichage de la commune dans les cas oï¿½ nï¿½cessaire
			if ( __TypeAffichage == ( "station_city" )
			|| __TypeAffichage == ( "station_city_if_new" ) && __DP->getDisplayedConnectionPlaces ()[__i] ->getCity() != __DerniereCommune
			)
			{
			std::stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (synthese::util::LowerCaseFilter());
			out.push (synthese::util::PlainCharFilter());
			out.push (ss);

			out << __DP->getDisplayedConnectionPlaces () [__i] ->getCity () ->getName();

			// std::stringMinuscules __TexteMinuscule;
			// __TexteMinuscule << __DP->GetGare( __i ) ->getTown() ->getName();

			pCtxt << __AvantCommune << ss.str () << __ApresCommune;
			__DerniereCommune = __DP->getDisplayedConnectionPlaces () [__i]->getCity();


			}

			// Affichage du nom d'arrï¿½t dans les cas oï¿½ nï¿½cessaire
			if ( __TypeAffichage.substr (0, 6) == "station" )
			pCtxt << __DP->getDisplayedConnectionPlaces () [__i] ->getName();

			pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getName (); // temporary
			/* TODO hugues // Affichage de la destination 13 caracteres dans les cas ou necessaire
			if ( __TypeAffichage == "char(13)" )
			pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getDesignation13();

			// Affichage de la destination 26 caracteres dans les cas ou necessaire
			if ( __TypeAffichage == "char(26)" )
			pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getDesignation26();
			*/
			/*	    }
			}
			*/ 

/*	case EI_BIBLIOTHEQUE_DescriptionPassage_Heure:  //48
			// Lecture des paramï¿½tres
			/*	const synthese::time::DateTime& __Moment = ( ( const PassThroughDescription* ) __Objet )->getRealMoment ();
			std::string __Zero = _Parametres[ 0 ] ->Texte( __Parametres );
			std::string __AvantSiImminent = _Parametres[ 1 ] ->Texte( __Parametres );
			std::string __ApresSiImminent = _Parametres[ 2 ] ->Texte( __Parametres );

			synthese::time::DateTime __Maintenant;
			__Maintenant.updateDateTime( synthese::time::TIME_CURRENT );
			int __Duree = __Moment - __Maintenant;

			if ( __Duree <= 1 )
			pCtxt << __AvantSiImminent;
			if ( __Moment.getHour ().getHours() < 10 )
			pCtxt << __Zero;
			pCtxt << __Moment.getHour ();
			if ( __Duree <= 1 )
			pCtxt << __ApresSiImminent;
			}
			break;

			case EI_BIBLIOTHEQUE_Cases_Particularites:  //49
			{
			const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

			for ( int __i = 0; __i < __Trajets->Taille(); __i++ )
			{
			// Crï¿½ation d'un objet de paramï¿½tres
			cInterface_Objet_Connu_ListeParametres __ParametresCase;

			//0
			__ParametresCase << __i + 1;

			// 1: Dï¿½signation principale
			__ParametresCase << ( __Trajets->operator [] ( __i ) ).getMaxAlarmLevel ();

			// Lancement de l'affichage
			__Site->Affiche( pCtxt, INTERFACECaseParticularite, __ParametresCase, NULL );

			}
			*/

/*	case EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau:  //50
		{
			// Lecture des paramï¿½tres
			int __Nombre = _Parametres[ 0 ] ->Nombre( __Parametres );
			std::string __Format = _Parametres[ 1 ] ->Texte( __Parametres );

			if ( __Format == "char(2)")
			{
				__Nombre = __Nombre % 100;
				pCtxt << __Nombre / 10;
				pCtxt << __Nombre % 10;
			} */




/** Nom de période de la journée
@code O09 @endcode
@param 0 Numéro de la période à afficher
@return Nom de la période désignée
*/
#define EI_BIBLIOTHEQUE_PERIODE         9


/** Liens vers photos de quais
@code O12 @endcode

Paramètres : 
- Objet LogicalPlace * Point d'arrêt
- 0 Texte du début du lien
- 1 Texte du lien si pas de nom de quai
- 2 Texte de fin du lien
Retour :
- Liens vers photos de quais
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienPhotosArretPhysiques 12


/** Liens vers autres photos
@code O13 @endcode
@param Objet LogicalPlace * : Point d'arr?t
@param 0 Texte du d?but du lien
@param 1 Texte de fin du lien
@param 2 Texte d?ouverture de la zone
@param 3 Texte de fermeture de la zone
@return Liens vers autres photos
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienAutresPhotos      13


/** Liens et description des services
@code O14 @endcode
@param Objet LogicalPlace * : Point d?arr?t
@param 0 Texte du d?but du lien
@param 1 Texte de fin du lien
@param 2 Texte d?ouverture de la zone
@param 3 Texte de fermeture de la zone
@return Liens et description des services
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACELienServices       14


/** Div de quais
@code O17 @endcode
@param Objet LogicalPlace * : Point d'arr?t
@return Div de quais
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivArretPhysiques 17


/** Div de photos
@code O18 @endcode
@param Objet LogicalPlace * : Logical Stop
@return Div de photos
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivPhotos 18


/** Div de services
@code O19 @endcode
@param Objet LogicalPlace * : Logical Stop
@return Div de services
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEDivServices 19


/** Scripts de fiche arr?t
@code O20 @endcode
@param Objet LogicalPlace * : Logical Stop
@return Scripts de fiche arrêt
@deprecated A remettre aux normes
*/
#define ELEMENTINTERFACEFicheArretScript 20


/** Date
@code O22 @endcode
@param 0 Date to display
@return Affichage de la date
*/
#define EI_BIBLIOTHEQUE_Date         22



/** List of stops.
@code 024 @endcode
@param 0 Error message if no stop founded
@param 1 Opening text : will be putted before the link
@param 2 Closing text : will be putted after the link
@param 3 Number of answered stops
@param 4 ID of the city
@param 5 Input text
@param 6 Text to write if empty line
@return List of stops
*/
#define ELEMENTINTERFACEListeArrets      24


/** Nom de Commune
@code O25 @endcode
@param 0 Num?ro de la commune
@return Nom de la commune
*/
#define ELEMENTINTERFACENomCommune  25


/** Tableau de d?part de gare
@code O26 @endcode
<<<<<<< .working
@param Objet cDescriptionPassage *  : Liste des d?parts et arriv?es
@return Tableau de d?parts de gare
=======
@param Objet cDescriptionPassage *  : Liste des d?parts et arriv?es
@param 0 Facteur multiplicateur du num?ro de rang?e transmis ? l'affichage de rang?e (vide=1)
@param 1 Gestion des pages. Valeurs :
- @c normal Une page unique
- @c intermediate Une page par destination interm?diaire
- @c destination Une page par destination interm?diaire et une pour le terminus
@param 2 Texte s?parateur de page
@return Tableau de d?parts de gare
>>>>>>> .merge-right.r440
*/
#define ELEMENTINTERFACETbDepGare 26


/** Departure table.
@code O27 @endcode
@param Objet cDescriptionPassage * : Departures list
@return Departure table.
@deprecated Utilité douteuse (objet 26 avec autre interface suffit)
*/
#define ELEMENTINTERFACETbDep 27

/** S?lecteur du filtre velo
@code O29 @endcode
@param 0 Valeur par d?faut (1/0)
@return HTML ?l?ment de formulaire du filtre v?lo
@todo int?grer cet objet ? l'objet 41
*/
#define ELEMENTINTERFACESelecteurVelo 29

/** S?lecteur du filtre taxi bus
@code O31 @endcode
@param 0 Valeur par d?faut (1/0)
@return HTML ?l?ment de formulaire du filtre taxi bus
@todo int?grer cet objet ? l'objet 41
*/

#define ELEMENTINTERFACESelecteurTaxiBus 31

/** Formulaire de r?servation
@code O33 @endcode
@param 0 Contenu champ nom
@param 1 Contenu champ pr?nom
@param 2 Champ pr?nom obligatoire
@param 3 Contenu champ Adresse
@param 4 Champ Adresse obligatoire
@param 5 Contenu champ e-mail
@param 6 Champ e-mail obligatoire
@param 7 Contenu champ t?l?phone
@param 8 Champ t?l?phone obligatoire
@param 9 Contenu champ num?ro abonn?
@param 10 Champ num?ro abonn? obligatoire
@param 11 Contenu champ adresse de d?part
@param 12 Champ adresse de d?part obligatoire
@param 13 Contenu champ adresse d'arriv?e
@param 14 Champ adresse d'arriv?e obligatoire
@param 15 Contenu champ nombre de places
@param 16 Contenu champ r?servation warning
@return Formulaire de r?servation
@todo Remettre aux normes
*/
#define ELEMENTINTERFACEChampsReservation       33


/** S?rie des feuilles de routes des trajets de la liste
@code O35 @endcode
@param Objet cTrajets * : Liste de trajets
@param 0 Etat du \ref IHMFiltreHandicape
@param 1 Etat du \ref IHMFiltreVelo
@return S?rie des feuilles de routes des trajets de la liste
*/
#define ELEMENTINTERFACEFeuillesRoute        35


/** Feuille de route d'un trajet
@code O36 @endcode
@param Objet cTrajet * : Trajets ? d?crire
@param 0 Etat du \ref IHMFiltreHandicape
@param 1 Etat du \ref IHMFiltreVelo
*/
#define ELEMENTINTERFACEFeuilleRoute        36


/** Dur?e du trajet
@code 038 @endcode
@param Objet cTrajet *
@return Dur?e du trajet
*/
#define EI_BIBLIOTHEQUE_Trajet_Duree        38


/** Ligne de tableau contenant les dur?es de chaque trajet
@code O39 @endcode
@param Objet cTrajets * Liste de trajets
@return Ligne de tableau contenant les dur?es de chaque trajet
*/
#define EI_BIBLIOTHEQUE_FicheHoraire_LigneDurees     39


/** Fabrication d'objets HTML permettant le lancement de fonctionnalit?s de SYNTHEES (formulaire ou lien)
@code O40 @endcode
@param 0 Type de sortie, valeurs :
- @c url : fabrique l'adresse complete d'un appel
- @c form @a texte : fabrique les champs d'un formulaire HTML, et place @a texte dans le tag form
@param 1 Fonction lanc?e, valeurs :
- @c timetable : Fiche horaire
- @c timetable @c validation : Validation avant fiche horaire
- @c from @c city @c list : Liste de communes de d?part
- @c to @c city @c list : Liste de communes d'arriv?e
- @c from @c station @c list : Liste d'arr?ts de d?part
- @c to @c station @c list : Liste d'arr?ts d'arriv?e

@code 040 {url/form} {timetable validation} @endcode
@param 2 Num?ro arret de d?part
@param 3 Num?ro arret d'arriv?e
@param 4 Num?ro d?signation de d?part
@param 5 Num?ro d?signation d'arriv?e
@param 6 Num?ro de comune de d?part
@param 7 Num?ro de comune d'arriv?e

@code 040 {url/form} {timetable} @endcode
@param 2 Num?ro arret de d?part
@param 3 Num?ro arret d'arriv?e
@param 4 Date du d?part
@param 5 Num?ro d?signation de d?part
@param 6 Num?ro d?signation d'arriv?e
@param 7 Code p?riode de la journ?e
@param 8 Filtre prise en charge des v?los
@param 9 Filtre prise en charge handicap?s
@param 10 Filtre lignes r?sa possible
@param 11 Filtre tarif :
- -1 = tout tarif

@code 040 {url/form} {from/to city/station list} @endcode
@param - Pas de param?tre suppl?mentaire

@return Formulaire ou lien permettant le lancement de fonctionnalit?s de SYNTHESE
*/
#define ELEMENTINTERFACEURLFormulaire        40


/** Fabrication d'?l?ments HTML permettant l'entr?e de param?tres pour des formulaires de lancement de fonctionnalit?s de SYNTHESE
@code O41 @endcode
@param 0 Champ, valeurs :
- @c date
- @c from @c city
- @c from @c station
- @c to @c city
- @c to @c station
- @c period : P?riode de la journ?e
- @c handicap @c filter
- @c tariff
@param 1 Valeur par d?faut
@param 2 Type de la balise input, valeurs :
- (rien) : type automatique
- @c text
- @c select
- @c checkbox : case ? cocher
- @c checkbox @a texte : case ? cocher de valeur @a tetxe si coch?e
NB : en cas de choix auto, l'?l?ment est fabriqu? int?gralement (ex: liste des dates) et est ferm?. en cas de choix d?termin?, alors aucun remplissage n'est effectu? et le select doit ?tre ferm?
@param 3 Code HTML ? ins?rer ? l'interieur de la balise

@code O41 {date} @endcode
@param 1 Valeur par d?faut (format AAAAMMJJ, ou commandes  de date (\ref cEnvironnement::dateInterpretee )
@param 2 Laisser vide pour s?lection
@param 3 Code HTML ? ins?rer ? l'int?rieur de la balise
@param 4 Premi?re date de la liste
@param 5 Derni?re date de la liste

@return El?ment HTML permettant l'entr?e de param?tres pour des formulaires de lancement de fonctionnalit?s de SYNTHESE
*/
#define ELEMENTINTERFACEInputHTML         41


/** Pr?fixe des messages d'alerte selon le niveau fourni en param?tre
@code O42 @endcode
@param 0 Niveau de l'alerte
@return Pr?fixe des messages d'alerte selon le niveau fourni en param?tre
*/
#define EI_BIBLIOTHEQUE_Interface_PrefixeAlerte      42


/** Destination d'une ligne
@code O43 @endcode
@param Objet cLigne * : Ligne
@return
- La destination affich?e sur les v?hicules (girouette) si renseign?e
- Le nom du terminus sinon
*/
#define EI_BIBLIOTHEQUE_Ligne_Destination       43


/** Ligne d'un d?part
@code O O44 @endcode
@param Objet cDescriptionPassage * : D?part
@param 0 HTML en d?but de ligne (conseill? tr)
@param 1 HTML en fin de ligne (conseill? /tr)
@param 2 Largeur en pixels de la case de lignes
@param 3 Hauteur en pixels de la case de lignes
@return Ligne d'un d?part
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne     44


/** Affichage d'un nombre format?.
@code O 045 @endcode
@param 0 Nombre ? afficher
@param 1 Nombre de chiffres ? afficher syst?matiquement
@param 2 Nombre ? ajouter au nombre ? afficher
*/
#define EI_BIBLIOTHEQUE_Nombre_Formate        45


/** Affichage de destination d'un service au d?part
@code 0 046 @endcode
@param Objet cDescriptionPassage* : D?part ? afficher
@param 0 Destination(s) ? afficher. Valeurs :
- @c terminus Aucune destination interm?diaire
- @c all Toutes destinations
- @a num Rang de la destination ? afficher
@param 1 Affichage du terminus. Valeurs :
- @c 0 Pas d'affichage du terminus
- @c 1 Affichage du terminus
@param 2 Type d'affichage de la destination. Valeurs :
- @c char(13) D?signation sur 13 caract?res
- @c char(26) D?signation sur 26 caract?res
- @c station Nom de l'arr?t
- @c station_city Nom de l'arr?t pr?c?d? du nom de commune
- @c station_city_if_new Nom de l'arr?t pr?c?d? du nom de commune uniquement si non d?j? affich?e par le parcours
@param 3 S?parateur entre arr?ts
@param 4 Texte ? placer avant nom de commune
@param 5 Texte ? placer entre nom de commune et nom d'arr?t

Table de v?rit? des affichages de destinations :
<table class="tableau">
<tr><th>Param.0 \ Param 1</th><th>0</th><th>1</th></tr>
<tr><th>terminus</th><td colspan="2">Terminus</td></tr>
<tr><th>all</th><td>Interm?diaires</td><td>Interm?diaires + Terminus</td></tr>
<tr><th>@a num entre 1 et le nombre de gares - 1 exclu</th><td colspan="2">Interm?diaire d?sign?e</td></tr>
<tr><th>@a num = le nombre de gares - 1</th><td>Aucun affichage</td><td>Terminus</td></tr>
<tr><th>@a num sup?rieur au nombre de gares - 1 </th><td colspan="2">Aucun affichage</td></tr>
</table>
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Destination    46

#define EI_BIBLIOTHEQUE_DescriptionPassage_ArretPhysique      47


/** Affichage de l'heure de d?part.
@code O 048 @endcode
@param Objet cDescriptionPassage* D?part
@param 0 Texte pr?c?dent le chiffre des heures si inf?rieur ? 10 (conseill? : espace, vide, ou 0)
@param 1 Texte pr?c?dent l'heure si le d?part s'effectue ? moins d'une minute
@param 2 Texte suivant l'heure si le d?part s'effectue ? moins d'une minute
*/
#define EI_BIBLIOTHEQUE_DescriptionPassage_Heure     48

#define EI_BIBLIOTHEQUE_Cases_Particularites      49



/** Num?ro de panneau affichant un tableau de d?part
@code O50 @endcode
@param Objet cTableauAffichage * : Tableau de d?part
@param 0 Format du num?ro. Valeurs :
- @c char(2) Num?ro sur 2 caract?res
*/
#define EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau       50

//@}



/*! @} */












//! @name 9 : EI_BIBLIOTHEQUE_PERIODE
//@{
#define EI_BIBLIOTHEQUE_PERIODE_INDEX       0 
//@}


//! @name 12 : ELEMENTINTERFACELienPhotosArretPhysiques
//@{
#define ELEMENTINTERFACELienPhotosArretPhysiquesDebut 0
#define ELEMENTINTERFACELienPhotosArretPhysiquesLibelleDefaut 1
#define ELEMENTINTERFACELienPhotosArretPhysiquesFin 2 
//@}


//! @name 13 : ELEMENTINTERFACELienAutresPhotos
//@{
#define ELEMENTINTERFACELienAutresPhotosDebut 0
#define ELEMENTINTERFACELienAutresPhotosFin 1
#define ELEMENTINTERFACELienAutresPhotosOuverture 2
#define ELEMENTINTERFACELienAutresPhotosFermeture 3 
//@}


//! @name 14 : ELEMENTINTERFACELienServices
//@{
#define ELEMENTINTERFACELienServicesDebut 0
#define ELEMENTINTERFACELienServicesFin 1
#define ELEMENTINTERFACELienServicesOuverture 2
#define ELEMENTINTERFACELienServicesFermeture 3 
//@}


//! @name 22 : EI_BIBLIOTHEQUE_Date
//@{
#define EI_BIBLIOTHEQUE_Date_Date       0 
//@}


//! @name 23 : ELEMENTINTERFACEListeCommunes
//@{
#define ELEMENTINTERFACEListeCommunesMessErreur  0
#define ELEMENTINTERFACEListeCommunesOuverture  1
#define ELEMENTINTERFACEListeCommunesFermeture  2
#define ELEMENTINTERFACEListeCommunesNombre   3
#define ELEMENTINTERFACEListeCommunesEntree   4
#define ELEMENTINTERFACEListeCommunesTexteSiVide 5 
//@}


//! @name 24 : ELEMENTINTERFACEListeArrets
//@{
#define ELEMENTINTERFACEListeArretsMessErreur 0
#define ELEMENTINTERFACEListeArretsOuverture 1
#define ELEMENTINTERFACEListeArretsFermeture 2
#define ELEMENTINTERFACEListeArretsNombre  3
#define ELEMENTINTERFACEListeArretsCommune  4
#define ELEMENTINTERFACEListeArretsEntree  5
#define ELEMENTINTERFACEListeArretsTexteSiVide 6 
//@}


//! @name 25 : ELEMENTINTERFACENomCommune
//@{
#define ELEMENTINTERFACENomCommuneNumero 0 
//@}


//! @name 29 : ELEMENTINTERFACESelecteurVelo
//@{
#define ELEMENTINTERFACESelecteurVeloDefaut 0 
//@}


//! @name 30 : ELEMENTINTERFACESelecteurHandicape
//@{
#define ELEMENTINTERFACESelecteurHandicapeDefaut 0 
//@}


//! @name 31 : ELEMENTINTERFACESelecteuTaxiBus
//@{
#define ELEMENTINTERFACESelecteurTaxiBusDefaut 0 
//@}


//! @name 32 : ELEMENTINTERFACESelecteurTarif
//@{
#define ELEMENTINTERFACESelecteurTarifDefaut 0 
//@}


//! @name 33 : ELEMENTINTERFACEChampsReservation
//@{
#define ELEMENTINTERFACEChampsReservationNom       0
#define ELEMENTINTERFACEChampsReservationPrenom      1
#define ELEMENTINTERFACEChampsReservationPrenomObligatoire   2
#define ELEMENTINTERFACEChampsReservationAdresse     3
#define ELEMENTINTERFACEChampsReservationAdresseObligatoire   4
#define ELEMENTINTERFACEChampsReservationEmail      5
#define ELEMENTINTERFACEChampsReservationEmailObligatoire   6
#define ELEMENTINTERFACEChampsReservationTelephone     7
#define ELEMENTINTERFACEChampsReservationTelephoneObligatoire  8
#define ELEMENTINTERFACEChampsReservationNumeroAbonne    9
#define ELEMENTINTERFACEChampsReservationNumeroAbonneObligatoire 10
#define ELEMENTINTERFACEChampsReservationAdresseDepart    11
#define ELEMENTINTERFACEChampsReservationAdresseDepartObligatoire  12
#define ELEMENTINTERFACEChampsReservationAdresseArrivee    13
#define ELEMENTINTERFACEChampsReservationAdresseArriveeObligatoire  14
#define ELEMENTINTERFACEChampsReservationNbPlaces     15
#define ELEMENTINTERFACEChampsReservationWarning       16 
//@}


//! @name 35 : ELEMENTINTERFACEFeuillesRoute
//@{
#define ELEMENTINTERFACEFeuillesRouteFiltreHandicape    0
#define ELEMENTINTERFACEFeuillesRouteFiltreVelo      1 
//@}


//! @name 36 : ELEMENTINTERFACEFeuilleRoute
//@{
#define ELEMENTINTERFACEFeuilleRouteFiltreHandicape     0
#define ELEMENTINTERFACEFeuilleRouteFiltreVelo      1 
//@}


//! \name 40 : ELEMENTINTERFACEURLFormulaire
//@{
#define ELEMENTINTERFACEURLFormulaireTypeSortie      0
#define ELEMENTINTERFACEURLFormulaireFonction      1
#define ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart   2
#define ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee   3
#define ELEMENTINTERFACEURLFormulaireFHDate       4
#define ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart  5
#define ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee  6
#define ELEMENTINTERFACEURLFormulaireFHPeriode      7
#define ELEMENTINTERFACEURLFormulaireFHVelo       8
#define ELEMENTINTERFACEURLFormulaireFHHandicape     9
#define ELEMENTINTERFACEURLFormulaireFHResa       10
#define ELEMENTINTERFACEURLFormulaireFHTarif      11
#define ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart   2
#define ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee   3
#define ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart  4
#define ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee 5
#define ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart   6
#define ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee  7 
//@}

//! \name 41 : ELEMENTINTERFACEInputHTML
//@{
#define ELEMENTINTERFACEInputHTMLChamp        0
#define ELEMENTINTERFACEInputHTMLValeurDefaut      1
#define ELEMENTINTERFACEInputHTMLType        2
#define ELEMENTINTERFACEInputHTMLSuite        3
#define ELEMENTINTERFACEInputHTMLListeDatesMin      4
#define ELEMENTINTERFACEInputHTMLListeDatesMax      5 
//@}

//! \name 42 : EI_BIBLIOTHEQUE_Interface_PrefixeAlerte
//@{
#define EI_BIBLIOTHEQUE_Interface_PrefixeAlerte_Niveau    0 
//@}

//! \name 44 : EI_BIBLIOTHEQUE_DescriptionPassage_Ligne
//@{
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLDebutLigne  0
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLFinLigne  1
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_LargeurCase  2
#define EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HauteurCase  3 
//@}


//! \name 50 : EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau
//@{
#define EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau_Format     0 
//@}



#endif

