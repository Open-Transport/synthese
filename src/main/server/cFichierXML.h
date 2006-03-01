#ifndef SYNTHESE_CFICHIERXML_H_
#define SYNTHESE_CFICHIERXML_H_


#include <iostream>

class cEnvironnement;
class XMLNode;
class SYNTHESE;

/** @defgroup m11 11 Lecture des fichiers de donn�es en XML
 @{
*/

/** Fichier XML en lecture
*/
class cFichierXML
{
    private:

        static const std::string SYNTHESE_TAG;
        static const std::string ENVIRONNEMENT_TAG;
        static const std::string ENVIRONNEMENT_ID_ATTR;
        static const std::string ENVIRONNEMENT_NOM_ATTR;
        static const std::string ENVIRONNEMENT_REP_ATTR;

        static const std::string DONNEES_TRANSPORT_TAG;

        static const std::string ARRET_LOGIQUE_TAG;
        static const std::string ARRET_LOGIQUE_ID_ATTR;
        static const std::string ARRET_LOGIQUE_TYPE_ATTR;

        static const std::string ARRET_LOGIQUE_TYPE_ATTR_CORR_AUTORISEE;
        static const std::string ARRET_LOGIQUE_TYPE_ATTR_CORR_INTERDITE;
        static const std::string ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE;
        static const std::string ARRET_LOGIQUE_TYPE_ATTR_CORR_RECOMMANDEE_COURT;

        static const std::string ARRET_LOGIQUE_X_ATTR;
        static const std::string ARRET_LOGIQUE_Y_ATTR;

        static const std::string ARRET_PHYSIQUE_TAG;
        static const std::string ARRET_PHYSIQUE_RANG_ATTR;
        static const std::string ARRET_PHYSIQUE_NOM_ATTR;
        static const std::string ARRET_PHYSIQUE_X_ATTR;
        static const std::string ARRET_PHYSIQUE_Y_ATTR;
        static const std::string ARRET_PHYSIQUE_LIEU_LOGIQUE_ATTR;

        static const std::string DELAI_TAG;
        static const std::string DELAI_ORIGINE_ATTR;
        static const std::string DELAI_DESTINATION_ATTR;
        static const std::string DELAI_VALEUR_ATTR;

        static const std::string DESIGNATION_TAG;
        static const std::string DESIGNATION_NOM_COMMUNE_ATTR;
        static const std::string DESIGNATION_TYPE_ATTR;

        static const std::string DESIGNATION_TYPE_ATTR_QUELCONQUE;
        static const std::string DESIGNATION_TYPE_ATTR_PRINCIPALE;
        static const std::string DESIGNATION_TYPE_ATTR_TOUT_LIEU;
        static const std::string DESIGNATION_TYPE_ATTR_LIEU_PUBLIC;
        static const std::string DESIGNATION_TYPE_ATTR_LIEU_QUELCONQUE;
        static const std::string DESIGNATION_TYPE_ATTR_PRINCIPALE_DEFAUT;

        static const std::string DESIGNATION_NOM_ARRET_ATTR;


        static const std::string MATERIEL_TAG;
        static const std::string MATERIEL_ID_ATTR;
        static const std::string MATERIEL_VITESSE_ATTR;

        static const std::string CALENDRIER_TAG;
        static const std::string CALENDRIER_ID_ATTR;
        static const std::string CALENDRIER_DEBUT_SERVICE_ATTR;
        static const std::string CALENDRIER_FIN_SERVICE_ATTR;

        static const std::string DATE_TAG;
        static const std::string DATE_DATE_MIN_ATTR;
        static const std::string DATE_DATE_MAX_ATTR;
        static const std::string DATE_PAS_ATTR;
        static const std::string DATE_SENS_ATTR;

        static const std::string CALENDRIER_INCLUS_TAG;
        static const std::string CALENDRIER_INCLUS_REF_ATTR;
        static const std::string CALENDRIER_INCLUS_SENS_ATTR;

        static const std::string MODALITE_RESA_TAG;
        static const std::string MODALITE_RESA_ID_ATTR;
        static const std::string MODALITE_RESA_NOM_ATTR;
        static const std::string MODALITE_RESA_TYPE_ATTR;

        static const std::string MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE;
        static const std::string MODALITE_RESA_TYPE_ATTR_FACULTATIVE;
        static const std::string MODALITE_RESA_TYPE_ATTR_IMPOSSIBLE;
        static const std::string MODALITE_RESA_TYPE_ATTR_OBLIGATOIRE_COLLECTIVEMENT;

        static const std::string MODALITE_RESA_MINUTES_MIN_ATTR;
        static const std::string MODALITE_RESA_EN_LIGNE_ATTR;
        static const std::string MODALITE_RESA_CHAMP_TELEPHONE_ATTR;
        static const std::string MODALITE_RESA_CHAMP_PRENOM_ATTR;

        static const std::string TARIF_TAG;
        static const std::string TARIF_ID_ATTR;
        static const std::string TARIF_TYPE_ATTR;
        static const std::string TARIF_TYPE_ATTR_ZONAGE;
        static const std::string TARIF_TYPE_ATTR_SECTION;
        static const std::string TARIF_TYPE_ATTR_DISTANCE;
        static const std::string TARIF_LIBELLE_ATTR;

        static const std::string HANDICAPE_TAG;
        static const std::string HANDICAPE_ID_ATTR;
        static const std::string HANDICAPE_TYPE_ATTR;
        static const std::string HANDICAPE_TYPE_ATTR_ACCEPTE;
        static const std::string HANDICAPE_TYPE_ATTR_INTERDIT;
        static const std::string HANDICAPE_TYPE_ATTR_INCONNU;
        static const std::string HANDICAPE_CONTENANCE_ATTR;
        static const std::string HANDICAPE_PRIX_ATTR;
        static const std::string HANDICAPE_RESA_ATTR;
        static const std::string HANDICAPE_DOC_ATTR;

        static const std::string VELO_TAG;
        static const std::string VELO_ID_ATTR;
        static const std::string VELO_TYPE_ATTR;
        static const std::string VELO_TYPE_ATTR_ACCEPTE;
        static const std::string VELO_TYPE_ATTR_INTERDIT;
        static const std::string VELO_TYPE_ATTR_INCONNU;
        static const std::string VELO_CONTENANCE_ATTR;
        static const std::string VELO_PRIX_ATTR;
        static const std::string VELO_RESA_ATTR;
        static const std::string VELO_DOC_ATTR;

        static const std::string PHOTO_TAG;
        static const std::string PHOTO_ID_ATTR;
        static const std::string PHOTO_DESIGNATION_GENERALE_ATTR;
        static const std::string PHOTO_DESIGNATION_LOCALE_ATTR;

        static const std::string LIEN_PHOTO_TAG;
        static const std::string LIEN_PHOTO_ZONE_CLICKABLE_ATTR;
        static const std::string LIEN_PHOTO_INDICATION_ATTR;
        static const std::string LIEN_PHOTO_PHOTO_CIBLE_ATTR;

        static const std::string AXE_TAG;
        static const std::string AXE_TYPE_ATTR;
        static const std::string AXE_TYPE_ATTR_STANDARD;
        static const std::string AXE_TYPE_ATTR_LIBRE;
        static const std::string AXE_TYPE_ATTR_INTERDIT;
        static const std::string AXE_CODE_ATTR;

        static const std::string INTERDICTION_CORRESPONDANCE_TAG;
        static const std::string INTERDICTION_CORRESPONDANCE_AXE_ATTR;

        static const std::string LIGNE_TAG;
        static const std::string LIGNE_CODE_ATTR;
        static const std::string LIGNE_RESEAU_ATTR;
        static const std::string LIGNE_RESERVATION_ATTR;
        static const std::string LIGNE_CARTOUCHE_ATTR;
        static const std::string LIGNE_STYLE_ATTR;
        static const std::string LIGNE_INDICATEUR_ATTR;
        static const std::string LIGNE_MATERIEL_ATTR;
        static const std::string LIGNE_TARIF_ATTR;
        static const std::string LIGNE_IMAGE_ATTR;
        static const std::string LIGNE_LIBELLE_COMPLET_ATTR;
        static const std::string LIGNE_VELO_ATTR;
        static const std::string LIGNE_HANDICAPE_ATTR;
        static const std::string LIGNE_AFFICHAGE_TABLEAU_DEPART_ATTR;
        static const std::string LIGNE_AFFICHAGE_INDICATEUR_ATTR;
        static const std::string LIGNE_ATTENTE_ATTR;


        static const std::string ARRET_LIGNE_TAG;
        static const std::string ARRET_LIGNE_PM_ATTR;
        static const std::string ARRET_LIGNE_ID_ATTR;
        static const std::string ARRET_LIGNE_QUAI_ATTR;
        static const std::string ARRET_LIGNE_TYPE_ATTR;
        static const std::string ARRET_LIGNE_TYPE_DEPART_ATTR;
        static const std::string ARRET_LIGNE_TYPE_PASSAGE_ATTR;
        static const std::string ARRET_LIGNE_TYPE_ARRIVEE_ATTR;
        static const std::string ARRET_LIGNE_HORAIRES_ATTR;

        static const std::string CALENDRIERS_TAG;
        static const std::string CALENDRIERS_VALEURS_ATTR;

        static const std::string NUMEROS_TAG;
        static const std::string NUMEROS_VALEURS_ATTR;

        static const std::string ARRET_TAG;
        static const std::string ARRET_PM_ATTR;
        static const std::string ARRET_ID_ATTR;
        static const std::string ARRET_QUAI_ATTR;
        static const std::string ARRET_TYPE_ATTR;
        static const std::string ARRET_DEPARTS_ATTR;
        static const std::string ARRET_PASSAGES_ATTR;
        static const std::string ARRET_ARRIVEES_ATTR;

        static const std::string POINT_TAG;
        static const std::string POINT_PM_ATTR;
        static const std::string POINT_X_ATTR;
        static const std::string POINT_Y_ATTR;

        static const std::string SITE_TAG;
        static const std::string SITE_ID_ATTR;
        static const std::string SITE_ENVIRONNEMENT_ATTR;
        static const std::string SITE_INTERFACE_ATTR;
        static const std::string SITE_DESIGNATION_ATTR;
        static const std::string SITE_DATE_DEBUT_ATTR;
        static const std::string SITE_DATE_FIN_ATTR;
        static const std::string SITE_URL_CLIENT_ATTR;
        static const std::string SITE_RESA_EN_LIGNE_ATTR;
        static const std::string SITE_SOLUTIONS_PASSEES_ATTR;

        static const std::string INTERFACE_TAG;
        static const std::string INTERFACE_ID_ATTR;
        static const std::string INTERFACE_NOM_ATTR;

        static const std::string ELEMENT_INTERFACE_TAG;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR;
        static const std::string ELEMENT_INTERFACE_PARAMS_ATTR;
        static const std::string ELEMENT_INTERFACE_SRC_ATTR;

        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_ENTETE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_PIED;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FORMULAIRE_ENTREE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_NOM_ARRET;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_DESIGNATION_LIGNE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_CARTOUCHE_LIGNE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_DEPART;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LISTE_COMMUNES_ARRIVEE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_DEPART;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LISTE_ARRETS_ARRIVEE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_ERREUR_ARRETS_FICHE_HORAIRE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_ATTENTE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP_GARE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_TB_DEP;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FORM_RESA;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_VALID_RESA;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_COLONNE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FEUILLE_ROUTE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_ARRET;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_CIRCULATION;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_FEUILLE_ROUTE_JONCTION;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FICHE_ARRET;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_CASE_LIGNES;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_AUCUNE_SOLUTION_FICHE_HORAIRE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_CASE_DUREE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_CASE_RENVOI;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE_LIGNE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_DUREE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_MINI_TB_DEP_GARE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_PAGE_ACCUEIL;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_FICHE_HORAIRE;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_LIGNE_TABLEAU_DEPART;
        static const std::string ELEMENT_INTERFACE_ROLE_ATTR_CASE_PARTICULARITE;


        static const std::string TABLEAU_TAG;
        static const std::string TABLEAU_CODE_ATTR;
        static const std::string TABLEAU_TYPE_ATTR;
        static const std::string TABLEAU_ENVIRONNEMENT_ATTR;
        static const std::string TABLEAU_INTERFACE_ATTR;
        static const std::string TABLEAU_TITRE_ATTR;
        static const std::string TABLEAU_ORIGINES_SEULEMENT_ATTR;
        static const std::string TABLEAU_NOMBRE_RANGEES_ATTR;
        static const std::string TABLEAU_TRI_ATTR;
        static const std::string TABLEAU_TRI_ATTR_CHRONOLOGIQUE;
        static const std::string TABLEAU_TRI_ATTR_SELECTIF;

        static const std::string REF_ARRET_LOGIQUE_TAG;
        static const std::string REF_ARRET_LOGIQUE_ID_ATTR;
        static const std::string REF_ARRET_PHYSIQUE_TAG;
        static const std::string REF_ARRET_PHYSIQUE_ID_ATTR;

        static const std::string DESTINATION_AFFICHEE_TAG;
        static const std::string DESTINATION_AFFICHEE_REF_ATTR;

        static const std::string DESTINATION_SELECTION_TAG;
        static const std::string DESTINATION_SELECTION_REF_ATTR;

        static const std::string RESEAU_TAG;
        static const std::string RESEAU_ID_ATTR;
        static const std::string RESEAU_NOM_ATTR;
        static const std::string RESEAU_URL_ATTR;
        static const std::string RESEAU_CARTOUCHE_HTML_ATTR;
        static const std::string RESEAU_DOC_HTML_ATTR;

        static const std::string INDICATEURS_TAG;
        static const std::string INDICATEURS_BASE_JC_ATTR;

        static const std::string cFichierXML::INDICATEUR_TAG;
        static const std::string INDICATEUR_TITRE_ATTR;
        static const std::string INDICATEUR_MASQUE_JC_ATTR;
        static const std::string INDICATEUR_COMMENCE_PAGE_ATTR;

        static const std::string INDICATEUR_ARRET_TAG;
        static const std::string INDICATEUR_ARRET_TYPE_ARRET_ATTR;
        static const std::string INDICATEUR_ARRET_TYPE_ARRET_ATTR_DEPART;
        static const std::string INDICATEUR_ARRET_TYPE_ARRET_ATTR_PASSAGE;
        static const std::string INDICATEUR_ARRET_TYPE_ARRET_ATTR_ARRIVEE;
        static const std::string INDICATEUR_ARRET_REF_ATTR;
        static const std::string INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR;
        static const std::string INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_OBLIGATOIRE;
        static const std::string INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_FACULTATIF;
        static const std::string INDICATEUR_ARRET_TYPE_INDICATEUR_ATTR_SUFFISANT;

        static const std::string COMMUNE_TAG;
        static const std::string COMMUNE_ID_ATTR;
        static const std::string COMMUNE_NOM_ATTR;
        static const std::string COMMUNE_CODE_INSEE_ATTR;
        static const std::string COMMUNE_DEPARTEMENT_ATTR;
        static const std::string COMMUNE_CODE_POSTAL_DEFAUT_ATTR;

        static const std::string ROUTE_TAG;
        static const std::string ROUTE_ID_ATTR;
        static const std::string ROUTE_NOM_ATTR;
        static const std::string ROUTE_TYPE_ATTR;

        static const std::string ROUTE_TYPE_AUTOROUTE;
        static const std::string ROUTE_TYPE_ATTR_ROUTE_CHAUSSEES_SEPAREES;
        static const std::string ROUTE_TYPE_ATTR_AXE_PRINCIPAL;
        static const std::string ROUTE_TYPE_ATTR_AXE_SECONDAIRE;
        static const std::string ROUTE_TYPE_ATTR_PONT;
        static const std::string ROUTE_TYPE_ATTR_RUE;
        static const std::string ROUTE_TYPE_ATTR_RUE_PIETONNE;
        static const std::string ROUTE_TYPE_ATTR_BRETELLE;
        static const std::string ROUTE_TYPE_ATTR_VOIE_PRIVEE;
        static const std::string ROUTE_TYPE_ATTR_CHEMIN;
        static const std::string ROUTE_TYPE_ATTR_TUNNEL;
        static const std::string ROUTE_TYPE_ATTR_ROUTE_SECONDAIRE;

        static const std::string ROUTE_DISCRIMINANT_ATTR;

        static const std::string SEGMENT_ROUTE_TAG;
        static const std::string SEGMENT_ROUTE_ID_ATTR;
        static const std::string SEGMENT_ROUTE_NO_DEBUT_DROITE_ATTR;
        static const std::string SEGMENT_ROUTE_NO_FIN_DROITE_ATTR;
        static const std::string SEGMENT_ROUTE_NO_DEBUT_GAUCHE_ATTR;
        static const std::string SEGMENT_ROUTE_NO_FIN_GAUCHE_ATTR;


        const std::string _basePath;

    private:

        int roleToIndex ( const std::string& role );

    public:
        //! \name Fonctions de chargement
        //@{
        void log ( const std::string& message );

        // SYNTHESE* chargeSynthese (std::istream& xmlStream);
        SYNTHESE* chargeSynthese ( const std::string& filename );

        void chargeDonneesTransport ( std::istream& xmlStream, cEnvironnement& env );
        void chargeDonneesRoutes ( const std::string& repertoire, cEnvironnement& env );
        void chargeDonneesRoutesCommune ( std::istream& xmlStream, cEnvironnement& env );

        void chargeDonneesTransport ( XMLNode& node, cEnvironnement& env );

        void chargeArretLogique ( XMLNode& node, cEnvironnement& env );
        void chargeAxe ( XMLNode& node, cEnvironnement& env );
        void chargeMateriel ( XMLNode& node, cEnvironnement& env );
        void chargeCalendrier ( XMLNode& node, cEnvironnement& env );
        void chargeModaliteResa ( XMLNode& node, cEnvironnement& env );
        void chargeTarif ( XMLNode& node, cEnvironnement& env );
        void chargeHandicape ( XMLNode& node, cEnvironnement& env );
        void chargeVelo ( XMLNode& node, cEnvironnement& env );
        void chargePhoto ( XMLNode& node, cEnvironnement& env );
        void chargeReseau ( XMLNode& node, cEnvironnement& env );
        void chargeIndicateurs ( XMLNode& node, cEnvironnement& env );


        void chargeInterface ( XMLNode& node, SYNTHESE& synthese );
        void chargeSite ( XMLNode& node, SYNTHESE& synthese );
        void chargeTableauDeparts ( XMLNode& node, SYNTHESE& synthese );



        //@}

        //! \name Constructeur et destructeur
        //@{
        cFichierXML( const std::string& basePath );
        ~cFichierXML();
        //@}
};

/** @} */



#endif /*SYNTHESE_CFICHIERXML_H_*/
