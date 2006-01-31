
#include "cInterface_Objet_Element_Bibliotheque.h"
#include <sstream>

#include "cTableauAffichage.h"



/*!	\brief Copie d'un �l�ment objet dynamique, selon les param�tres fournis
	\param Parametres Chaine de param�tres desquels tenir compte
	\author Hugues Romain
	\date 2000-2005
	
La copie d'un �l�ment de param�tre dynamique consiste � cr�er une copie du ni�me param�tre fourni, avec n �tant le num�ro du param�tre de l'objet (transalation), quelle que soit sa nature.
*/
cInterface_Objet_Element* cInterface_Objet_Element_Bibliotheque::Copie(const cInterface_Objet_AEvaluer_ListeParametres& __Parametres) const
{
	cInterface_Objet_Element_Bibliotheque* __EI = new cInterface_Objet_Element_Bibliotheque(_Index);
	for (const cInterface_Objet_Element_Parametre* __Parametre = _Parametres[0]; __Parametre != NULL; __Parametre = __Parametre->getSuivant())
		__EI->_Parametres.Chaine(__Parametre->Copie(__Parametres));
	return __EI;
}




/*!	\brief Constructeur
*/
cInterface_Objet_Element_Bibliotheque::cInterface_Objet_Element_Bibliotheque(tIndex __Index)
{
	_Index = __Index;
}



/*!	\brief Constructeur
*/
cInterface_Objet_Element_Bibliotheque::cInterface_Objet_Element_Bibliotheque(tIndex __Index, const cTexte& __Texte)
{
	_Index = __Index;
	_Parametres.InterpreteTexte(cInterface_Objet_AEvaluer_ListeParametres(), __Texte);
}



/*!	\brief Evaluation d'un objet dynamique
	\param pCtxt Le flux de sortie sur lequel �crire le r�sultat de l'�valuation
	\param __Parametres Les param�tres � fournir � l'�l�ment d'interface pour l'�valuation
	\param __Objet Pointeur vers un objet � fournir � l'�l�ment d'interface pour l'�valuation
	\param __Site Pointeur vers le site ayant g�n�r� l'�valuation
	\return R�f�rence vers le flux de sortie
	\author Hugues Romain
	\date 2001-2005
	
*/
tIndex cInterface_Objet_Element_Bibliotheque::Evalue(ostream& pCtxt, const cInterface_Objet_Connu_ListeParametres& __Parametres
										, const void* __Objet, const cSite* __Site) const
{
// PROVISOIRE
#define __RepBI "/bi/" << _Interface->Index() << "/"

	switch (_Index)
	{
	case ELEMENTINTERFACELieuArret:
		{
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			
			// Initialisation des param�tres
			tIndex iNumeroDesignation = _Parametres[ELEMENTINTERFACELieuArretNumeroDesignation]->Nombre(__Parametres);
			cArretLogique* curPA = __Environnement->getArretLogique(_Parametres[ELEMENTINTERFACELieuArretNumeroArretLogique]->Nombre(__Parametres));
			
			// Affichage
			// PROVISOIRE LANGAGE IMPOSE
			cTexteHTML HTML;
			if (iNumeroDesignation)
				HTML << *curPA->GetAccesPADe(iNumeroDesignation) << "<br />Arr&ecirc;t: ";
			HTML << *curPA; 
			pCtxt << HTML;
		}
		break;

	case ELEMENTINTERFACEFicheHoraire:
		if (__Objet == NULL)	// Cas Pas de solution
			__Site->Affiche(pCtxt, INTERFACEAucuneSolutionFicheHoraire, __Parametres);
		else
		{ 
			// Evaluation des param�tres
			const cTrajets* __Trajets = (const cTrajets*) __Objet;
			
			const cElementTrajet* curET;
			int __Ligne;
			
			cMoment tempMoment;

			// Initialisation des lignes de texte
 			stringstream* __Tampons = new stringstream [__Trajets->TailleListeOrdonneePointsArret()];
			
			// Parcours de chaque trajet
			for (int __IndexTrajet=0; __IndexTrajet<__Trajets->Taille(); __IndexTrajet++)
			{
				const cTrajet* __Trajet = &__Trajets->operator[](__IndexTrajet);
				
				// Parcours de chaque �l�ment de trajet
				__Ligne=0;
				for (curET = __Trajet->PremierElement(); curET != NULL; curET = curET->Suivant())
				{
					// Pr�paration des cases d�part et arriv�e
					cInterface_Objet_Connu_ListeParametres __ParametresCaseDepart;
					cInterface_Objet_Connu_ListeParametres __ParametresCaseArrivee;
					
					//0 Premiere ligne ?
					__ParametresCaseDepart << (__Ligne == 0 ? "1" : "");
					__ParametresCaseArrivee << "1";
					
					//1 Derniere ligne ?
					__ParametresCaseDepart << "1";
					__ParametresCaseArrivee << (curET->Suivant() == NULL ? "1" : "");
					
					//2 Num�ro de colonne
					__ParametresCaseDepart << __IndexTrajet + 1;
					__ParametresCaseArrivee << __IndexTrajet + 1;

					//3 Ligne � pied ?
					__ParametresCaseDepart << (curET->getLigne()->EstUneLigneAPied() ? "1" : "");
					__ParametresCaseArrivee << (curET->getLigne()->EstUneLigneAPied() ? "1" : "");
					
					//4 Premier d�part
					cTexte __Heure;
					__Heure.Vide();
					__Heure << curET->MomentDepart().getHeure();
					__ParametresCaseDepart << __Heure;	//4 Premier d�part
					__Heure.Vide();
					__Heure << curET->MomentArrivee().getHeure();	//4 Premier d�part
					__ParametresCaseArrivee << __Heure;
					
					//5,6
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
					{
						tempMoment = curET->MomentDepart();
						tempMoment += __Trajet->getAmplitudeServiceContinu();
						__ParametresCaseDepart << tempMoment.getHeure();	//5 Dernier d�part
						tempMoment = curET->MomentArrivee();
						tempMoment += __Trajet->getAmplitudeServiceContinu();
						__ParametresCaseArrivee << tempMoment.getHeure();	//5 Dernier d�part
						
						//6 Ligne service continu ?
						__ParametresCaseDepart << "1";
						__ParametresCaseArrivee << "1";
					}
					else
					{
						//5 Dernier d�part
						__ParametresCaseDepart << "";	//5
						__ParametresCaseArrivee << "";
						
						//6 Ligne service continu ?
						__ParametresCaseDepart << "";	//6
						__ParametresCaseArrivee << "";
					}

					// Pr�paration param�tres case vide
					cInterface_Objet_Connu_ListeParametres __ParametresCaseVide;
					__ParametresCaseVide << "1";
					__ParametresCaseVide << "1";
					__ParametresCaseVide << __IndexTrajet;
					__ParametresCaseVide << "";
					__ParametresCaseVide << "";
					__ParametresCaseVide << "";
					__ParametresCaseVide << "";
					
					// Affichage
					__Site->Affiche(__Tampons[__Ligne], INTERFACEFicheHoraireColonne, __ParametresCaseDepart);
					for (__Ligne++; __Trajets->getListeOrdonneePointsArret(__Ligne) != curET->getGareArrivee(); __Ligne++)
 						__Site->Affiche(__Tampons[__Ligne], INTERFACEFicheHoraireColonne, __ParametresCaseVide);
					__Site->Affiche(__Tampons[__Ligne], INTERFACEFicheHoraireColonne, __ParametresCaseArrivee);
				}
			}
			
			// Pr�paration des lignes
			bool __Couleur = false;
			for (__Ligne = 0; __Ligne < __Trajets->TailleListeOrdonneePointsArret(); __Ligne++)
			{
				cInterface_Objet_Connu_ListeParametres __ParametresLigne;
				__ParametresLigne << __Tampons[__Ligne].str();
				__ParametresLigne << (__Couleur ? "1" : "");
				__Couleur = !__Couleur;
				__Site->Affiche(pCtxt, INTERFACEFicheHoraireLigne, __ParametresLigne, __Trajets->getListeOrdonneePointsArret(__Ligne));
			}

			delete[] __Tampons;
			
			/*		
					// GESTION DES ALERTES
					// Gestion des alertes : 3 cas possibles :
					// Alerte sur arr�t de d�part
					// Circulation � r�servation
					// Alerte sur circulation
					// Alerte sur arr�t d'arriv�e
					cMoment __debutAlerte, __finAlerte;
					
					// Alerte sur arr�t de d�part
					// D�but alerte = premier d�part
					// Fin alerte = dernier d�part
					cMoment debutPrem = curET->MomentDepart();
					cMoment finPrem = debutPrem;
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						finPrem += __Trajet->getAmplitudeServiceContinu();
					if (curET->getGareDepart()->getAlerte()->showMessage(__debutAlerte, __finAlerte)
					&&	__NiveauRenvoiColonne < curET->getGareDepart()->getAlerte()->Niveau())
						__NiveauRenvoiColonne = curET->getGareDepart()->getAlerte()->Niveau();
					
					// Circulation � r�servation obligatoire
					cMoment maintenant;
					maintenant.setMoment();
					if (curET->getLigne()->GetResa()->TypeResa() == Obligatoire
					&&	curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->MomentDepart())
					&&	__NiveauRenvoiColonne < ALERTE_ATTENTION)
						__NiveauRenvoiColonne = ALERTE_ATTENTION;
					
					// Circulation � r�servation possible
					maintenant.setMoment();
					if (curET->getLigne()->GetResa()->TypeResa() == Facultative
					&&	curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->MomentDepart())
					&&	__NiveauRenvoiColonne < ALERTE_INFO)
						__NiveauRenvoiColonne = ALERTE_INFO;
					
					// Alerte sur circulation
					// D�but alerte = premier d�part
					// Fin alerte = derni�re arriv�e
					debutPrem = curET->MomentDepart();
					finPrem = curET->MomentArrivee();
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						finPrem += __Trajet->getAmplitudeServiceContinu();
					if (curET->getLigne()->getAlerte()->showMessage(__debutAlerte, __finAlerte)
					&&	__NiveauRenvoiColonne < curET->getLigne()->getAlerte()->Niveau())
						__NiveauRenvoiColonne = curET->getLigne()->getAlerte()->Niveau();
					
					// Alerte sur arr�t d'arriv�e
					// D�but alerte = premi�re arriv�e
					// Fin alerte = dernier d�part de l'arr�t si correspondnce, derni�re arriv�e sinon
					__debutAlerte = curET->MomentArrivee();
					__finAlerte = __debutAlerte;
					if (curET->Suivant() != NULL)
						__finAlerte = curET->Suivant()->MomentDepart();
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						__finAlerte += __Trajet->getAmplitudeServiceContinu();
					if (curET->getGareArrivee()->getAlerte()->showMessage(__debutAlerte, __finAlerte)
					&&	__NiveauRenvoiColonne < curET->getGareArrivee()->getAlerte()->Niveau())
						__NiveauRenvoiColonne = curET->getGareArrivee()->getAlerte()->Niveau();
				}
				
				// Affichage du renvoi si necessaire
//				if (__NiveauRenvoiColonne)
//				{
//					TamponRenvois << "<img src=\"" << __RepBI << "img/warning.gif\" alt=\"Cliquez sur la colonne pour plus d'informations\" />";
//					__MontrerLigneRenvois = true;
//				}

				for (l++;l!=m;l++)
				{
					Tampons[l] << "<td class=\"tdHoraires";
					if (Couleur1)
						Tampons[l] << "1";
					else
						Tampons[l] << "2";
					Tampons[l] << "\" onclick=\"show('divFiche";
					Tampons[l].Copie(n,3);
					Tampons[l] << "')\">";
					if (Couleur1)
						Couleur1=false;
					else
						Couleur1=true;
				}

//				pCtxt << "</tr></table></div>";
//				TamponRenvois << "</td>";
			}
			*/
			
// 			delete [] Tampons;
		}
		break;
		
		
	case EI_OBJETDYNAMIQUE_ListeLignesTrajets:
		{
			// Collecte des param�tres
			cTrajets* __Trajets = (cTrajets*) __Objet;
			
			// Fabrication de l'affichage
			for (tIndex __n=0; __n < __Trajets->Taille(); __n++)
			{
				// Pr�paration des param�tres de la case
				cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
				// 0 : Num�ro de la colonne
				__ParametresCaseLigne << __n + 1;	// 0 : Num�ro de la colonne
				
				// Lancement de l'affichage
				__Site->Affiche(pCtxt, INTERFACECaseLignes, __ParametresCaseLigne, __Trajets->GetElement(__n));
			}
		}
		break;

	// 7
	case EI_OBJETDYNAMIQUE_ListeLignesTrajet:
		{
			// Collecte des param�tres
			const cTrajet* __Trajet = (const cTrajet*) __Objet;
			bool __AfficherLignesPied = _Parametres[EI_OBJETDYNAMIQUE_ListeLignesTrajet_AffichePietons]->Nombre(__Parametres) > 0;
			
			cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
			__ParametresCaseLigne << _Parametres[EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLDebutLigne]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLFinLigne]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_OBJETDYNAMIQUE_ListeLignesTrajet_LargeurCase]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_OBJETDYNAMIQUE_ListeLignesTrajet_HauteurCase]->Texte(__Parametres);
			
			// Fabrication de l'affichage
			for (const cElementTrajet* __ET = __Trajet->PremierElement(); __ET != NULL; __ET = __ET->Suivant())
			{
				if (__AfficherLignesPied || !__ET->getLigne()->EstUneLigneAPied())
					__Site->Affiche(pCtxt, INTERFACECartoucheLigne, __ParametresCaseLigne, __ET->getLigne());
			}
		}
		break;
	
	case ELEMENTINTERFACEMiniTbDepGare:
		{	/*
			cDescriptionPassage* curDP = (cDescriptionPassage*) ObjetAAfficher;
			
			// SET PORTAGE LINUX
			//cAlternance curAlternance("tdHoraires2",2);
			cAlternance curAlternance(cTexte("tdHoraires2"),2);
			//END PORTAGE LINUX
			if (curDP==NULL)
				pCtxt << "<tr><td style=\"text-align:center\">Pas de d�part d'ici 24h";
			else
			{
				for (; curDP!=NULL; curDP=curDP->Suivant())
				{
					pCtxt << "<tr><td style=\"width:25px;\" class=\"" << curAlternance << "\">";
					curDP->getGareLigne()->Ligne()->Cartouche(23,16, pCtxt);
					pCtxt << "<td style=\"width:35px;\" class=\"" << curAlternance << "\"><b>";
					if (curDP->getSitPert() == NULL)
						curDP->getMomentPrevu().getHeure().toString(pCtxt);
					else
						curDP->getMomentReel().getHeure().toString(pCtxt);
					pCtxt << "</b><td class=\"" << curAlternance << "\">";
					cArretLogique* PAT = curDP->getGareLigne()->Destination()->ArretLogique();
					if (curDP->getSitPert() == NULL)
					{ 
							PAT->NomHTML(pCtxt);
					}
					else
					{
						pCtxt << "<marquee>";
						PAT->NomHTML(pCtxt);

						pCtxt << " - D�part initialement pr�vu � ";

						curDP->getMomentPrevu().getHeure().toString(pCtxt);
						pCtxt << " - " << curDP->getSitPert()->Motif << " - </marquee>";
					}
					pCtxt << "</tr>";
					curAlternance.Incrementer();
				}
			}
		*/}
		break;
	
	case EI_BIBLIOTHEQUE_PERIODE:
		pCtxt << __Site->getInterface()->GetPeriode(_Parametres[EI_BIBLIOTHEQUE_PERIODE_INDEX]->Nombre(__Parametres))->Libelle();
		break;

	case ELEMENTINTERFACELienPhotosArretPhysiques:

		{
			// Initialisation des param�tres
			const cArretLogique* __ArretLogique = (const cArretLogique*) __Objet;
			cTexte __TexteEnTete = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesDebut]->Texte(__Parametres);
			cTexte __TexteLibelle = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesLibelleDefaut]->Texte(__Parametres);
			cTexte __TextePied = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesFin]->Texte(__Parametres);
			
			for (tNumeroVoie __ArretPhysique = 1; __ArretPhysique <= __ArretLogique->NombreArretPhysiques(); __ArretPhysique++)
			{
				pCtxt << __TexteEnTete << "<a href=\"javascript:showdiv('Q" << __ArretPhysique << "');\">";
				if (__ArretLogique->getArretPhysique(__ArretPhysique)->getNom().Taille() == 0)
					pCtxt << __TexteLibelle << __ArretPhysique;
				else
					pCtxt << __ArretLogique->getArretPhysique(__ArretPhysique)->getNom();
				pCtxt << "</a>" << __TextePied;
			}
		}

		break;

	case ELEMENTINTERFACELienAutresPhotos:
		{
			const cArretLogique* ArretLogique = (const cArretLogique*) __Objet;
			cTexte __TexteEnTete = _Parametres[ELEMENTINTERFACELienAutresPhotosOuverture]->Texte(__Parametres);
			cTexte __TexteLienDebut = _Parametres[ELEMENTINTERFACELienAutresPhotosDebut]->Texte(__Parametres);
			cTexte __TexteLienFin = _Parametres[ELEMENTINTERFACELienAutresPhotosFin]->Texte(__Parametres);
			cTexte __TextePied = _Parametres[ELEMENTINTERFACELienAutresPhotosFermeture]->Texte(__Parametres);
			
			if (ArretLogique->NombrePhotos())
			{
				pCtxt << __TexteEnTete;
				for (tNumeroVoie iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
					pCtxt << __TexteLienDebut << "<a href=\"javascript:showdiv('P" << iArretPhysique << "');\">" << ArretLogique->getPhoto(iArretPhysique)->DescriptionLocale() << "</a>" << __TexteLienFin;
				pCtxt << __TextePied;
			}
		}
		break;

	case ELEMENTINTERFACELienServices:
		{
			const cArretLogique* ArretLogique = (const cArretLogique*) __Objet;
			cTexte __TexteEnTete = _Parametres[ELEMENTINTERFACELienServicesOuverture]->Texte(__Parametres);
			cTexte __TexteLienDebut = _Parametres[ELEMENTINTERFACELienServicesDebut]->Texte(__Parametres);
			cTexte __TexteLienFin = _Parametres[ELEMENTINTERFACELienServicesFin]->Texte(__Parametres);
			cTexte __TextePied = _Parametres[ELEMENTINTERFACELienServicesFermeture]->Texte(__Parametres);
						
			if (ArretLogique->GetService(0))
			{
				pCtxt << __TexteEnTete;
				for (tNumeroVoie iArretPhysique=0; ArretLogique->GetService(iArretPhysique); iArretPhysique++)
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
		}
		break;


	case ELEMENTINTERFACEDivArretPhysiques:
		{
			// Collecte des param�tres
			const cArretLogique* ArretLogique = (const cArretLogique*) __Objet;
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			
			// Affichage
			const cPhoto* curPhoto;
			tNumeroVoie iArretPhysique2;
			for (tNumeroVoie iArretPhysique=1; iArretPhysique <= ArretLogique->NombreArretPhysiques(); iArretPhysique++)
			{
				pCtxt << "<div id=\"Q" << iArretPhysique << "\" class=\"Q\">";
				curPhoto = ArretLogique->getArretPhysique(iArretPhysique)->getPhoto();
				if (curPhoto)
				{
					pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
					pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
					pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MQ" << TXT(iArretPhysique) << "\" />";
//! \todo METTRE A JOUR
// 					pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
					pCtxt << "<map name=\"MQ" << iArretPhysique << "\">";
					for (tIndex iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
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
		}
		break;

	case ELEMENTINTERFACEDivPhotos:
		{
			const cArretLogique* ArretLogique = (const cArretLogique*) __Objet;
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			
			const cPhoto* curPhoto;
			tNumeroVoie iArretPhysique2;
			for (tNumeroVoie iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
			{
				pCtxt << "<div id=\"P" << TXT(iArretPhysique) << "\" class=\"P\">";
				curPhoto = ArretLogique->getPhoto(iArretPhysique);
				if (curPhoto)
				{
					pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
					pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
					pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MP" << TXT(iArretPhysique) << "\" />";
//! \todo METTRE A JOUR
//					pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
					pCtxt << "<map name=\"MP" << TXT(iArretPhysique) << "\">";
					for (tIndex iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
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
		}
		break;
	
	
	case ELEMENTINTERFACEDivServices:
	/*	{
			cArretLogique* ArretLogique = (cArretLogique*) ObjetAAfficher;
			cPhoto* curPhoto;
			tNumeroVoie iArretPhysique2;
			if (ArretLogique->Service != NULL)
			{									
				for (tNumeroVoie iArretPhysique=0; ArretLogique->Service[iArretPhysique]!=NULL; iArretPhysique++)
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
		break;

	case ELEMENTINTERFACEFicheArretScript:
		{	// A VIRER
			const cArretLogique* ArretLogique = (const cArretLogique*) __Objet;
			pCtxt << "<script> function showdiv(s) {";
			//pCtxt << "if (s=='PS') document.all.PS.style.visibility = 'visible'; else document.all.PS.style.visibility = 'hidden';";
			pCtxt << "for (var k=1; k<=" << ArretLogique->NombreArretPhysiques() << "; k++) if (s=='Q'+k) eval(\"document.all.Q\" + k.toString() + \".style.visibility = 'visible';\"); else eval(\"document.all.Q\" + k.toString() + \".style.visibility = 'hidden';\");";
			if (ArretLogique->NombrePhotos())
				pCtxt << "for (k=0; k<" << ArretLogique->NombrePhotos() << "; k++) if (s=='P'+k) eval(\"document.all.P\" + k.toString() + \".style.visibility = 'visible';\"); else eval(\"document.all.P\" + k.toString() + \".style.visibility = 'hidden';\");";
			for (tNumeroVoie iService=0; ArretLogique->GetService(iService); iService++)
				if (ArretLogique->GetService(iService)->getPhoto())
					pCtxt << "if (s=='S" << iService << "') eval(\"document.all.S" << iService << ".style.visibility = 'visible';\"); else eval(\"document.all.S" << iService << ".style.visibility = 'hidden';\");";
			pCtxt << "} </script>";
		}
		break;

	case EI_BIBLIOTHEQUE_Date:
		{
			cDate curDate;
			curDate = _Parametres[EI_BIBLIOTHEQUE_Date_Date]->Texte(__Parametres);
            cDate& refDate = curDate;
		    __Site->getInterface()->AfficheDate(pCtxt, refDate);
		}
		break;

	case ELEMENTINTERFACEListeCommunes:
		{
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			tIndex n = _Parametres[ELEMENTINTERFACEListeCommunesNombre]->Nombre(__Parametres);
			cCommune** tbCommunes = __Environnement->TextToCommune(_Parametres[ELEMENTINTERFACEListeCommunesEntree]->Texte(__Parametres),n);
			
			
			if (tbCommunes[1]==NULL)
				pCtxt << _Parametres[ELEMENTINTERFACEListeCommunesMessErreur]->Texte(__Parametres);
			else
			{
				pCtxt << "<script>Nom = new Array; Num = new Array;</script>";
				for (tIndex i=1; i <= n; i++)
					if (tbCommunes[i]!=NULL)
					{
						pCtxt << "<script>Nom[" << i << "]=\"" << tbCommunes[i]->GetNom() << "\";Num[" << i << "]=" << tbCommunes[i]->Index() << ";</script>";
						pCtxt << _Parametres[ELEMENTINTERFACEListeCommunesOuverture]->Texte(__Parametres);
						pCtxt << "<a href=\"javascript:MAJ(" << i << ")\">" << *tbCommunes[i] << "</a>";
						pCtxt << _Parametres[ELEMENTINTERFACEListeCommunesFermeture]->Texte(__Parametres);
					}
					else
						pCtxt << _Parametres[ELEMENTINTERFACEListeCommunesTexteSiVide]->Texte(__Parametres);
			}
		}
		break;

	case ELEMENTINTERFACEListeArrets:
		{
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			tIndex n = _Parametres[ELEMENTINTERFACEListeArretsNombre]->Nombre(__Parametres);
			cAccesPADe** tbAccesPADe = __Environnement->getCommune(_Parametres[ELEMENTINTERFACEListeArretsCommune]->Nombre(__Parametres))
										->textToPADe(_Parametres[ELEMENTINTERFACEListeArretsEntree]->Texte(__Parametres),n);
			if (tbAccesPADe[1]==NULL)
				pCtxt << _Parametres[ELEMENTINTERFACEListeArretsMessErreur]->Texte(__Parametres);
			else
			{
				pCtxt << "<script>Nom = new Array; NumPA = new Array; NumDes = new Array;</script>";
				for (tIndex i = 1; i <= n; i++)
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
		}
		break;

	// 25 A mettre dans donn�e environnement
	case ELEMENTINTERFACENomCommune:
		{
			const cEnvironnement*	__Environnement = __Site->getEnvironnement();
			cCommune* curCommune = __Environnement->getCommune(_Parametres[ELEMENTINTERFACENomCommuneNumero]->Nombre(__Parametres));
			pCtxt << curCommune->GetNom();
		}
		break;
	
	// 26	
	case ELEMENTINTERFACETbDepGare:
		{	
			// Lecture des param�tres
			cDescriptionPassage* __DP = (cDescriptionPassage*) __Objet;
			tIndex			__MultiplicateurRangee = _Parametres[0]->Nombre(__Parametres) ? _Parametres[0]->Nombre(__Parametres) : 1;
			const cTexte&	__Pages = _Parametres[1]->Texte(__Parametres);
			const cTexte&	__SeparateurPage = _Parametres[2]->Texte(__Parametres);
						
			// Gestion des pages
			tIndex __NombrePages = 1;
			if (__Pages.Compare("intermediate") || __Pages.Compare("destination"))
			{
				for (cDescriptionPassage* ___DP = __DP; ___DP != NULL; ___DP = ___DP->Suivant())
					if (___DP->NombreGares() - 2 > __NombrePages)
						__NombrePages = ___DP->NombreGares() - 2;
			}
			if (__Pages.Compare("destination"))
				__NombrePages++;

			// Boucle sur les pages
			for (tIndex __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++)
			{
				// S�parateur de page
				if (__NumeroPage > 1)
					pCtxt << __SeparateurPage;

				// Boucle sur les rang�es
				tIndex __Rangee = __MultiplicateurRangee;
				for (cDescriptionPassage* ___DP = __DP; ___DP != NULL; ___DP = ___DP->Suivant())
				{
					// Pr�paration des param�tres
					cInterface_Objet_Connu_ListeParametres  __ParametresColonne;
					__ParametresColonne << __Rangee;		// 0 : Num�ro de rang�e
					
					int __NombrePagesRangee = ___DP->NombreGares() - 2 + (__Pages.Compare("destination") ? 1 : 0);
					if (!__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * (__NombrePages / __NombrePagesRangee))
						__ParametresColonne << __NumeroPage;
					else
						__ParametresColonne << 1 + __NumeroPage % __NombrePagesRangee;					// 1 : Num�ro de page
					
					// Lancement de l'affichage de la rang�e
					__Site->Affiche(pCtxt, INTERFACELigneTableauDepart, __ParametresColonne, (const void*) ___DP);

					// Incr�mentation du num�ro de rang�e
					__Rangee += __MultiplicateurRangee;
				}
			}
		}
		break;

	// 27
	case ELEMENTINTERFACETbDep:
		{	/*
			cDescriptionPassage* curDP = (cDescriptionPassage*) ObjetAAfficher;
			
			// Curseurs
			cGareLigne* curGLA;
			
			cDescriptionPassage* curDP2;
			tHeure DerniereHeureAffichee = 25;
			short int NombreReponses = 0;
			
			cAlternance curAlternance(cTexte("tdHoraires2"),2);
			
			while (curDP!=NULL)
			{
				NombreReponses++;
				if (curDP->getMomentPrevu().Heures() != DerniereHeureAffichee)
				{
					if (DerniereHeureAffichee != 25)
						pCtxt << "</table>";
					DerniereHeureAffichee = curDP->getMomentPrevu().Heures();
					pCtxt << "<h2>" << TXT(DerniereHeureAffichee) << "h</h2><table style=\"border-collapse:collapse\">";
				}
				pCtxt << "<tr class=\"";
				curAlternance.Afficher(pCtxt);
				pCtxt << "\"><td class=\"";
				curAlternance.Afficher(pCtxt);
				pCtxt << "\">";
				curDP->getGareLigne()->Ligne()->Cartouche(23, 16, pCtxt);
				
				pCtxt << "<td class=\"";
				curAlternance.Afficher(pCtxt);
				pCtxt << "\"><b>";
				curDP->getMomentPrevu().getHeure().toString(pCtxt);
				pCtxt << "</b><td class=\"";
				curAlternance.Afficher(pCtxt);
				pCtxt << "\">";
				for (curGLA = curDP->getGareLigne()->getArriveeSuivante(); curGLA != NULL; curGLA = curGLA->getArriveeSuivante())
					if (curGLA->HorairesSaisis())
					{					
						curGLA->ArretLogique()->NomHTML(pCtxt);
						pCtxt << " (";
						curGLA->CalculeArrivee(curDP).getHeure().toString(pCtxt);
						pCtxt << ") ";
					}
				pCtxt << "\n";
	
				curAlternance.Incrementer();
	
				curDP2 = curDP;
				curDP = curDP->Suivant();
				delete curDP2;
			}
		*/}
		break;				
	
	// 29 A mettre dans 41
	case ELEMENTINTERFACESelecteurVelo:
		{
			tBool3 VeloDefaut = (tBool3) _Parametres[ELEMENTINTERFACESelecteurVeloDefaut]->Nombre(__Parametres);

			if (VeloDefaut== Vrai)
				pCtxt << " checked>";
			else
				pCtxt << " >";
		}
		break;

	// 31 A mettre dans 41
	case ELEMENTINTERFACESelecteurTaxiBus:
		{
			tBool3 TaxiBusDefaut = (tBool3) _Parametres[ELEMENTINTERFACESelecteurTaxiBusDefaut]->Nombre(__Parametres);

			if (TaxiBusDefaut == Vrai)
				pCtxt << " checked>";
			else
				pCtxt << " >";
		}
		break;

	// 34
	case ELEMENTINTERFACEChampsReservation:
		{						
			// affichage du champ nom
			pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationNom]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_NOM << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOM << "\"/>";
			pCtxt << "</td></tr>";					

			// affichage du champ pr�nom
			tChampsResa bPrenom = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationPrenomObligatoire]->Nombre(__Parametres);
			if (bPrenom == eChampFacultatif || bPrenom == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationPrenom]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_PRENOM << "\" class=\"frmField\" size=\"35\">";
				if (bPrenom == eChampObligatoire)
				{
					pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_PRENOM << "\"/>";
				}
				pCtxt << "</td></tr>";
			}
			
			// affichage du champ adresse
			tChampsResa bAdresse = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationAdresseObligatoire]->Nombre(__Parametres);
			if (bAdresse == eChampFacultatif || bAdresse == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationAdresse]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_ADRESSE << "\" class=\"frmField\" size=\"35\">";
				if (bAdresse == eChampObligatoire)
				{
					pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE << "\"/>";
				}
				pCtxt << "</td></tr>";
			}

			// affichage du champ email
			tChampsResa bEmail = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationEmailObligatoire]->Nombre(__Parametres);
			if (bEmail == eChampFacultatif || bEmail == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationEmail]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_EMAIL << "\" class=\"frmField\" size=\"35\">";
				if (bEmail == eChampObligatoire)
				{
					pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_EMAIL << "\"/>";
				}
				pCtxt << "</td></tr>";
			}

			// affichage du champ t�l�phone
			tChampsResa bTelephone = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationTelephoneObligatoire]->Nombre(__Parametres);
			if (bTelephone == eChampFacultatif || bTelephone == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationTelephone]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_TELEPHONE << "\" class=\"frmField\" size=\"35\">";
				if (bTelephone == eChampObligatoire)
				{
					pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_TELEPHONE << "\"/>";
				}
				pCtxt << "</td></tr>";
			}

			// affichage du champ num�ro d'abonn�
			tChampsResa bNumAbonne = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationNumeroAbonneObligatoire]->Nombre(__Parametres);
			if (bNumAbonne == eChampFacultatif || bNumAbonne == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationNumeroAbonne]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_CLIENT_NUMERO_ABONNE << "\" class=\"frmField\" size=\"35\">";
				if (bNumAbonne == eChampObligatoire)
				{
					pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NUMERO_ABONNE << "\"/>";
				}
				pCtxt << "</td></tr>";
			}

			// affichage du champ adresse d�part
			tChampsResa bAdresseDepart = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationAdresseDepartObligatoire]->Nombre(__Parametres);
			if (bAdresseDepart == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationAdresseDepart]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_ADRESSE_DEPART << "\" class=\"frmField\" size=\"35\">";
				pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_DEPART << "\"/></td></tr>";
			}

			// affichage du champ adresse arriv�e
			tChampsResa bAdresseArrivee = (tChampsResa) _Parametres[ELEMENTINTERFACEChampsReservationAdresseArriveeObligatoire]->Nombre(__Parametres);
			if (bAdresseArrivee == eChampObligatoire)
			{
				pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationAdresseArrivee]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_ADRESSE_ARRIVEE << "\" class=\"frmField\" size=\"35\">";
				pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_ARRIVEE << "\"/></td></tr>";
			}
			
			// affichage du champ nombre de places
			pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationNbPlaces]->Texte(__Parametres) << "<td><input type=\"text\" name=\"" << REQUETE_COMMANDE_NOMBRE_PLACES << "\" class=\"frmField\" size=\"35\">";
			pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOMBRE_PLACES << "\"/>";
			pCtxt << "</td></tr>";
			
			pCtxt << "<tr>" << _Parametres[ELEMENTINTERFACEChampsReservationWarning]->Texte(__Parametres);	
			pCtxt << "</tr>";
		}
		break;
		
	
	case ELEMENTINTERFACEFeuillesRoute:
		{
			// Collecte des param�tres
			const cTrajets* __Trajets = (const cTrajets*) __Objet;
			
			// Affichage de chaque feuille de route
			for (int __n=0; __n < __Trajets->Taille(); __n++)
			{
				// Cr�ation de la liste de param�tres
				cInterface_Objet_Connu_ListeParametres __ParametresColonne;
				
				// 0 : Index du trajet
				__ParametresColonne << __n + 1;

				// 1 : Etat filtre handicap�
				__ParametresColonne << _Parametres[ELEMENTINTERFACEFeuillesRouteFiltreHandicape]->Nombre(__Parametres);

				// 2 : Etat filtre v�lo
				__ParametresColonne << _Parametres[ELEMENTINTERFACEFeuillesRouteFiltreVelo]->Nombre(__Parametres);

				// 3 : Index trajet pr�c�dent
				__ParametresColonne << (__n > 0 ? __n : 0);

				// 4 : Index trajet suivant
				__ParametresColonne << (__n < __Trajets->Taille() - 1 ? __n + 2 : 0);

				// 5 : Date du d�part (format interne)
				cTexteCodageInterne __txtDate;
				__txtDate << __Trajets->operator [](__n).getMomentDepart().getDate();
				__ParametresColonne << __txtDate;
				
				// Lancement de l'affichage
				__Site->Affiche(pCtxt, INTERFACEFeuilleRoute, __ParametresColonne, (const void*) &(__Trajets->operator[](__n)));
			}
		}
		break;
	
			
	case ELEMENTINTERFACEFeuilleRoute:
		{
			// Collecte des param�tres
			const cTrajet* __Trajet = (const cTrajet*) __Objet;
			int __FiltreHandicape = (_Parametres[ELEMENTINTERFACEFeuilleRouteFiltreHandicape]->Nombre(__Parametres) > 1);
			int __FiltreVelo = (_Parametres[ELEMENTINTERFACEFeuilleRouteFiltreVelo]->Nombre(__Parametres) > 1);
			
			// Affichage de chaque ligne de la feuille de route
			bool __Couleur = false;
			for (const cElementTrajet* __ET = __Trajet->PremierElement(); __ET != NULL; __ET = __ET->Suivant())
			{
				// LIGNE ARRET MONTEE Si premier point d'arr�t et si alerte
				if(__ET == __Trajet->PremierElement())
				{
					cMoment debutPrem, finPrem;
					debutPrem = __ET->MomentDepart();
					finPrem = debutPrem;
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						finPrem += __Trajet->getAmplitudeServiceContinu();
							
					if (__ET->getGareDepart()->getAlerte()->showMessage(debutPrem,finPrem))
					{
						cInterface_Objet_Connu_ListeParametres __ParametresMontee;
						__ParametresMontee << 0;
						__ParametresMontee << __ET->getGareDepart()->getAlerte()->getMessage();
						__ParametresMontee << __ET->getGareDepart()->getAlerte()->Niveau();
						__ParametresMontee << "";
						__ParametresMontee << *__ET->getGareDepart();
						__ParametresMontee << (__Couleur ? "1" : "");
						__Couleur = !__Couleur;

						__Site->Affiche(pCtxt, INTERFACELigneFeuilleRouteArret, __ParametresMontee, NULL);
					}
				}
				
				if (__ET->getLigne()->Materiel()->Code() != MATERIELPied)
				{
					// LIGNE CIRCULATIONS
					cMoment debutLigne, finLigne, tempMoment;
					debutLigne = __ET->MomentDepart();
					finLigne = __ET->MomentArrivee();
					
					cInterface_Objet_Connu_ListeParametres __ParametresLigne;
					
					// 0/1 Heures de d�part
					__ParametresLigne << __ET->MomentDepart().getHeure();	//0
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
					{
						tempMoment = __ET->MomentDepart();
						tempMoment += __Trajet->getAmplitudeServiceContinu();
						__ParametresLigne << tempMoment.getHeure();		//1
					}
					else
						__ParametresLigne << "";						//1
					
					// 2/3 Heures d'arriv�e
					__ParametresLigne << __ET->MomentArrivee().getHeure();	//2
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
					{
						tempMoment = __ET->MomentArrivee();
						tempMoment += __Trajet->getAmplitudeServiceContinu();
						__ParametresLigne << tempMoment.getHeure();	//3
						finLigne = tempMoment;
					}
					else
						__ParametresLigne << "";					//3
					
					// 4/5/6 Description du mat�riel roulant
					__ParametresLigne << __ET->getLigne()->Materiel()->Code();	//4
					__ParametresLigne << __ET->getLigne()->Materiel()->getLibelleSimple(); //5
					cTexteHTML LibelleCompletMatosHTML;	//!< \todo PROVISOIRE FAIRE UN OBJET STANDARD LIBELLECOMPLETMATOS
//					__ET->getLigne()->LibelleComplet(LibelleCompletMatosHTML);
					__ParametresLigne << LibelleCompletMatosHTML; //6
	
					// 7 Destination du v�hicule
					cTexteHTML DestinationHTML;	//!< \todo PROVISOIRE FAIRE UN OBJET STANDARD POINT D'ARRET pas si simple : il faut g�rer la destination forc�e
//					__ET->getLigne()->LibelleDestination(DestinationHTML);
					__ParametresLigne << DestinationHTML;	//7
					
					// 8/9 Prise en charge des handicap�s
					__ParametresLigne << __FiltreHandicape;	//8
					__ParametresLigne << __ET->getLigne()->getHandicape()->getContenance();	//9
	
					// 10/11 Prise en charge des v�los
					__ParametresLigne << __FiltreVelo;	//10
					__ParametresLigne << __ET->getLigne()->getVelo()->Contenance();	//11
					
					// 12/18 R�servation
					cMoment maintenant;
					bool __ResaOuverte = false;
					maintenant.setMoment();
					if (__ET->getLigne()->GetResa()->TypeResa() == Obligatoire
					&&	__ET->getLigne()->GetResa()->reservationPossible(__ET->getLigne()->GetTrain(__ET->getService()), maintenant, __ET->MomentDepart())
					) {
						__ParametresLigne << "1"; //12
						__ResaOuverte = true;
					}
					else
						__ParametresLigne << ""; //12
					
					maintenant.setMoment();
					if (__ET->getLigne()->GetResa()->TypeResa() == Facultative
					&&	__ET->getLigne()->GetResa()->reservationPossible(__ET->getLigne()->GetTrain(__ET->getService()), maintenant, __ET->MomentDepart())
					) {
						__ParametresLigne << "1"; //13
						__ResaOuverte = true;
					}
					else
						__ParametresLigne << ""; //13
					
					if (__ResaOuverte)
					{	
						__ParametresLigne << __ET->getLigne()->GetResa()->momentLimiteReservation(__ET->getLigne()->GetTrain(__ET->getService()), __ET->MomentDepart());	//14
						
						__ParametresLigne << __ET->getLigne()->GetResa()->GetTelephone(); //15
						__ParametresLigne << __ET->getLigne()->GetResa()->GetHorairesTelephone(); //16
						__ParametresLigne << __ET->getLigne()->GetResa()->GetSiteWeb();	//17
						if (__Site->ResaEnLigne() && __ET->getLigne()->GetResa()->ReservationEnLigne())
						{
							cTexte __URLResa;
							cTexteRequeteSYNTHESE __Requete;
							__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_FORMULAIRE_RESA);
							__Requete.AddParam(REQUETE_COMMANDE_SITE, __Site->getClef());
							__Requete.AddParam(REQUETE_COMMANDE_CODE_LIGNE, __ET->getLigne()->getCode());
							__Requete.AddParam(REQUETE_COMMANDE_NUMERO_SERVICE, (__ET->getLigne()->GetTrain(__ET->getService()))->getNumero());
							__Requete.AddParam(REQUETE_COMMANDE_CODE_RESA, __ET->getLigne()->GetResa()->Index());
							__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART, __ET->getGareDepart()->Index());
							__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE, __ET->getGareArrivee()->Index());
							__Requete.AddParam(REQUETE_COMMANDE_DATE, __ET->MomentDepart());
							__URLResa << __Site->getURLClient() << "?" << __Requete;
							__ParametresLigne << __URLResa;	//18
						}
						else
							__ParametresLigne << "";	//18
					}
					else
					{
						__ParametresLigne << "";	//14
						__ParametresLigne << "";	//15
						__ParametresLigne << "";	//16
						__ParametresLigne << "";	//17
						__ParametresLigne << "";	//18
					}
					
					// 19/20 Alertes
					if (__ET->getLigne()->getAlerte()->showMessage(debutLigne, finLigne))
					{
						__ParametresLigne << __ET->getLigne()->getAlerte()->getMessage();	//19
						__ParametresLigne << __ET->getLigne()->getAlerte()->Niveau();		//20
					}
					else
					{
						__ParametresLigne << "";	//19
						__ParametresLigne << "";	//20
					}

					// 21 Couleur de la ligne de tableau
					__ParametresLigne << (__Couleur ? "1" : "");
					__Couleur = !__Couleur;
					
					// Lancement de l'affichage
					__Site->Affiche(pCtxt, INTERFACELigneFeuilleRouteCirculation, __ParametresLigne, __ET->getLigne());
					
					// LIGNE ARRET DE DESCENTE
					cInterface_Objet_Connu_ListeParametres __ParametresDescente;
					
					
					// 0 Descente
					__ParametresDescente << "1";	//0
					
					// 1/2 Message d'alerte
					cMoment debutArret, finArret;
					debutArret = __ET->MomentArrivee();
					finArret = debutArret;
					if (__ET->Suivant() != NULL)
						finArret = __ET->Suivant()->MomentDepart();
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						finArret += __Trajet->getAmplitudeServiceContinu();
					if (__ET->getGareArrivee()->getAlerte()->showMessage(debutArret,finArret))
					{
						__ParametresDescente << __ET->getGareArrivee()->getAlerte()->getMessage();	//1
						__ParametresDescente << __ET->getGareArrivee()->getAlerte()->Niveau();		//2
					}
					else
					{
						__ParametresDescente << "";	//1
						__ParametresDescente << "";	//2
					}	
									
					// 3/4 Informations sur le point d'arr�t 
					__ParametresDescente << (__ET->getGareArrivee() == __ET->getLigne()->DerniereGareLigne()->ArretLogique() ? "1" : "");
					cTexteHTML NomArret;	//!< \todo PROVISOIRE METTRE HTML EN MEMOIRE QUELQUE PART
					NomArret << *__ET->getGareArrivee();
					__ParametresDescente << NomArret; //4

					// 5 Couleur du fond de case
					__ParametresDescente << (__Couleur ? "1" : "");
					__Couleur = !__Couleur;

					// 6/7 Heures d'arriv�e
					__ParametresDescente << __ET->MomentArrivee().getHeure();	//6
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
					{
						tempMoment = __ET->MomentArrivee();
						tempMoment += __Trajet->getAmplitudeServiceContinu();
						__ParametresDescente << tempMoment.getHeure();	//7
					}
					else
						__ParametresDescente << "";					//7
					
					__Site->Affiche(pCtxt, INTERFACELigneFeuilleRouteArret, __ParametresDescente);
				}
				else
				{	
					// LIGNE JONCTION A PIED (si applicable)
					cInterface_Objet_Connu_ListeParametres __ParametresJonction;
					
					__ParametresJonction << __ET->getGareArrivee()->Index();	// 0
					
					// 1/2 Alerte	
					cMoment debutArret, finArret;
					debutArret = __ET->MomentArrivee();
					finArret = debutArret;
					if (__ET->Suivant() != NULL)
						finArret = __ET->Suivant()->MomentDepart();
					if (__Trajet->getAmplitudeServiceContinu().Valeur())
						finArret += __Trajet->getAmplitudeServiceContinu();
					if (__ET->getGareArrivee()->getAlerte()->showMessage(debutArret,finArret))
					{
						__ParametresJonction << __ET->getGareArrivee()->getAlerte()->getMessage();	// 1
						__ParametresJonction << __ET->getGareArrivee()->getAlerte()->Niveau();	// 2
					}
					else
					{
						__ParametresJonction << "";	// 1
						__ParametresJonction << "";	// 2
					}

					// 3 Couleur du fond de case
					__ParametresJonction << (__Couleur ? "1" : "");
					__Couleur = !__Couleur;

					// Lancement de l'affichage
					__Site->Affiche(pCtxt, INTERFACELigneFeuilleRouteJonction, __ParametresJonction);
				}
			}
		}
		break;
	
	case EI_BIBLIOTHEQUE_Trajet_Duree:
		{
			// Collecte des param�tres
			const cTrajet* __Trajet = (const cTrajet*) __Objet;

			cInterface_Objet_Connu_ListeParametres __Parametres;
			
			//0 : Dur�e du trajet
			__Parametres << __Trajet->getDuree().Valeur();
			
			//1 : Dur�e < 1h ?
			__Parametres << (__Trajet->getDuree().Valeur() < MINUTES_PAR_HEURE ? "1" : "");
				
			//2 : Nombre d'heures
			__Parametres << (__Trajet->getDuree().Valeur() / MINUTES_PAR_HEURE);
			
			//3 : Nombre de minutes
			__Parametres << (__Trajet->getDuree().Valeur() % MINUTES_PAR_HEURE);

			//4 : Nombre de minutes sup�rieur � 10 ou inf�rieur � 60 ?
			__Parametres << (__Trajet->getDuree().Valeur() < MINUTES_PAR_HEURE || __Trajet->getDuree().Valeur() % MINUTES_PAR_HEURE >= 10 ? "1" : "");
			
			// Lancement de l'affichage					
			__Site->Affiche(pCtxt, INTERFACEDuree, __Parametres, NULL);
		}
		break;
		
	case EI_BIBLIOTHEQUE_FicheHoraire_LigneDurees:
		{
			// Collecte des param�tres
  			const cTrajets* __Trajets = (const cTrajets*) __Objet;
			
			// Affichage de chaque feuille de route
			for (int __n=0; __n < __Trajets->Taille(); __n++)
			{
				cInterface_Objet_Connu_ListeParametres __ParametresCase;
				__ParametresCase << __n + 1;
					
				__Site->Affiche(pCtxt, INTERFACECaseDuree, __ParametresCase, (const void*) &(__Trajets->operator[](__n)));
			}
		
		}
		break;
		
	case ELEMENTINTERFACEURLFormulaire:
		{	
			// Initialisation des param�tres
			const cTexte& __TypeSortie = _Parametres[ELEMENTINTERFACEURLFormulaireTypeSortie]->Texte(__Parametres);
			const cTexte& __Fonction = _Parametres[ELEMENTINTERFACEURLFormulaireFonction]->Texte(__Parametres);
			
			// Fabrication de la requ�te
			cTexteRequeteSYNTHESE __Requete;
			
			// Site d'affichage
			__Requete.AddParam(REQUETE_COMMANDE_SITE, __Site->getClef());
			
			// Parametres cas validation fiche horaire
			if (__Fonction.Compare("timetable validation"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_VALID_FICHE_HORAIRE);
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_COMMUNE_DEPART
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_DESIGNATION_DEPART
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_COMMUNE_ARRIVEE
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_DESIGNATION_ARRIVEE
					, _Parametres[ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee]->Texte(__Parametres));
			}	
			// Pour fiche horaire seulement
			else if (__Fonction.Compare("timetable"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_FICHE_HORAIRE);
				__Requete.AddParam(REQUETE_COMMANDE_DATE
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHDate]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_PERIODE
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHPeriode]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_VELO
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHVelo]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_HANDICAPE
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHHandicape]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_TAXIBUS
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHResa]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_TARIF
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHTarif]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_DESIGNATION_DEPART
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee]->Texte(__Parametres));
				__Requete.AddParam(REQUETE_COMMANDE_NUMERO_DESIGNATION_ARRIVEE
					, _Parametres[ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee]->Texte(__Parametres));
			}
			else if (__Fonction.Compare("from city list"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_LISTE_COMMUNE);
				__Requete.AddParam(REQUETE_COMMANDE_SENS, 1);
			}
			else if (__Fonction.Compare("to city list"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_LISTE_COMMUNE);
				__Requete.AddParam(REQUETE_COMMANDE_SENS, 0);
			}
			else if (__Fonction.Compare("from station list"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_LISTE_POINT_ARRET);
				__Requete.AddParam(REQUETE_COMMANDE_SENS, 1);
			}
			else if (__Fonction.Compare("to station list"))
			{
				__Requete.AddParam(REQUETE_COMMANDE_FONCTION, FONCTION_LISTE_POINT_ARRET);
				__Requete.AddParam(REQUETE_COMMANDE_SENS, 0);
			}
			
			// Affichage de la requ�te au format voulu
			if (__TypeSortie.Compare("url"))
			{
				// Partie ex�cutable de l'url
				pCtxt << __Site->getURLClient() << "?";
				
				// Champ fonction
				pCtxt << __Requete;
			}
			else if (__TypeSortie.Compare("form", 4))
			{
				// Tag d'ouverture du formulaire
				pCtxt	<< "<form method=\"get\" action=\"" << __Site->getURLClient() << "\" " 
						<< __TypeSortie.Extrait(5) << ">";
	
				__Requete.AfficheFormulaireHTML(pCtxt);
			}		
		}
		break;
		
	case ELEMENTINTERFACEInputHTML:	//41
		{
			// Initialisation des param�tres
			const cTexte& __Champ = _Parametres[ELEMENTINTERFACEInputHTMLChamp]->Texte(__Parametres);
			const cTexte& __Type = _Parametres[ELEMENTINTERFACEInputHTMLType]->Texte(__Parametres);
						
			// Balise d'ouverture
			cTexte __Balise;
			if (!__Type.Taille())
			{
				if (__Champ.Compare("date") || __Champ.Compare("period"))
					__Balise = "select";
				else if (__Champ.Compare("handicap filter"))
					__Balise = "input type=\"checkbox\"";
				else
					__Balise = "input type=\"text\"";
			}
			else
			{
				if (__Type.Compare("text"))
					__Balise = "input type=\"text\"";
				else if (__Type.Compare("select"))
					__Balise = "select";
				else if (__Type.Compare("checkbox", 8))
					__Balise = "input type=\"checkbox\"";
			}
			
			// Ecriture de l'ouverture
			pCtxt	<< "<" << __Balise << " " << _Parametres[ELEMENTINTERFACEInputHTMLSuite]->Texte(__Parametres);
					
			// Ecriture du nom du champ
			pCtxt << " name=\"";
			if (__Champ.Compare("date"))
				pCtxt << REQUETE_COMMANDE_DATE;
			else if (__Champ.Compare("period"))
				pCtxt << REQUETE_COMMANDE_PERIODE;
			else if (__Champ.Compare("from city"))
				pCtxt << REQUETE_COMMANDE_COMMUNE_DEPART;
			else if (__Champ.Compare("from station"))
				pCtxt << REQUETE_COMMANDE_POINT_ARRET_DEPART;
			else if (__Champ.Compare("to city"))
				pCtxt << REQUETE_COMMANDE_COMMUNE_ARRIVEE;
			else if (__Champ.Compare("to station"))
				pCtxt << REQUETE_COMMANDE_POINT_ARRET_ARRIVEE;
			else if (__Champ.Compare("handicap filter"))
				pCtxt << REQUETE_COMMANDE_HANDICAPE;
			else if (__Champ.Compare("tariff"))
				pCtxt << REQUETE_COMMANDE_TARIF;
			pCtxt << "\"";
			
			// Cas champ checkbox
			if (__Balise.Compare("input type=\"checkbox\""))
			{
				// Etat coch�
				tBool3 __Bool3Defaut = Faux;
				
				// Chackbox filtre sur valeur
				if (__Type.Taille() > 9)
				{
					if (_Parametres[ELEMENTINTERFACEInputHTMLValeurDefaut]->Texte(__Parametres).Compare(__Type, 0, 0, 9))
						__Bool3Defaut = Vrai;
					pCtxt << " value=\"" << __Type.Extrait(9) << "\"";
				}
				else	//Checkbox bool�en
				{
					__Bool3Defaut = (tBool3) _Parametres[ELEMENTINTERFACEInputHTMLValeurDefaut]->Nombre(__Parametres);
					pCtxt << " value=\"1\"";
				}
				if (__Bool3Defaut == Vrai)
					pCtxt << " checked=\"1\"";
				pCtxt << " />";
			}
			if (__Balise.Compare("input type=\"text\"")) // Cas champ input texte
			{
				pCtxt << " value=\"" << _Parametres[ELEMENTINTERFACEInputHTMLValeurDefaut]->Texte(__Parametres) << "\" />";
			}
			else if (__Balise.Compare("select")) // Cas champ select
			{
				pCtxt << ">";
			 	
				// Si select auto alors fourniture des choix
				if (!__Type.Taille())
				{
					if (__Champ.Compare("date"))
					{					
						// Collecte des param�tres sp�cifiques
						const cEnvironnement*	__Environnement = __Site->getEnvironnement();
						cDate DateDefaut = __Environnement->dateInterpretee(_Parametres[ELEMENTINTERFACEInputHTMLValeurDefaut]->Texte(__Parametres));
						cDate DateMin = __Environnement->dateInterpretee(_Parametres[ELEMENTINTERFACEInputHTMLListeDatesMin]->Texte(__Parametres));
						cDate __DateJour;
						__DateJour.setDate();
						if (!__Site->getSolutionsPassees() && DateMin < __DateJour)
							DateMin = __DateJour;
						cDate DateMax = __Environnement->dateInterpretee(_Parametres[ELEMENTINTERFACEInputHTMLListeDatesMax]->Texte(__Parametres));
						
						// Construction de l'objet HTML
						cTexteCodageInterne DateInterne;
						for (cDate iDate = DateMin; iDate <= DateMax; iDate++)
						{
							DateInterne.Vide();
							DateInterne << iDate;
							
							pCtxt << "<option ";
							if (iDate == DateDefaut)
								pCtxt << "selected=\"1\" ";
							pCtxt << "value=\"" << DateInterne << "\">";
							__Site->getInterface()->AfficheDate(pCtxt, iDate);
							pCtxt << "</option>";
						}
					}
					else if (__Champ.Compare("period"))
					{
						tIndex __IndexPeriodeDefaut = _Parametres[ELEMENTINTERFACEInputHTMLValeurDefaut]->Nombre(__Parametres);
			
						for (int iPeriode=0; __Site->getInterface()->GetPeriode(iPeriode); iPeriode++)
						{
							pCtxt << "<option ";
							if (iPeriode == __IndexPeriodeDefaut)
								pCtxt << "selected ";
							pCtxt << "value=\"" << iPeriode << "\">" << __Site->getInterface()->GetPeriode(iPeriode)->Libelle() << "</option>";
						}
					}
					pCtxt << "</select>";
				}
			}
			
			
			
		}
		break;
	
	case EI_BIBLIOTHEQUE_Interface_PrefixeAlerte:	//42
		{
			// Lecture des param�tres
			tIndex __Niveau = _Parametres[EI_BIBLIOTHEQUE_Interface_PrefixeAlerte_Niveau]->Nombre(__Parametres);

			// Sortie
			if (__Niveau > 0)
				pCtxt << __Site->getInterface()->getPrefixeAlerte(__Niveau);
		}
		break;


	case EI_BIBLIOTHEQUE_Ligne_Destination:	//43
		{
			// Lecture des param�tres
			const cLigne* __Ligne = (const cLigne*) __Objet;

			// Affichage de la girouette
			if (__Ligne->getGirouette().Taille())
				pCtxt << __Ligne->getGirouette();
			else	// Affichage du terminus
			{
				// Cr�ation d'un objet de param�tres
				cInterface_Objet_Connu_ListeParametres __ParametresLigne;

				// 0: D�signation principale
				__ParametresLigne << "0";
				
				// Lancement de l'affichage
				__Site->Affiche(pCtxt, INTERFACENomArret, __ParametresLigne, (const void*) __Ligne->DerniereGareLigne()->ArretLogique());
			}
		}
		break;
	
	case EI_BIBLIOTHEQUE_DescriptionPassage_Ligne:	//44
		{
			const cLigne* __Ligne = ((const cDescriptionPassage*) __Objet)->getGareLigne()->Ligne();
			cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
			__ParametresCaseLigne << _Parametres[EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLDebutLigne]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLFinLigne]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_LargeurCase]->Texte(__Parametres);
			__ParametresCaseLigne << _Parametres[EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HauteurCase]->Texte(__Parametres);
			
			__Site->Affiche(pCtxt, INTERFACECartoucheLigne, __ParametresCaseLigne, (const void*) __Ligne);
		}
		break;

	// 45
	case EI_BIBLIOTHEQUE_Nombre_Formate:
		{
			// Lecture des param�tres
			int __Nombre = _Parametres[0]->Nombre(__Parametres) + _Parametres[2]->Nombre(__Parametres);
			int __Chiffres = _Parametres[1]->Nombre(__Parametres);
			

			for (; __Chiffres; __Chiffres--)
			{
				if (__Nombre < pow(10.0, __Chiffres - 1))
					pCtxt << "0";
			}
			pCtxt << __Nombre;
		}
		break;

	case EI_BIBLIOTHEQUE_DescriptionPassage_Destination:	//46
		{
			// Lecture des param�tres
			const cDescriptionPassage* __DP = (const cDescriptionPassage*) __Objet;
			cTexte __DestinationsAAfficher = _Parametres[0]->Texte(__Parametres);
			bool __AfficherTerminus = (_Parametres[1]->Nombre(__Parametres) == 1);
			cTexte __TypeAffichage = _Parametres[2]->Texte(__Parametres);
			cTexte __SeparateurEntreArrets = _Parametres[3]->Texte(__Parametres);
			cTexte __AvantCommune = _Parametres[4]->Texte(__Parametres);
			cTexte __ApresCommune = _Parametres[5]->Texte(__Parametres);
			
			const cCommune* __DerniereCommune = __DP->GetGare(0)->getCommune();
			
			for (tIndex __i=1; __i < __DP->NombreGares(); __i++)
			{
				if (__DestinationsAAfficher.Compare("all") && (__i < __DP->NombreGares() - 1 || __AfficherTerminus)
					|| __DestinationsAAfficher.Compare("terminus") && __i == __DP->NombreGares() - 1
					|| __DestinationsAAfficher.GetNombre() == __i && (__i < __DP->NombreGares() - 1 || __AfficherTerminus)
				){
					if (__i > 1)
						pCtxt << __SeparateurEntreArrets;
					
					// Affichage de la commune dans les cas o� n�cessaire
					if (__TypeAffichage.Compare("station_city")
						|| __TypeAffichage.Compare("station_city_if_new") && __DP->GetGare(__i)->getCommune() != __DerniereCommune
					){
						cTexteMinuscules __TexteMinuscule;
						__TexteMinuscule << __DP->GetGare(__i)->getCommune()->GetNom();
						pCtxt << __AvantCommune << __TexteMinuscule << __ApresCommune;
						__DerniereCommune = __DP->GetGare(__i)->getCommune();
					}

					// Affichage du nom d'arr�t dans les cas o� n�cessaire
					if (__TypeAffichage.Compare("station", 6))
						pCtxt << __DP->GetGare(__i)->getNom();

					// Affichage de la destination 13 caract�res dans les cas o� n�cessaire
					if (__TypeAffichage.Compare("char(13)"))
						pCtxt << __DP->GetGare(__i)->getDesignation13();

					// Affichage de la destination 26 caract�res dans les cas o� n�cessaire
					if (__TypeAffichage.Compare("char(26)"))
						pCtxt << __DP->GetGare(__i)->getDesignation26();
				}
			}
		}
		break;

	case EI_BIBLIOTHEQUE_DescriptionPassage_ArretPhysique:	//47
		break;

	case EI_BIBLIOTHEQUE_DescriptionPassage_Heure:	//48
		{
			// Lecture des param�tres
			const cMoment& __Moment = ((const cDescriptionPassage*) __Objet)->getMomentReel();
			cTexte __Zero = _Parametres[0]->Texte(__Parametres);
			cTexte __AvantSiImminent = _Parametres[1]->Texte(__Parametres);
			cTexte __ApresSiImminent = _Parametres[2]->Texte(__Parametres);

			cMoment __Maintenant;
			__Maintenant.setMoment(TEMPS_ACTUEL);
			cDureeEnMinutes __Duree = __Moment - __Maintenant;
			
			if (__Duree.Valeur() <= 1)
				pCtxt << __AvantSiImminent;
			if (__Moment.getHeure().Heures() < 10)
				pCtxt << __Zero;
            pCtxt << __Moment.getHeure();
			if (__Duree.Valeur() <= 1)
				pCtxt << __ApresSiImminent;
		}
		break;

	case EI_BIBLIOTHEQUE_Cases_Particularites:	//49
		{
			const cTrajets* __Trajets = (const cTrajets*) __Objet;

			for (tIndex __i=0; __i<__Trajets->Taille(); __i++)
			{
				// Cr�ation d'un objet de param�tres
				cInterface_Objet_Connu_ListeParametres __ParametresCase;

				//0
				__ParametresCase << __i + 1;
				
				// 1: D�signation principale
				__ParametresCase << (__Trajets->operator [](__i)).getNiveauMaxAlerte();
				
				// Lancement de l'affichage
				__Site->Affiche(pCtxt, INTERFACECaseParticularite, __ParametresCase, NULL);
			
			}
		}
		break;

	case EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau:	//50
		{
			// Lecture des param�tres
//			const cTableauAffichage* __Tb = (const cTableauAffichage*) __Objet;
			tIndex __Nombre = _Parametres[0]->Nombre(__Parametres);
			cTexte __Format = _Parametres[1]->Texte(__Parametres);

			if (__Format.Compare("char(2)"))
			{
				__Nombre = __Nombre % 100;
				pCtxt << __Nombre / 10;
				pCtxt << __Nombre % 10;
			}
            
		}
		break;


	}

	
	return INCONNU;
}
