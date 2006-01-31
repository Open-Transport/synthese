/*!	\file cFichierXML.cpp
	\brief Classe responsable du parsing d'un fichier d'environnement
	       en XML et qui cree les objets metiers correspondants.
*/

#include "cFichierXML.h"
#include "cEnvironnement.h"
#include "cArretPhysique.h"
#include "cCommune.h"
#include "cAccesPADe.h"
#include "cAxe.h"
#include "cHeure.h"
#include "cDureeEnMinutes.h"
#include "cJourCirculation.h"
#include "cHandicape.h"
#include "cVelo.h"
#include "cPhoto.h"
#include "cInterface.h"
#include "cModaliteReservation.h"
#include "cModaliteReservationEnLigne.h"
#include "cTarif.h"
#include "SYNTHESE.h"

#include "cTableauAffichage.h"
#include "cTableauAffichageSelectifDestinations.h"

#include <fstream>
#include "xmlParser.h"

#include "cDate.h"


const std::string cFichierXML::SYNTHESE_TAG = "synthese";

const std::string cFichierXML::ENVIRONNEMENT_TAG = "environnement";
const std::string cFichierXML::ENVIRONNEMENT_ID_ATTR = "id";
const std::string cFichierXML::ENVIRONNEMENT_NOM_ATTR = "nom";
const std::string cFichierXML::ENVIRONNEMENT_REP_ATTR = "rep";

const std::string cFichierXML::DONNEES_TRANSPORT_TAG = "donnees_transport";

const std::string cFichierXML::ARRET_LOGIQUE_TAG = "arret_logique";
const std::string cFichierXML::ARRET_LOGIQUE_ID_ATTR = "id";
const std::string cFichierXML::ARRET_LOGIQUE_TYPE_ATTR = "type";
const std::string cFichierXML::ARRET_LOGIQUE_TYPE_ATTR_CORR_AUTORISEE = "correspondance";
const std::string cFichierXML::ARRET_LOGIQUE_TYPE_ATTR_CORR_INTERDITE = "passage";
const std::string cFichierXML::ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE = "echange_courte_distance";
const std::string cFichierXML::ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE_COURT = "echange_longue_distance";

const std::string cFichierXML::ARRET_LOGIQUE_X_ATTR = "x";
const std::string cFichierXML::ARRET_LOGIQUE_Y_ATTR = "y";

const std::string cFichierXML::ARRET_PHYSIQUE_TAG = "arret_physique";
const std::string cFichierXML::ARRET_PHYSIQUE_RANG_ATTR = "rang";
const std::string cFichierXML::ARRET_PHYSIQUE_NOM_ATTR = "nom";
const std::string cFichierXML::ARRET_PHYSIQUE_X_ATTR = "x";
const std::string cFichierXML::ARRET_PHYSIQUE_Y_ATTR = "y";
	
const std::string cFichierXML::DELAI_TAG = "delai";
const std::string cFichierXML::DELAI_ORIGINE_ATTR = "origine";
const std::string cFichierXML::DELAI_DESTINATION_ATTR = "destination";
const std::string cFichierXML::DELAI_VALEUR_ATTR = "valeur";


const std::string cFichierXML::DESIGNATION_TAG = "designation";
const std::string cFichierXML::DESIGNATION_NOM_COMMUNE_ATTR = "nom_commune";
const std::string cFichierXML::DESIGNATION_NOM_ARRET_ATTR = "nom_arret";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR = "type";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_QUELCONQUE = "quelconque";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_PRINCIPALE = "principale";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_TOUT_LIEU = "tout_lieu";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_LIEU_PUBLIC = "lieu_public";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_LIEU_QUELCONQUE = "lieu_quelconque";
const std::string cFichierXML::DESIGNATION_TYPE_ATTR_PRINCIPALE_DEFAUT = "principale_defaut";
	
const std::string cFichierXML::AXE_TAG = "axe";
const std::string cFichierXML::AXE_TYPE_ATTR = "type";
const std::string cFichierXML::AXE_TYPE_ATTR_STANDARD = "standard";
const std::string cFichierXML::AXE_TYPE_ATTR_LIBRE = "libre";
const std::string cFichierXML::AXE_TYPE_ATTR_INTERDIT = "interdit";
const std::string cFichierXML::AXE_CODE_ATTR = "code";

const std::string cFichierXML::INTERDICTION_CORRESPONDANCE_TAG = "interdiction_correspondance";
const std::string cFichierXML::INTERDICTION_CORRESPONDANCE_AXE_ATTR = "axe";

const std::string cFichierXML::LIGNE_TAG = "ligne";
const std::string cFichierXML::LIGNE_CODE_ATTR = "code";
const std::string cFichierXML::LIGNE_RESEAU_ATTR = "reseau";
const std::string cFichierXML::LIGNE_RESERVATION_ATTR = "reservation";
const std::string cFichierXML::LIGNE_CARTOUCHE_ATTR = "cartouche";
const std::string cFichierXML::LIGNE_STYLE_ATTR = "style";
const std::string cFichierXML::LIGNE_INDICATEUR_ATTR = "indicateur";
const std::string cFichierXML::LIGNE_MATERIEL_ATTR = "materiel";
const std::string cFichierXML::LIGNE_TARIF_ATTR = "tarif";
const std::string cFichierXML::LIGNE_ATTENTE_ATTR = "attente";
const std::string cFichierXML::LIGNE_IMAGE_ATTR = "image";
const std::string cFichierXML::LIGNE_LIBELLE_COMPLET_ATTR = "libelle_complet";
const std::string cFichierXML::LIGNE_VELO_ATTR = "velo";
const std::string cFichierXML::LIGNE_HANDICAPE_ATTR = "handicape";
const std::string cFichierXML::LIGNE_AFFICHAGE_TABLEAU_DEPART_ATTR = "affichage_tableau_depart";
const std::string cFichierXML::LIGNE_AFFICHAGE_INDICATEUR_ATTR = "affichage_indicateur";

const std::string cFichierXML::ARRET_LIGNE_TAG = "arret_ligne";
const std::string cFichierXML::ARRET_LIGNE_PM_ATTR = "pm";
const std::string cFichierXML::ARRET_LIGNE_ID_ATTR = "id";
const std::string cFichierXML::ARRET_LIGNE_QUAI_ATTR = "quai";
const std::string cFichierXML::ARRET_LIGNE_TYPE_ATTR = "type";
const std::string cFichierXML::ARRET_LIGNE_TYPE_DEPART_ATTR = "depart";
const std::string cFichierXML::ARRET_LIGNE_TYPE_PASSAGE_ATTR = "passage";
const std::string cFichierXML::ARRET_LIGNE_TYPE_ARRIVEE_ATTR = "arrivee";

const std::string cFichierXML::ARRET_LIGNE_HORAIRES_ATTR = "horaires";

const std::string cFichierXML::CALENDRIERS_TAG = "calendriers";
const std::string cFichierXML::CALENDRIERS_VALEURS_ATTR = "valeurs";

const std::string cFichierXML::NUMEROS_TAG = "numeros";
const std::string cFichierXML::NUMEROS_VALEURS_ATTR = "valeurs";

const std::string cFichierXML::ARRET_TAG = "arret";
const std::string cFichierXML::ARRET_PM_ATTR = "pm";
const std::string cFichierXML::ARRET_ID_ATTR = "id";
const std::string cFichierXML::ARRET_QUAI_ATTR = "quai";
const std::string cFichierXML::ARRET_TYPE_ATTR = "type";
const std::string cFichierXML::ARRET_DEPARTS_ATTR = "departs";
const std::string cFichierXML::ARRET_PASSAGES_ATTR = "passages";
const std::string cFichierXML::ARRET_ARRIVEES_ATTR = "arrivees";

const std::string cFichierXML::POINT_TAG = "point";
const std::string cFichierXML::POINT_PM_ATTR = "pm";
const std::string cFichierXML::POINT_X_ATTR = "x";
const std::string cFichierXML::POINT_Y_ATTR = "y";


const std::string cFichierXML::MATERIEL_TAG = "materiel";
const std::string cFichierXML::MATERIEL_ID_ATTR = "id";
const std::string cFichierXML::MATERIEL_VITESSE_ATTR = "vitesse";

const std::string cFichierXML::CALENDRIER_TAG = "calendrier";
const std::string cFichierXML::CALENDRIER_ID_ATTR = "id";
const std::string cFichierXML::CALENDRIER_DEBUT_SERVICE_ATTR = "debut_service";
const std::string cFichierXML::CALENDRIER_FIN_SERVICE_ATTR = "fin_service";

const std::string cFichierXML::DATE_TAG = "date";
const std::string cFichierXML::DATE_DATE_MIN_ATTR = "date_min";
const std::string cFichierXML::DATE_DATE_MAX_ATTR = "date_max";
const std::string cFichierXML::DATE_PAS_ATTR = "pas";
const std::string cFichierXML::DATE_SENS_ATTR = "sens";

const std::string cFichierXML::CALENDRIER_INCLUS_TAG = "calendrier_inclus";
const std::string cFichierXML::CALENDRIER_INCLUS_REF_ATTR = "ref";
const std::string cFichierXML::CALENDRIER_INCLUS_SENS_ATTR = "sens";


const std::string cFichierXML::MODALITE_RESA_TAG = "modalite_resa";
const std::string cFichierXML::MODALITE_RESA_ID_ATTR = "id";
const std::string cFichierXML::MODALITE_RESA_NOM_ATTR = "nom";
const std::string cFichierXML::MODALITE_RESA_TYPE_ATTR = "type";

const std::string cFichierXML::MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE = "obligatoire";
const std::string cFichierXML::MODALITE_RESA_TYPE_ATTR_FACULTATIVE = "facultative";
const std::string cFichierXML::MODALITE_RESA_TYPE_ATTR_IMPOSSIBLE = "impossible";
const std::string cFichierXML::MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE_COLLECTIVEMENT = "obligatoire_collectivement";

const std::string cFichierXML::MODALITE_RESA_MINUTES_MIN_ATTR = "minutes_min";
const std::string cFichierXML::MODALITE_RESA_EN_LIGNE_ATTR = "en_ligne";
const std::string cFichierXML::MODALITE_RESA_CHAMP_TELEPHONE_ATTR = "champ_telephone";
const std::string cFichierXML::MODALITE_RESA_CHAMP_PRENOM_ATTR = "champ_prenom";

const std::string cFichierXML::TARIF_TAG = "tarif";
const std::string cFichierXML::TARIF_ID_ATTR = "id";
const std::string cFichierXML::TARIF_TYPE_ATTR = "type";
const std::string cFichierXML::TARIF_TYPE_ATTR_ZONAGE = "zonage";
const std::string cFichierXML::TARIF_TYPE_ATTR_SECTION = "section";
const std::string cFichierXML::TARIF_TYPE_ATTR_DISTANCE = "distance";
const std::string cFichierXML::TARIF_LIBELLE_ATTR = "libelle";

const std::string cFichierXML::HANDICAPE_TAG = "handicape";
const std::string cFichierXML::HANDICAPE_ID_ATTR = "id";
const std::string cFichierXML::HANDICAPE_TYPE_ATTR = "type";
const std::string cFichierXML::HANDICAPE_TYPE_ATTR_ACCEPTE = "accepte";
const std::string cFichierXML::HANDICAPE_TYPE_ATTR_INTERDIT = "interdit";
const std::string cFichierXML::HANDICAPE_TYPE_ATTR_INCONNU = "inconnu";
const std::string cFichierXML::HANDICAPE_CONTENANCE_ATTR = "contenance";
const std::string cFichierXML::HANDICAPE_PRIX_ATTR = "prix";
const std::string cFichierXML::HANDICAPE_RESA_ATTR = "resa";
const std::string cFichierXML::HANDICAPE_DOC_ATTR = "doc";

const std::string cFichierXML::VELO_TAG = "velo";
const std::string cFichierXML::VELO_ID_ATTR = "id";
const std::string cFichierXML::VELO_TYPE_ATTR = "type";
const std::string cFichierXML::VELO_TYPE_ATTR_ACCEPTE = "accepte";
const std::string cFichierXML::VELO_TYPE_ATTR_INTERDIT = "interdit";
const std::string cFichierXML::VELO_TYPE_ATTR_INCONNU = "inconnu";
const std::string cFichierXML::VELO_CONTENANCE_ATTR = "contenance";
const std::string cFichierXML::VELO_PRIX_ATTR = "prix";
const std::string cFichierXML::VELO_RESA_ATTR = "resa";
const std::string cFichierXML::VELO_DOC_ATTR = "doc";

const std::string cFichierXML::PHOTO_TAG = "photo";
const std::string cFichierXML::PHOTO_ID_ATTR = "id";
const std::string cFichierXML::PHOTO_DESIGNATION_GENERALE_ATTR = "designation_generale";
const std::string cFichierXML::PHOTO_DESIGNATION_LOCALE_ATTR = "designation_locale";

const std::string cFichierXML::LIEN_PHOTO_TAG = "lien_photo";
const std::string cFichierXML::LIEN_PHOTO_ZONE_CLICKABLE_ATTR = "zone_clickable";
const std::string cFichierXML::LIEN_PHOTO_INDICATION_ATTR = "indication";
const std::string cFichierXML::LIEN_PHOTO_PHOTO_CIBLE_ATTR = "photo_cible";

const std::string cFichierXML::SITE_TAG = "site";
const std::string cFichierXML::SITE_ID_ATTR = "id";
const std::string cFichierXML::SITE_ENVIRONNEMENT_ATTR = "environnement";
const std::string cFichierXML::SITE_INTERFACE_ATTR = "interface";
const std::string cFichierXML::SITE_DESIGNATION_ATTR = "designation";
const std::string cFichierXML::SITE_DATE_DEBUT_ATTR = "date_debut";
const std::string cFichierXML::SITE_DATE_FIN_ATTR = "date_fin";
const std::string cFichierXML::SITE_URL_CLIENT_ATTR = "url_client";
const std::string cFichierXML::SITE_RESA_EN_LIGNE_ATTR = "resa_en_ligne";
const std::string cFichierXML::SITE_SOLUTIONS_PASSEES_ATTR = "solutions_passees";

const std::string cFichierXML::INTERFACE_TAG = "interface";
const std::string cFichierXML::INTERFACE_ID_ATTR = "id";
const std::string cFichierXML::INTERFACE_NOM_ATTR = "nom";

const std::string cFichierXML::ELEMENT_INTERFACE_TAG = "element";
const std::string cFichierXML::ELEMENT_INTERFACE_PARAMS_ATTR = "params";
const std::string cFichierXML::ELEMENT_INTERFACE_SRC_ATTR = "src";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR = "role";

const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_ENTETE = "entete";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_PIED = "pied";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FORMULAIRE_ENTREE = "formulaire_entree";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_NOM_ARRET = "nom_arret";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_DESIGNATION_LIGNE = "designation_ligne";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_CARTOUCHE_LIGNE = "cartouche_ligne";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_DEPART = "liste_communes_depart";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_ARRIVEE = "liste_communes_arrivee";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_DEPART = "liste_arrets_depart";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_ARRIVEE = "liste_arrets_arrivee";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_ERREUR_ARRETS_FICHE_HORAIRE = "erreur_arrets_fiche_horaire";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_ATTENTE = "attente";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP_GARE = "tb_dep_gare";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP = "tb_dep";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FORM_RESA = "form_resa";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_VALID_RESA = "valid_resa";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_COLONNE = "fiche_horaire_colonne";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FEUILLE_ROUTE = "feuille_route";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_ARRET = "ligne_feuille_route_arret";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_CIRCULATION = "ligne_feuille_route_circulation";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_JONCTION = "ligne_feuille_route_jonction";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FICHE_ARRET = "ligne_fiche_arret";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_CASE_LIGNES = "ligne_case_lignes";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_AUCUNE_SOLUTION_FICHE_HORAIRE = "aucune_solution_fiche_horaire";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_CASE_DUREE = "case_duree";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_CASE_RENVOI = "case_renvoi";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_LIGNE = "fiche_horaire_ligne";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_DUREE = "duree";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_MINI_TB_DEP_GARE = "mini_tb_dep_gare";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_PAGE_ACCUEIL = "page_accueil";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE = "fiche_horaire";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_TABLEAU_DEPART = "ligne_tableau_depart";
const std::string cFichierXML::ELEMENT_INTERFACE_ROLE_ATTR_CASE_PARTICULARITE = "case_particularite";



const std::string cFichierXML::TABLEAU_TAG = "tableau";
const std::string cFichierXML::TABLEAU_CODE_ATTR = "code";
const std::string cFichierXML::TABLEAU_TYPE_ATTR = "type";
const std::string cFichierXML::TABLEAU_ENVIRONNEMENT_ATTR = "environnement";
const std::string cFichierXML::TABLEAU_INTERFACE_ATTR = "interface";
const std::string cFichierXML::TABLEAU_TITRE_ATTR = "titre";
const std::string cFichierXML::TABLEAU_ORIGINES_SEULEMENT_ATTR = "origines_seulement";
const std::string cFichierXML::TABLEAU_NOMBRE_RANGEES_ATTR = "nombre_rangees";
const std::string cFichierXML::TABLEAU_TRI_ATTR = "tri";
const std::string cFichierXML::TABLEAU_TRI_ATTR_CHRONOLOGIQUE = "chronologique";
const std::string cFichierXML::TABLEAU_TRI_ATTR_SELECTIF = "selectif";

const std::string cFichierXML::REF_ARRET_LOGIQUE_TAG = "ref_arret_logique";
const std::string cFichierXML::REF_ARRET_LOGIQUE_ID_ATTR = "id";
const std::string cFichierXML::REF_ARRET_PHYSIQUE_TAG = "ref_arret_physique";
const std::string cFichierXML::REF_ARRET_PHYSIQUE_ID_ATTR = "id";

const std::string cFichierXML::DESTINATION_AFFICHEE_TAG = "destination_affichee";
const std::string cFichierXML::DESTINATION_AFFICHEE_REF_ATTR = "ref";

const std::string cFichierXML::DESTINATION_SELECTION_TAG = "destination_selection";
const std::string cFichierXML::DESTINATION_SELECTION_REF_ATTR = "ref";

const std::string cFichierXML::RESEAU_TAG = "reseau";
const std::string cFichierXML::RESEAU_ID_ATTR = "id";
const std::string cFichierXML::RESEAU_NOM_ATTR = "nom";
const std::string cFichierXML::RESEAU_URL_ATTR = "url";
const std::string cFichierXML::RESEAU_CARTOUCHE_HTML_ATTR = "cartouche_html";
const std::string cFichierXML::RESEAU_DOC_HTML_ATTR = "doc_html";

const std::string cFichierXML::INDICATEURS_TAG = "indicateurs";
const std::string cFichierXML::INDICATEURS_BASE_JC_ATTR = "base_jc";

const std::string cFichierXML::INDICATEUR_TAG = "indicateur";
const std::string cFichierXML::INDICATEUR_TITRE_ATTR = "titre";
const std::string cFichierXML::INDICATEUR_MASQUE_JC_ATTR = "masque_jc";
const std::string cFichierXML::INDICATEUR_COMMENCE_PAGE_ATTR = "commence_page";

const std::string cFichierXML::INDICATEUR_ARRET_TAG = "indicateur_arret";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_ARRET_ATTR = "type_arret";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_ARRET_ATTR_DEPART = "depart";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_ARRET_ATTR_PASSAGE = "passage";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_ARRET_ATTR_ARRIVEE = "arrivee";
const std::string cFichierXML::INDICATEUR_ARRET_REF_ATTR = "ref";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR = "type_indicateur";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_OBLIGATOIRE = "obligatoire";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_FACULTATIF = "facultatif";
const std::string cFichierXML::INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_SUFFISANT = "suffisant";



cFichierXML::cFichierXML(const std::string& basePath)
: _basePath (basePath)
{
}



cFichierXML::~cFichierXML()
{
}


void 
cFichierXML::log (const std::string& message) {
	std::cout << "LOG   " << message << std::endl;
}






SYNTHESE*
cFichierXML::chargeSynthese (const std::string& filename) 
{
	
	log ("Chargement de synthese...");

	std::string configPath (_basePath + "/" + filename);
	
    // Creation de l'arbre XML
	XMLNode node = XMLNode::parseFile(configPath.c_str(), SYNTHESE_TAG.c_str());
	
	// Creation de l'objet environnement
	SYNTHESE* synth = new SYNTHESE();
	
	for (int i=0; i<node.nChildNode(); ++i) 
	{
		XMLNode childNode = node.getChildNode(i);
		// const XMLNodeContents& childElt = node.enumContents(i);
		std::string name = childNode.getName ();
		
		if (name == ENVIRONNEMENT_TAG) 
		  {
		  	// Cree un nouvel environnement
		  	int nbCalculateurs = 1;
		  	cEnvironnement* env = new cEnvironnement (nbCalculateurs);
		  	
			int id = atoi (childNode.getAttribute (ENVIRONNEMENT_ID_ATTR.c_str()));
			std::string nom (childNode.getAttribute (ENVIRONNEMENT_NOM_ATTR.c_str()));
			std::string rep (childNode.getAttribute (ENVIRONNEMENT_REP_ATTR.c_str()));
			
			env->SetIndex(id);
			//! \todo dates min et max heritees depuis main args
			
			std::string donneesEnvPath (_basePath);
			donneesEnvPath += "/";
			donneesEnvPath += rep;
			donneesEnvPath += "/";
			donneesEnvPath += nom;
			donneesEnvPath += ".xml";
			
			std::ifstream ifs (donneesEnvPath.c_str());
			chargeDonneesTransport (ifs, *env);
			
		    synth->Enregistre(env, id);
		    
		  }
		 else if (name == INTERFACE_TAG) 
		  {
		    chargeInterface (childNode, *synth);
		  }
		 else if (name == SITE_TAG) 
		  {
		    chargeSite (childNode, *synth);
		  }
		 else if (name == TABLEAU_TAG) 
		  {
		  	std::cout << name << std::endl;
		    chargeTableauDeparts (childNode, *synth);
		  }

	}
	
	log ("Chargement termine...");
	
	return synth;
}





void
cFichierXML::chargeDonneesTransport (istream& xmlStream, cEnvironnement& env) 
{
	// transfert vers une string 
	char ch;
	std::string buf("");
	while (xmlStream.get(ch)) buf += ch;
	
    // Creation de l'arbre XML
	XMLNode root = XMLNode::parseString(buf.c_str(), DONNEES_TRANSPORT_TAG.c_str());


	return chargeDonneesTransport(root, env);	
}




void
cFichierXML::chargeDonneesTransport (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement donnees transport...");
	
	for (int i=0; i<node.nChildNode(); ++i) 
	{
		XMLNode childNode = node.getChildNode(i);
		// const XMLNodeContents& childElt = node.enumContents(i);
		std::string name = childNode.getName ();
		
		if (name == ARRET_LOGIQUE_TAG) 
		  {
		    chargeArretLogique (childNode, env);
		  }
		 else if (name == AXE_TAG) 
		  {
		    chargeAxe (childNode, env);
		  }
		 else if (name == MATERIEL_TAG) 
		  {
		    chargeMateriel (childNode, env);
		  }
		 else if (name == CALENDRIER_TAG) 
		  {
		    chargeCalendrier (childNode, env);
		  }
		  else if (name == MODALITE_RESA_TAG) 
		  {
		  	chargeModaliteResa (childNode, env);
		  }
		  else if (name == TARIF_TAG) 
		  {
		  	chargeTarif (childNode, env);
		  }
		  else if (name == HANDICAPE_TAG) 
		  {
		  	chargeHandicape (childNode, env);
		  }
		  else if (name == VELO_TAG) 
		  {
		  	chargeVelo (childNode, env);
		  }
		  else if (name == PHOTO_TAG) 
		  {
		  	chargePhoto (childNode, env);
		  }
		  else if (name == INDICATEURS_TAG) 
		  {
		  	chargeIndicateurs (childNode, env);
		  }
		  
	}
}





void
cFichierXML::chargeArretLogique (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement arret logique...");
	
	const char* idAttr = node.getAttribute (ARRET_LOGIQUE_ID_ATTR.c_str());
	int id = atoi (idAttr);

	const char* typeAttr = node.getAttribute (ARRET_LOGIQUE_TYPE_ATTR.c_str());
	tNiveauCorrespondance curNiveauCorrespondance = CorrInterdite;
	if (typeAttr == ARRET_LOGIQUE_TYPE_ATTR_CORR_AUTORISEE) {
		curNiveauCorrespondance = CorrAutorisee;
	} else if (typeAttr == ARRET_LOGIQUE_TYPE_ATTR_CORR_INTERDITE) {
		curNiveauCorrespondance = CorrInterdite;
	} else if (typeAttr == ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE) {
		curNiveauCorrespondance = CorrRecommandee;
	} else if (typeAttr == ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE_COURT) {
		curNiveauCorrespondance = CorrRecommandeeCourt;
	}
		
	const char* xAttr = node.getAttribute (ARRET_LOGIQUE_X_ATTR.c_str());
	int x = atoi (xAttr); 
	
	const char* yAttr = node.getAttribute (ARRET_LOGIQUE_Y_ATTR.c_str());
	int y = atoi (yAttr);
	
	cArretLogique* al = env.GetGareAvecCreation(curNiveauCorrespondance, id);

	al->SetPoint(x, y);

	// Designations
	int nbDesignations = node.nChildNode(DESIGNATION_TAG.c_str());
	for (int i=0; i<nbDesignations; ++i) {
		XMLNode designationNode = node.getChildNode(DESIGNATION_TAG.c_str(), i);
		
		const char* nomCommuneAttr = designationNode.getAttribute (DESIGNATION_NOM_COMMUNE_ATTR.c_str());
		std::string typeAttr (designationNode.getAttribute (DESIGNATION_TYPE_ATTR.c_str()));
		const char* nomArretAttr = designationNode.getAttribute (DESIGNATION_NOM_ARRET_ATTR.c_str());
		
		
		tTypeAccesPADe type = ePrincipaleDefaut;
		if (typeAttr == DESIGNATION_TYPE_ATTR_PRINCIPALE) {
			type = 	ePrincipale;
		} else if (typeAttr == DESIGNATION_TYPE_ATTR_TOUT_LIEU) {
			type = eToutLieu;
		} else if (typeAttr == DESIGNATION_TYPE_ATTR_LIEU_PUBLIC) {
			type = eLieuPublic;
		} else if (typeAttr == DESIGNATION_TYPE_ATTR_LIEU_QUELCONQUE) {
			type = eLieuQuelconque;
		} else if (typeAttr == DESIGNATION_TYPE_ATTR_PRINCIPALE_DEFAUT) {
			type = ePrincipaleDefaut;
		}
		
		cCommune* commune = env.GetCommuneAvecCreation (nomCommuneAttr);
		cTexte nomArret (nomArretAttr);
		cAccesPADe* designation = new cAccesPADe (al, commune, nomArret);
		env.Enregistre(designation, type);
		
	}		


	// Arrets physiques
	int nbArretsPhysiques = node.nChildNode(ARRET_PHYSIQUE_TAG.c_str());
	al->setNbArretPhysiques(nbArretsPhysiques);
	
	for (int i=0; i<nbArretsPhysiques; ++i) {
		XMLNode apNode = node.getChildNode(ARRET_PHYSIQUE_TAG.c_str(), i);
		
		const char* nomAttr = apNode.getAttribute (ARRET_PHYSIQUE_NOM_ATTR.c_str());
		int rang = atoi (apNode.getAttribute (ARRET_PHYSIQUE_RANG_ATTR.c_str()));
		int x = atoi (apNode.getAttribute (ARRET_PHYSIQUE_X_ATTR.c_str()));
		int y = atoi (apNode.getAttribute (ARRET_PHYSIQUE_Y_ATTR.c_str()));
		
		cArretPhysique* ap = new cArretPhysique ();

		if (!al->AddArretPhysique(rang, ap))
		{
			delete ap;
		} else {
			cTexte nom (nomAttr);
			ap->setNom(nom);
			ap->setX(x);
			ap->setY(y);
			//!	\todo Ajouter photo dans la spe xml ?
		}
		
		// Delais de correspondance entre arrets physiques
		int nbDelais = node.nChildNode(DELAI_TAG.c_str());
		for (int i=0; i<nbDelais; ++i) {
			XMLNode delaiNode = node.getChildNode(DELAI_TAG.c_str(), i);
			int rangOrigine = atoi (delaiNode.getAttribute (DELAI_ORIGINE_ATTR.c_str()));
			int rangDestination = atoi (delaiNode.getAttribute (DELAI_DESTINATION_ATTR.c_str()));
			int valeurDelai = atoi (delaiNode.getAttribute (DELAI_VALEUR_ATTR.c_str()));
			al->setDelaiCorrespondance(rangOrigine, rangDestination, valeurDelai);
		}
		
	    //!	\todo Ajouter Photo ArretLogique ?
		
		//!	\todo Ajouter Services ?
		
		//!	\todo A FAIRE Ajouter Alertes ?	une seul message par aret logique aujourd'hui
		
		
		al->Ferme();
		
		
	}
	
	
}









void
cFichierXML::chargeMateriel (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement materiel...");
	
	int id = atoi (node.getAttribute (MATERIEL_ID_ATTR.c_str()));
	int vitesse = atoi (node.getAttribute (MATERIEL_VITESSE_ATTR.c_str()));
	
	cMateriel* materiel = new cMateriel (id);
	materiel->setVitesseMoyenneMax(vitesse);
	
	//!	\todo materiel->setArticle();
	//!	\todo materiel->setIndicateur();
	//!	\todo materiel->setLibelleSimple();   A METTRE DANS MAP
	
	env.Enregistre(materiel, id);
}






void
cFichierXML::chargeCalendrier (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement calendrier...");
	
	int id = atoi (node.getAttribute (CALENDRIER_ID_ATTR.c_str()));
	int anneeDebutService = atoi(node.getAttribute (CALENDRIER_DEBUT_SERVICE_ATTR.c_str()));
	int anneeFinService = atoi(node.getAttribute (CALENDRIER_FIN_SERVICE_ATTR.c_str()));
	env.SetDatesService(anneeDebutService, anneeFinService);
	
	//!	\todo intitule / categorie ??
	
	cJC* jc = new cJC (env.PremiereAnnee () , env.DerniereAnnee (), cTexte (""));
	
	int nbDate = node.nChildNode(DATE_TAG.c_str());
	for (int i=0; i<nbDate; ++i) {
		XMLNode dateNode = node.getChildNode(DATE_TAG.c_str(), i);
		std::string date_min (dateNode.getAttribute (DATE_DATE_MIN_ATTR.c_str()));
		int dminAnnee = atoi (date_min.substr(0, 4).c_str());
		int dminMois = atoi (date_min.substr(5, 2).c_str());
		int dminJour = atoi (date_min.substr(8, 2).c_str());

		std::string sensAttr (dateNode.getAttribute (DATE_SENS_ATTR.c_str()));
		tSens sens = (sensAttr == "positif") ? Positif : Negatif;
			
		cDate dmin;
		dmin.setDate(dminJour, dminMois, dminAnnee);
		
		if (dateNode.getAttribute (DATE_DATE_MAX_ATTR.c_str()) == NULL) {
			// Si pas de date_max => date unique
			jc->SetCircule (dmin, sens);
			
		} else {
				
			std::string date_max (dateNode.getAttribute (DATE_DATE_MAX_ATTR.c_str()));
	
			int dmaxAnnee = atoi (date_max.substr(0, 4).c_str());
			int dmaxMois = atoi (date_max.substr(5, 2).c_str());
			int dmaxJour = atoi (date_max.substr(8, 2).c_str());
			
			int pas = atoi (dateNode.getAttribute (DATE_PAS_ATTR.c_str()));
			
			cDate dmax;
			dmax.setDate(dmaxJour, dmaxMois, dmaxAnnee);
			
			jc->SetCircule (dmin, dmax, sens, pas);
		}
			
	}
	
	int nbCalendriersInclus = node.nChildNode(CALENDRIER_INCLUS_TAG.c_str());
	for (int i=0; i<nbCalendriersInclus; ++i) {
		XMLNode calendrierInclusNode = node.getChildNode(CALENDRIER_INCLUS_TAG.c_str(), i);
		int refid = atoi (calendrierInclusNode.getAttribute (CALENDRIER_INCLUS_REF_ATTR.c_str())); 
		std::string sensAttr (calendrierInclusNode.getAttribute (CALENDRIER_INCLUS_SENS_ATTR.c_str()));
		tSens sens = (sensAttr == "positif") ? Positif : Negatif;
		
		cJC* inclus = env.GetJC(refid);
		jc->SetInclusionToMasque(*inclus, sens);
	}
	
	env.Enregistre(jc, id);


}






void
cFichierXML::chargeModaliteResa (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement modalite resa...");
	
	int id = atoi (node.getAttribute (MODALITE_RESA_ID_ATTR.c_str()));
	std::string nomAttr (node.getAttribute (MODALITE_RESA_NOM_ATTR.c_str()));
	std::string typeAttr (node.getAttribute (MODALITE_RESA_NOM_ATTR.c_str()));
	int minutesMin = atoi (node.getAttribute (MODALITE_RESA_MINUTES_MIN_ATTR.c_str()));
	bool enLigne = atoi (node.getAttribute (MODALITE_RESA_EN_LIGNE_ATTR.c_str())) == 1;
	std::string champTelephoneAttr (node.getAttribute (MODALITE_RESA_CHAMP_TELEPHONE_ATTR.c_str()));

	cModaliteReservation* mresa = enLigne ? new cModaliteReservationEnLigne()
									: new cModaliteReservation ();
			
	tResa type = Obligatoire;
	if (typeAttr == MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE) {
		type = Obligatoire;
	} else if (typeAttr == MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE_COLLECTIVEMENT) {
		type = ObligatoireCollectivement;
	} else if (typeAttr == MODALITE_RESA_TYPE_ATTR_FACULTATIVE) {
		type = Facultative;
	} else if (typeAttr == MODALITE_RESA_TYPE_ATTR_IMPOSSIBLE) {
		type = Impossible;
	}
		
	mresa->SetDoc(cTexte (nomAttr.c_str()));
	mresa->setTypeResa(type);
	
	 
	mresa->SetDelaiMinMinutes(minutesMin);
	//!	\todo heure ?? addmresa->SetDelaiMinHeureMax(cHeure (...));
	
	mresa->SetTel(cTexte (champTelephoneAttr.c_str()));
	
	if (enLigne) {
		std::string champPrenomAttr (node.getAttribute (MODALITE_RESA_CHAMP_PRENOM_ATTR.c_str()));
				
		cModaliteReservationEnLigne* mresael = (cModaliteReservationEnLigne*) mresa;
		//!	\todo champPrenom ??  mresael->SetRELPrenom()
	}	
	
	env.Enregistre(mresa, id);
	
}





void 
cFichierXML::chargeTarif (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement tarif...");
	
	int id = atoi (node.getAttribute (TARIF_ID_ATTR.c_str()));
	std::string typeAttr (node.getAttribute (TARIF_TYPE_ATTR.c_str()));
	std::string libelleAttr (node.getAttribute (TARIF_LIBELLE_ATTR.c_str()));

	tTypeTarif type = Zonage;
	if (typeAttr == TARIF_TYPE_ATTR_ZONAGE) {
		type = Zonage;
	} else if (typeAttr == TARIF_TYPE_ATTR_DISTANCE) {
		type = Distance;
	} else if (typeAttr == TARIF_TYPE_ATTR_SECTION) {
		type = Section;
	}		

	cTarif* tarif = new cTarif ();
	tarif->setNumeroTarif(id);
	tarif->setTypeTarif(type);
	tarif->setLibelle(cTexte(libelleAttr.c_str()));
		
	env.Enregistre(tarif, id);
	
}


void 
cFichierXML::chargeHandicape (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement handicape...");
	
	int id = atoi (node.getAttribute (HANDICAPE_ID_ATTR.c_str()));
	std::string typeAttr (node.getAttribute (HANDICAPE_TYPE_ATTR.c_str()));
	int contenance = atoi (node.getAttribute (HANDICAPE_CONTENANCE_ATTR.c_str()));
	int prix = atoi (node.getAttribute (HANDICAPE_PRIX_ATTR.c_str()));
	int resa = atoi (node.getAttribute (HANDICAPE_RESA_ATTR.c_str()));
	std::string docAttr (node.getAttribute (HANDICAPE_DOC_ATTR.c_str()));
	
	tBool3 type = Indifferent;
	if (typeAttr == HANDICAPE_TYPE_ATTR_INCONNU) {
		type = Indifferent;
	} else if (typeAttr == HANDICAPE_TYPE_ATTR_ACCEPTE) {
		type = Vrai;
	} else if (typeAttr == HANDICAPE_TYPE_ATTR_INTERDIT) {
		type = Faux;
	}
	
	cHandicape* handi = new cHandicape ();
	handi->setTypeHandicape(type);
	handi->setContenance(contenance);
	handi->setPrix(prix);
	handi->setResa(env.getResa(resa));
	handi->setDoc(cTexte (docAttr.c_str()));
	
	env.Enregistre(handi, id);
}


void 
cFichierXML::chargeVelo (XMLNode& node, cEnvironnement& env)
{
	log ("Chargement velo...");
	
	int id = atoi (node.getAttribute (VELO_ID_ATTR.c_str()));
	std::string typeAttr (node.getAttribute (VELO_TYPE_ATTR.c_str()));
	int contenance = atoi (node.getAttribute (VELO_CONTENANCE_ATTR.c_str()));
	int prix = atoi (node.getAttribute (VELO_PRIX_ATTR.c_str()));
	int resa = atoi (node.getAttribute (VELO_RESA_ATTR.c_str()));
	std::string docAttr (node.getAttribute (VELO_DOC_ATTR.c_str()));
	
	tBool3 type = Indifferent;
	if (typeAttr == VELO_TYPE_ATTR_INCONNU) {
		type = Indifferent;
	} else if (typeAttr == VELO_TYPE_ATTR_ACCEPTE) {
		type = Vrai;
	} else if (typeAttr == VELO_TYPE_ATTR_INTERDIT) {
		type = Faux;
	}
	
	cVelo* velo = new cVelo ();
	velo->setTypeVelo(type);
	velo->setContenance(contenance);
	velo->setPrix(prix);
	velo->setResa(env.getResa(resa));
	velo->setDoc(cTexte (docAttr.c_str()));
	
	env.Enregistre(velo, id);
	
}




void 
cFichierXML::chargePhoto (XMLNode& node, cEnvironnement& env)
{
	log ("Chargement photo...");
	
	int id = atoi (node.getAttribute (PHOTO_ID_ATTR.c_str()));
	std::string degenAttr (node.getAttribute (PHOTO_DESIGNATION_GENERALE_ATTR.c_str()));
	std::string delocAttr (node.getAttribute (PHOTO_DESIGNATION_LOCALE_ATTR.c_str()));
	
	cPhoto* photo = new cPhoto (id);
	photo->setDescriptionGenerale(cTexte (degenAttr.c_str()));
	photo->setDescriptionLocale(cTexte (delocAttr.c_str()));
	
	int nbLiens = node.nChildNode(LIEN_PHOTO_TAG.c_str());
	for (int i=0; i<nbLiens; ++i) {
		XMLNode lienNode = node.getChildNode(LIEN_PHOTO_TAG.c_str(), i);
		std::string zclickable (lienNode.getAttribute (LIEN_PHOTO_ZONE_CLICKABLE_ATTR.c_str()));
		std::string indication (lienNode.getAttribute (LIEN_PHOTO_INDICATION_ATTR.c_str()));
		int idCible = atoi (lienNode.getAttribute (LIEN_PHOTO_PHOTO_CIBLE_ATTR.c_str()));
		
		cTexteHTML alt;
		alt << indication.c_str ();

		photo->addMapPhoto(cTexte(zclickable.c_str()), idCible, alt);
	}
	// env.Enregistre (photo);
	
}


void 
cFichierXML::chargeReseau (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement reseau...");
	
	int id = atoi (node.getAttribute (RESEAU_ID_ATTR.c_str()));
	std::string nom (node.getAttribute (RESEAU_NOM_ATTR.c_str()));
	std::string url (node.getAttribute (RESEAU_URL_ATTR.c_str()));
	std::string cartoucheHtml (node.getAttribute (RESEAU_CARTOUCHE_HTML_ATTR.c_str()));
	std::string docHtml (node.getAttribute (RESEAU_DOC_HTML_ATTR.c_str()));
	
	cReseau* reseau = new cReseau ();
	reseau->setNom(cTexte (nom.c_str()));
	reseau->setURL(cTexte (url.c_str ()));
	reseau->setDoc(cTexte (docHtml));
	//! \todo cartouche ??

	env.Enregistre(reseau, id);
}



void 
cFichierXML::chargeIndicateurs (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement indicateurs...");

	int baseJcId = atoi (node.getAttribute (INDICATEURS_BASE_JC_ATTR.c_str()));
	cJC* jcBase = env.GetJC(baseJcId);
	
	int nbIndicateurs = node.nChildNode(INDICATEUR_TAG.c_str());
	for (int i=0; i<nbIndicateurs; ++i) {
		XMLNode indicateurNode = node.getChildNode(INDICATEUR_TAG.c_str(), i);
		std::string titre (indicateurNode.getAttribute (INDICATEUR_TITRE_ATTR.c_str()));
		int masqueJcId = atoi (indicateurNode.getAttribute (INDICATEUR_MASQUE_JC_ATTR.c_str()));
		bool commencePage = atoi (indicateurNode.getAttribute (INDICATEUR_COMMENCE_PAGE_ATTR.c_str())) > 0;

		cJC* jcMasque = env.GetJC(masqueJcId);

		cIndicateurs* indicateur = new cIndicateurs (cTexte (titre.c_str ()), &env);
		indicateur->setCommencePage(commencePage);
		indicateur->setJC(*jcMasque, *jcBase);
		
		int nbIndicateurArrets = indicateurNode.nChildNode(INDICATEUR_ARRET_TAG.c_str());
		for (int j=0; j<nbIndicateurArrets; ++j) {
			
			XMLNode indicateurArretNode = indicateurNode.getChildNode(INDICATEUR_ARRET_TAG.c_str(), j);
			std::string typeArret (indicateurArretNode.getAttribute (INDICATEUR_ARRET_TYPE_ARRET_ATTR.c_str()));		
			int refid = atoi (indicateurArretNode.getAttribute (INDICATEUR_ARRET_REF_ATTR.c_str()));
			std::string typeIndicateur (indicateurArretNode.getAttribute (INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR.c_str()));

			cArretLogique* refAL = env.getArretLogique(refid);

			tTypeGareLigneDA typeDA = Depart;
			if (typeArret == INDICATEUR_ARRET_TYPE_ARRET_ATTR_DEPART) {
				typeDA = Depart;				
			} else if (typeArret == INDICATEUR_ARRET_TYPE_ARRET_ATTR_PASSAGE) {
				typeDA = Passage;				
			} else if (typeArret == INDICATEUR_ARRET_TYPE_ARRET_ATTR_ARRIVEE) {
				typeDA = Arrivee;				
			}
				
			tTypeGareIndicateur	typeGareInd = PassageFacultatif;
			if (typeIndicateur == INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_OBLIGATOIRE) {
				typeGareInd = PassageObligatoire;
			} else if (typeIndicateur == INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_FACULTATIF) {
				typeGareInd = PassageFacultatif;
			} else if (typeIndicateur == INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_SUFFISANT) {
				typeGareInd = PassageSuffisant;
			}
			
			indicateur->addArretLogique(refAL, typeDA, typeGareInd);
			
		}
	}
	
}
		


void
cFichierXML::chargeAxe (XMLNode& node, cEnvironnement& env) 
{
	log ("Chargement axe...");
	
	std::string codeAttr (node.getAttribute (AXE_CODE_ATTR.c_str()));
	std::string typeAttr (node.getAttribute (AXE_TYPE_ATTR.c_str()));
	
	
	bool libre = false;
	bool autorise = false;
	
	if (typeAttr == AXE_TYPE_ATTR_STANDARD) {
		libre = false;
		autorise = true;
	} else if (typeAttr == AXE_TYPE_ATTR_LIBRE) {
		libre = true;
		autorise = true;
	} else if (typeAttr == AXE_TYPE_ATTR_INTERDIT) {
		libre = false;
		autorise = false;
	}

	cAxe* axe = new cAxe (libre, cTexte (codeAttr.c_str ()), autorise);
	
	
	
	int nbInterdictions = node.nChildNode(INTERDICTION_CORRESPONDANCE_TAG.c_str());
	for (int i=0; i<nbInterdictions; ++i) {
		XMLNode interdictionNode = node.getChildNode(INTERDICTION_CORRESPONDANCE_TAG.c_str(), i);
		//! \todo non gere pour l'instant...
	}
	
	int nbLignes = node.nChildNode(LIGNE_TAG.c_str());
	for (int i=0; i<nbLignes; ++i) {
		XMLNode ligneNode = node.getChildNode(LIGNE_TAG.c_str(), i);
		
		std::string code (ligneNode.getAttribute (LIGNE_CODE_ATTR.c_str()));
		int reseau = atoi (ligneNode.getAttribute (LIGNE_RESEAU_ATTR.c_str()));
		int reservation = atoi (ligneNode.getAttribute (LIGNE_RESERVATION_ATTR.c_str()));
		std::string cartouche (ligneNode.getAttribute (LIGNE_CARTOUCHE_ATTR.c_str()));
		std::string style (ligneNode.getAttribute (LIGNE_STYLE_ATTR.c_str()));
		int materielId = atoi (ligneNode.getAttribute (LIGNE_MATERIEL_ATTR.c_str()));
		int tarifId = atoi (ligneNode.getAttribute (LIGNE_TARIF_ATTR.c_str()));
		std::string indicateur (ligneNode.getAttribute (LIGNE_INDICATEUR_ATTR.c_str()));

		std::string image (ligneNode.getAttribute (LIGNE_IMAGE_ATTR.c_str()));
		std::string libelleComplet (ligneNode.getAttribute (LIGNE_LIBELLE_COMPLET_ATTR.c_str()));
		int veloId = atoi (ligneNode.getAttribute (LIGNE_VELO_ATTR.c_str()));
		int handicapeId = atoi (ligneNode.getAttribute (LIGNE_HANDICAPE_ATTR.c_str()));

		bool affichageTableauDepart = atoi (ligneNode.getAttribute (LIGNE_AFFICHAGE_TABLEAU_DEPART_ATTR.c_str())) > 0;
		bool affichageIndicateur = atoi (ligneNode.getAttribute (LIGNE_AFFICHAGE_INDICATEUR_ATTR.c_str())) > 0;

		cLigne* ligne = new cLigne (cTexte (code.c_str ()), axe);
		env.Enregistre(ligne);
		
		ligne->setReseau(env.getReseau(reseau));
		ligne->setLibelleSimple(cTexte (cartouche.c_str()));
		ligne->setStyle (style.c_str());
		ligne->setImage(cTexte (image.c_str()));
		ligne->setLibelleComplet(cTexte (libelleComplet.c_str ()));
		
		ligne->setMateriel(env.GetMateriel(materielId));
		ligne->setResa(env.getResa (reservation));

		ligne->setVelo(env.getVelo(veloId));
		ligne->setHandicape(env.getHandicape(handicapeId));

		ligne->setTarif(env.getTarif(tarifId));
		
		//!	\todo A FAIRE Ajouter Alertes ?	une seul message par aret logique aujourd'hui
		
		ligne->setAAfficherSurTableauDeparts(affichageTableauDepart);
		ligne->setAAfficherSurIndicateurs(affichageIndicateur);

		ligne->setNomPourIndicateur(cTexte(indicateur));
		
		// Ajout des calendriers a la ligne - un seul tag calendriers pour tous
		// les calendriers un par arret logique
		XMLNode calendrierNode = ligneNode.getChildNode(CALENDRIERS_TAG.c_str(), 0);
		std::string valeurs (calendrierNode.getAttribute (CALENDRIERS_VALEURS_ATTR.c_str()));
		
		int nbCal = valeurs.size() / 6;
		ligne->NombreServices(nbCal);
		
		// 6 caracteres par id de calendrier... remplacer par un tokenizer
		for (int k=0; k<nbCal; ++k) {
			int calid = atoi (valeurs.substr (i*6, 6).c_str());
			ligne->setJC (k, env.GetJC (calid));
		}
		
		int attente = atoi (ligneNode.getAttribute (LIGNE_ATTENTE_ATTR.c_str()));
		for (int k=0; k < ligne->NombreServices(); ++k) {
			ligne->setAttente (k, cDureeEnMinutes(attente));
		}
		
		
//! \todo fin -- service continu

		// Ajout des numeros de service
		XMLNode numerosNode = node.getChildNode(NUMEROS_TAG.c_str(), 0);
		valeurs = ligneNode.getAttribute (NUMEROS_VALEURS_ATTR.c_str());

		for (int k=0; k < ligne->NombreServices(); ++k) {
			ligne->setNumeroService(k, cTexte (valeurs.substr (k*6, 6).c_str ()));
		}
		
		
		int nbArretLignes = ligneNode.nChildNode(ARRET_LIGNE_TAG.c_str());
		for (int k=0; nbArretLignes; ++k) {
			XMLNode arretLigneNode = ligneNode.getChildNode(ARRET_LIGNE_TAG.c_str(), i);
			int alId = atoi (arretLigneNode.getAttribute (ARRET_LIGNE_ID_ATTR.c_str()));
			int pm = atoi (arretLigneNode.getAttribute (ARRET_LIGNE_PM_ATTR.c_str()));
			int apId = atoi (arretLigneNode.getAttribute (ARRET_LIGNE_QUAI_ATTR.c_str()));
			std::string type (arretLigneNode.getAttribute (ARRET_LIGNE_TYPE_ATTR.c_str()));
			bool newHorairesSaisis = arretLigneNode.getAttribute (ARRET_LIGNE_HORAIRES_ATTR.c_str()) != NULL;
			

			tTypeGareLigneDA typeGL = Depart;
			if (type == ARRET_LIGNE_TYPE_DEPART_ATTR) {
				typeGL = Depart;
			} else if (type == ARRET_LIGNE_TYPE_PASSAGE_ATTR) {
				typeGL = Passage;
			} else if (type == ARRET_LIGNE_TYPE_ARRIVEE_ATTR) {
				typeGL = Arrivee;
			}
			
			
			cGareLigne* gareLigne =	ligne->addGareLigne(NULL, pm, apId, typeGL, 
				env.getArretLogique(alId), newHorairesSaisis, false);


			// Comprends rien !!!
			if (newHorairesSaisis) {
				std::string horaires (arretLigneNode.getAttribute (ARRET_LIGNE_HORAIRES_ATTR.c_str()));
				
			} else {
				
			}


			
		}
		
		
		
		
		
		
		
			
//! \todo import source ?  classe admin
//! \todo code base trains ?  classe admin
//! \todo depot ?  classe admin


		
		


		
		// ou ca va ?
	}
	
	
}



void 
cFichierXML::chargeInterface (XMLNode& node, SYNTHESE& synthese) {
	int id = atoi (node.getAttribute (INTERFACE_ID_ATTR.c_str()));
	log ("Chargement de l'interface ...");
	std::string nom (node.getAttribute (INTERFACE_NOM_ATTR.c_str()));
	
	cInterface* interface = new cInterface ();
	//!	\todo nom ?
	
	int nbEltInterface = node.nChildNode(ELEMENT_INTERFACE_TAG.c_str());
	
	for (int j=0; j<nbEltInterface; ++j) {
		XMLNode eltInterfaceNode = node.getChildNode(ELEMENT_INTERFACE_TAG.c_str(), j);
		std::string role (eltInterfaceNode.getAttribute (ELEMENT_INTERFACE_ROLE_ATTR.c_str()));
		std::string src (eltInterfaceNode.getAttribute (ELEMENT_INTERFACE_SRC_ATTR.c_str()));
		std::string constants;
		if (eltInterfaceNode.getAttribute (ELEMENT_INTERFACE_PARAMS_ATTR.c_str()) != NULL)
		{
			constants = eltInterfaceNode.getAttribute (ELEMENT_INTERFACE_PARAMS_ATTR.c_str());
		}
		
		cFichierObjetInterfaceStandard fObjInt (src.c_str(), _basePath+"/");
		fObjInt.Charge (interface->Element(roleToIndex (role)), *interface);
		
	}
	synthese.Enregistre(interface, id);
}






void 
cFichierXML::chargeSite (XMLNode& node, SYNTHESE& synthese) {
	log ("Chargement site...");
	
	std::string id (node.getAttribute (SITE_ID_ATTR.c_str()));
	int refEnv = atoi (node.getAttribute (SITE_ENVIRONNEMENT_ATTR.c_str()));
	int refInterface = atoi (node.getAttribute (SITE_INTERFACE_ATTR.c_str()));
	std::string designation (node.getAttribute (SITE_DESIGNATION_ATTR.c_str()));
	bool resaEnLigne = (atoi (node.getAttribute (SITE_RESA_EN_LIGNE_ATTR.c_str())) > 0);
	std::string urlClient (node.getAttribute (SITE_URL_CLIENT_ATTR.c_str()));
	bool solutionsPassees = (atoi (node.getAttribute (SITE_SOLUTIONS_PASSEES_ATTR.c_str())) > 0);
	
	cSite* site = new cSite (cTexte (id.c_str ()));
	
	
	site->SetEnvironnement(synthese.TableauEnvironnements()[refEnv]);
	site->SetInterface(synthese.GetInterface(refInterface));
	site->setIdentifiant(cTexte (designation.c_str()));

	
	const cTexte txtDateDebut (node.getAttribute (SITE_DATE_DEBUT_ATTR.c_str()));

	cDate dateDebut;
	//si date non valide, on ne permet pas le site
	if (txtDateDebut.Taille()<=0 || txtDateDebut[0]==' ')
		dateDebut.setDate(TEMPS_MAX);
	else
		dateDebut = txtDateDebut;

	site->setDateDebut(dateDebut);
	
	const cTexte txtDateFin (node.getAttribute (SITE_DATE_FIN_ATTR.c_str()));

	cDate dateFin;
	//si date non valide, on ne permet pas le site
	if (txtDateFin.Taille()<=0 || txtDateFin[0]==' ')
		dateFin.setDate(TEMPS_MAX);
	else
		dateFin = txtDateFin;

	site->setDateFin(dateFin);

	site->setResaEnLigne(resaEnLigne);
	site->setURLClient(cTexte (urlClient.c_str()));
	site->setSolutionsPassees(solutionsPassees);
	
	synthese.Enregistre(site);
	
}



void 
cFichierXML::chargeTableauDeparts (XMLNode& node, SYNTHESE& synthese) {
	log ("Chargement tableau departs...");

	std::string code (node.getAttribute (TABLEAU_CODE_ATTR.c_str()));
	std::string type (node.getAttribute (TABLEAU_TYPE_ATTR.c_str()));
	int refEnv = atoi (node.getAttribute (TABLEAU_ENVIRONNEMENT_ATTR.c_str()));
	int refInterface = atoi (node.getAttribute (TABLEAU_INTERFACE_ATTR.c_str()));
	std::string titre (node.getAttribute (TABLEAU_TITRE_ATTR.c_str()));
	bool originesSeulement = (atoi (node.getAttribute (TABLEAU_ORIGINES_SEULEMENT_ATTR.c_str())) > 0);
	int nbRangees = atoi (node.getAttribute (TABLEAU_NOMBRE_RANGEES_ATTR.c_str()));
	std::string tri (node.getAttribute (TABLEAU_TRI_ATTR.c_str()));
	
	cTableauAffichage* tableau = (tri == TABLEAU_TRI_ATTR_CHRONOLOGIQUE) ? 
		new cTableauAffichage (cTexte (code.c_str())) : 
		new cTableauAffichageSelectifDestinations (cTexte (code.c_str()));
		
	//!	\todo type ?
	tableau->SetEnvironnement(synthese.TableauEnvironnements()[refEnv]);
	tableau->SetInterface(synthese.GetInterface(refInterface));
	tableau->SetTitre(cTexte (titre.c_str()));
	tableau->SetOriginesSeulement(originesSeulement);
	tableau->SetNombreDeparts(nbRangees);
	
	const cEnvironnement* env = synthese.GetEnvironnement(refEnv);

	int nbRefArretLogique = node.nChildNode(REF_ARRET_LOGIQUE_TAG.c_str());
	for (int i=0; i<nbRefArretLogique; ++i) {
		XMLNode refALNode = node.getChildNode(REF_ARRET_LOGIQUE_TAG.c_str(), i);
		int alId = atoi (refALNode.getAttribute (REF_ARRET_LOGIQUE_ID_ATTR.c_str()));
		cArretLogique* al = env->getArretLogique(alId);

		int nbRefArretPhysique = refALNode.nChildNode(REF_ARRET_PHYSIQUE_TAG.c_str());
		for (int j=0; j<nbRefArretPhysique; ++j) {
			XMLNode refAPNode = refALNode.getChildNode(REF_ARRET_PHYSIQUE_TAG.c_str(), j);
			std::string apId = refAPNode.getAttribute (REF_ARRET_PHYSIQUE_ID_ATTR.c_str());
			if (apId == "*") {
				// Tous les arrets physiques de l'arret logique sont pris en compte
				// rien a faire.
				break;
			} else {
				tableau->SetArretLogique(al);
				tableau->AddArretPhysiqueAutorise(atoi (apId.c_str()));	
			}
		}
	}

	int nbDestinationsAffichees = node.nChildNode(DESTINATION_AFFICHEE_TAG.c_str());
	for (int i=0; i<nbDestinationsAffichees; ++i) {
		XMLNode refDestAffNode = node.getChildNode(DESTINATION_AFFICHEE_TAG.c_str(), i);
		int alId = atoi (refDestAffNode.getAttribute (DESTINATION_AFFICHEE_REF_ATTR.c_str()));
		tableau->AddDestinationAffichee(env->GetArretLogique(alId));
	}		
	
	
	if (tri == TABLEAU_TRI_ATTR_SELECTIF) {
		cTableauAffichageSelectifDestinations* tableauSelectif = 
			(cTableauAffichageSelectifDestinations*) tableau;
		
		int nbDestinationsSelection = node.nChildNode(DESTINATION_SELECTION_TAG.c_str());
		for (int i=0; i<nbDestinationsSelection; ++i) {
			XMLNode refDestSelNode = node.getChildNode(DESTINATION_SELECTION_TAG.c_str(), i);
			int alId = atoi (refDestSelNode.getAttribute (DESTINATION_SELECTION_REF_ATTR.c_str()));
			
			tableauSelectif->AddDestination(env->GetArretLogique(alId));
		}		
	}
	
	synthese.Enregistre(tableau);
	
}












int 
cFichierXML::roleToIndex (const std::string& role) {
	
	if (role == ELEMENT_INTERFACE_ROLE_ATTR_ENTETE) {
		return INTERFACEEnTete;
	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_PIED) {
		return INTERFACEPied;
	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FORMULAIRE_ENTREE) {
		return INTERFACEFormulaireEntree;
	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_NOM_ARRET) {
		return INTERFACENomArret;
	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_DESIGNATION_LIGNE) {
		return INTERFACEDesignationLigne;
	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_CARTOUCHE_LIGNE) {
		return INTERFACECartoucheLigne;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_DEPART) {
		return INTERFACEListeCommunesDepart;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_ARRIVEE) {
		return INTERFACEListeCommunesArrivee;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_DEPART) {
		return INTERFACEListeArretsDepart;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_ARRIVEE) {
		return INTERFACEListeArretsArrivee;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_ERREUR_ARRETS_FICHE_HORAIRE) {
		return INTERFACEErreurArretsFicheHoraire;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP_GARE) {
		return INTERFACETbDepGare;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP) {
		return INTERFACETbDep;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FORM_RESA) {
		return INTERFACEFormResa;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_VALID_RESA) {
		return INTERFACEValidResa;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_COLONNE) {
		return INTERFACEFicheHoraireColonne;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FEUILLE_ROUTE) {
		return INTERFACEFeuilleRoute;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_ARRET) {
		return INTERFACELigneFeuilleRouteArret;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_CIRCULATION) {
		return INTERFACELigneFeuilleRouteCirculation;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_JONCTION) {
		return INTERFACELigneFeuilleRouteJonction;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FICHE_ARRET) {
		return INTERFACEFicheArret;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_CASE_LIGNES) {
		return INTERFACECaseLignes;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_AUCUNE_SOLUTION_FICHE_HORAIRE) {
		return INTERFACEAucuneSolutionFicheHoraire;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_CASE_DUREE) {
		return INTERFACECaseDuree;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_CASE_RENVOI) {
		return INTERFACECaseRenvoi;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_LIGNE) {
		return INTERFACEFicheHoraireLigne;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_DUREE) {
		return INTERFACEDuree;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_MINI_TB_DEP_GARE) {
		return INTERFACEMiniTbDepGare;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_PAGE_ACCUEIL) {
		return INTERFACEPageAccueil;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE) {
		return INTERFACEFicheHoraire;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_TABLEAU_DEPART) {
		return INTERFACELigneTableauDepart;

	} else if (role == ELEMENT_INTERFACE_ROLE_ATTR_CASE_PARTICULARITE) {
		return INTERFACECaseParticularite;

	}
		
	return 0;
}

