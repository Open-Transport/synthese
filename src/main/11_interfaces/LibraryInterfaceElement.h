
/** LibraryInterfaceElement class header.
	@file LibraryInterfaceElement.h

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

#ifndef SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_LIBRARY_INTERFACE_ELEMENT_H

#include<boost/shared_ptr.hpp>

#include "01_util/Factorable.h"
#include "01_util/Registrable.h"

#include "11_interfaces/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		class InterfacePage;
		class ValueElementList;

		/** @defgroup refLibrary Library Interface Elements.
			@ingroup refInt
		*/

		/** element which can produce a single display.
			@ingroup m11
		*/
		class LibraryInterfaceElement
			: public util::Factorable
			, public util::Registrable<int, LibraryInterfaceElement>
		{
		protected:
			boost::shared_ptr<const InterfacePage>	_page;
			std::string _label;

			virtual void storeParameters(ValueElementList& vel) = 0;

		public:

			virtual std::string getLabel() const { return std::string(); }
			
			void parse( const std::string& text );

			/** Library page element creation.
				@param text Text to parse
				@param page Page to link
				@return Pointer to the created element. If the line is empty, then a null pointer is returned.
				@exception InterfacePageException if the specified key was not found in the library
				@author Hugues Romain
				@date 2007
			*/
			static boost::shared_ptr<LibraryInterfaceElement> create( const std::string & text, boost::shared_ptr<const InterfacePage> page );

			template<class T>
			boost::shared_ptr<LibraryInterfaceElement> copy( const ParametersVector& parameters )
			{
				return boost::shared_ptr<T>(new T(*((T*) this)));
			}


			/** Virtual display method.
				This method must be implemented in each subclass. It defines the behavior of the interface library element in three ways :
					- it can write outputs on the stream : parameter stream
					- it can write (and read) local variables : parameter variables
					- it can return a label to go after the display
				
				@param stream Stream to write the output on
				@param parameters Execution parameters
				@param variables Execution variables
				@param object Object pointer to read
				@param request The source request (read only)
				@return Label to go after the display
			*/
			virtual std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const = 0;
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

/*
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



#define EI_BIBLIOTHEQUE_Cases_Particularites      49


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

