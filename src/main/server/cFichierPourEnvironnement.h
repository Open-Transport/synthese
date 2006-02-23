/**	En-t�te et impl�mentation inline classes cFichier d�crivant des objets enregistr�s dans les environnements.
	@file cFichierPourEnvironnement.h


Point de trac�
@code
<point x="<X>" y="<Y>" rang="<rang>" datemin="(date_min)" datemax="(date_max") />
@endcode
@param X Longitude du point en projection Lambert II Etendu	
@param Y Latitude du point en projection Lambert II Etendu	
@param date_min	Date de d�but de validit� du trac�	
@param date_max	Date de fin de validit� du trac�	
@param rang Rang du point au sein de son objet p�re (ex : rang d'arr�t dans la ligne)

Ligne
@code
<ligne code="<ID>" format="<format>" couleur="<couleur>"
       datemin="<date_min>" datemax="<date_max>">
	<arretphysique /> <point /> <point /> ... <point />
	<arretphysique /> <point /> <point /> ... <point />
	...
	<arretphysique /> <point /> <point /> ... <point />
	<arretphysique />
</ligne>
@endcode
@param ID Code de la ligne (R�seau/Ligne commerciale/Itin�raire)	
@param couleur	Couleur par d�faut de la ligne (RGB HTML)	
@param datemin	Date de d�but de validit� du trac�	
@param datemax	Date de fin de validit� du trac�	
@param nom	Nom de la ligne	
@param <point>, <arretphysique> : Succession de points, d'arr�ts de franchissements de limites communales du trac�, dans l'ordre
@warning Le chagement d'une ligne doit se faire apr�s chargement des arr�ts physiques


Arr�ts physiques
@code
<arretphysique code="(ID)" nom="(nom)">
	<point /> <point /> ... <point />
</arretphysique>
@endcode
@param ID Code de l'arr�t physique	
@param nom	Nom de l'arr�t physique au sein de l'arr�t logique	

A METTRE DANS SYNTHESE :
Dur�es de correspondance
<correspondance origine="(ID_origine)" destination="(ID_destination) duree="(duree)" />

D�signation d'arr�t
@code
<designation commune="(commune)" nom="(nom)" type="(type)" />
@endcode
@param commune ID ou nom de commune
@param nom Nom de l'arr�t dans la commune
@param type Type de d�signation :
	- @c main Arr�t principal de la commune
	- @c normal (d�faut) Arr�t de commune
	- @c site Lieu public
	- @c all Arr�t tous lieux


Fonds de carte
@code
<fond type="<type>" xmin="<xmin>" ymin="<ymin>" xmax="<xmax>" ymax="<ymax>" 
      echelle="<echelle>" src="<chemin>" />
@endcode
@param type	Type du fond (plan, carte, photo)	
@param xmin	Longitude du coin inf�rieur gauche de la carte en projection Lambert II Etendu	
@param ymin	Latitude du coin inf�rieur gauche de la carte en projection Lambert II Etendu	
@param xmax	Longitude du coin sup�rieur droit de la carte en projection Lambert II Etendu	
@param ymax	Latitude du coin sup�rieur droit de la carte en projection Lambert II Etendu	
@param echelle	Echelle en pixel par m�tre	
@param chemin Chemin d'acc�s au fichier	


*/

#ifndef SYNTHESE_CFICHIERPOURENVIRONNEMENT_H
#define SYNTHESE_CFICHIERPOURENVIRONNEMENT_H

/** @addtogroup m11
	@{ */

//!	@name Balise commune
//@{

/** D�claration de commune.
	@code
	<commune id="(ID)" nom="(nom)">
		<point /> <point /> ... <point />
	</commune>
	@endcode
	@param ID Code de la commune
	@param nom Nom de la commune
	@param <point> Suite de @ref XML_ENV_POINT "points" formant le contour de la commune
*/
#define XML_ENV_COMMUNE "commune"
#define XML_ENV_COMMUNE_ID "id"
#define XML_ENV_COMMUNE_NOM "nom"
#define XML_ENV_COMMUNE_POINT "point"
//@}


//!	@name Balise arr�t logique
//@{

/** D�claration d'arr�t logique.
	@code
	<arret_logique id="(ID)" type="(type)>
		<arret_physique /> <arret_physique /> ... <arret_physique />
		<designation /> <designation /> ... <designation />
		<alerte /> <alerte /> ... <alerte />
		<delai /><delai /> ... <delai />
	</arret_logique>
	@endcode
	@param ID Code de l'arr�t logique (voir @ref base)
	@param type Type d'arr�t logique, pouvant avoir les valeurs suivantes :
		- @c passage : arr�t de passage (correspondance interdite)
		- @c correspondance : arr�t de correspondance (correspondance autoris�e)
		- @c echange_courte_distance : arr�t de correspondance recommand� en courte distance uniquement
		- @c echange_longue_distance : arr�t de correspondance recommante pour toutes distances
	@param <arret_physique> Arr�ts physiques appartenant � l'arr�t logique
	@param <designation> @ref XML_ENV_ARRET_DESIGNATION "d�signation" de l'arr�t logique. La premi�re est la d�signation officielle.
	@param <alerte> @ref XML_ENV_ALERTE "alerte" concernant l'arr�t logique.

	D�tail de la balise <arret_physique> :
	@copydoc XML_ENV_ARRET_PHYSIQUE
*/
#define XML_ENV_ARRET_LOGIQUE "arret_logique"
#define XML_ENV_ARRET_LOGIQUE_ID "id"
#define XML_ENV_ARRET_LOGIQUE_TYPE "type"
//@}


//!	@name Balise arr�t physique
//@{

/** D�claration d'un arr�t physique.
	@code
	<arret_physique rang="(rang)" x="(x)" y="(y)"/>
	@endcode
*/
#define XML_ENV_ARRET_PHYSIQUE "arret_physique"
#define XML_ENV_ARRET_PHYSIQUE_RANG "rang"
#define XML_ENV_ARRET_PHYSIQUE_X "x"
#define XML_ENV_ARRET_PHYSIQUE_Y "y"
//@}





//! @name Fichiers d'horaires
//@{
#define HORAIRESFORMAT "horaires"
#define HORAIRESEXTENSION "horaires.per"

/** Appel de fichier inclus
	@code > nom_fichier @endcode
	@param nom_fichier chemin du fichier inclus, relativement au r�pertoire du fichier courant, ne portant pas l�extension .horaires.per (Ex&nbsp;: &gt; MP/index envoie vers <i>&lt;r�pertoire courant&gt;</i>/MP/index.horaires.per)
*/
#define HORAIRESFORMATLIGNELien						TYPELien


/** D�claration d'axe
	@code #type code @endcode
	@param type Type d'\ref defAxe :
		- # : \ref defAxeLibre
		- � : \ref defAxeInterdit
		- (rien) : Axe classique
	@param code Code unique d'axe (@ref baseHoraires)
*/
#define HORAIRESFORMATLIGNEAxe						TYPESection


/** D�claration de ligne
	@code [code_ligne @endcode
	@param code_ligne Code unique de la @ref defLigne d�clar�e (voir @ref baseHoraires )
	@warning Toute ligne doit �tre contenue dans un axe d�clar� auparavant.
*/
#define HORAIRESFORMATLIGNELigne					TYPESousSection

#define HORAIRESFORMATLIGNEDocumentation			0
#define HORAIRESFORMATLIGNEJoursCirculationLigne		1
#define HORAIRESFORMATLIGNEReseau					2


/** Libell� simple de ligne
	@code CAR libell� @endcode
	@param libell� Libell� simple de la ligne
*/
#define HORAIRESFORMATLIGNECartouche				3


/** Style de la ligne
	@code STY code_style @endcode
	@param code_style nom de style CSS r�f�rence aux d�finitions fournies dans le fichier <b>couleurs.css</b>.
*/
#define HORAIRESFORMATLIGNEStyle					4
#define HORAIRESFORMATLIGNEMateriel				5
#define HORAIRESFORMATLIGNEResa					6
#define HORAIRESFORMATLIGNEVelo					7
#define HORAIRESFORMATLIGNETarification				8
#define HORAIRESFORMATLIGNEJoursCirculationServices	9

/** D�claration de @ref defServiceContinu : attente aux arr�ts
	@code ATT attente1 @endcode
	@param attente Attente � l'arr�t en minutes.
	@warning Un service continu doit �tre le seul service de la @ref defLigne.
	Le fait de d�clarer une attente � un service le proclame au rang de service continu.
*/
#define HORAIRESFORMATLIGNEAttente					10
#define HORAIRESFORMATLIGNEFin					11


/** Codes des services
	@code NUM num1 num2 ... @endcode
	@param num1 Code du premier service
	@param num2 Code du second service
	@param ... etc
	@warning La num�rotation des services est facultative, sauf dans le cas de lignes offrant la r�servation en ligne.
*/
#define HORAIRESFORMATLIGNENumero					12

#define HORAIRESFORMATLIGNEHoraires				13
#define HORAIRESFORMATLIGNEAAfficher				14
#define HORAIRESFORMATLIGNECodeIndicateur			15
#define HORAIRESFORMATLIGNECodeImportationSource		16
#define HORAIRESFORMATLIGNECodeBaseTrains			17


/** (facultatif) Libell� complet de ligne
	@code DES libell� @endcode
	@param libell� Libell� complet de la ligne
	Si ce param�tre n'est pas fourni, alors le libell� complet g�n�r� par l'interface sera appliqu�.
*/
#define HORAIRESFORMATLIGNELibelleComplet			18


/** Chemin de l�image repr�sentant la ligne 
	@code IMG nom_fichier @endcode
	@param nom_fichier nom du fichier image sans extension et sans r�pertoire (ces informations sont produites par l'interface)
	@warning Au moins un des deux libell�s @ref HORAIRESFORMATLIGNECartouche ou @ref HORAIRESFORMATLIGNEImage doit �tre fourni.
*/
#define HORAIRESFORMATLIGNEImage					19
#define HORAIRESFORMATLIGNEAffIndicateurs			20
#define HORAIRESFORMATLIGNEDepot					21
#define HORAIRESFORMATLIGNEHandicape				22
#define HORAIRESFORMATLIGNEAlerte					23
#define HORAIRESFORMATLIGNEAlerteDebut				24
#define HORAIRESFORMATLIGNEAlerteFin				25
#define HORAIRESFORMATLIGNEGirouette				26
#define HORAIRESFORMATLIGNENombreFormats			27

#define HORAIRESFORMATCOLONNEPointHectometrique		0
#define HORAIRESFORMATCOLONNEArretLogique				1
#define HORAIRESFORMATCOLONNETypeArretLogique			2
#define HORAIRESFORMATCOLONNEArretPhysique					3
#define HORAIRESFORMATCOLONNEHoraire				4
#define HORAIRESFORMATCOLONNEDateAlerte				5
#define HORAIRESFORMATCOLONNEAlerte					6
#define HORAIRESFORMATCOLONNENombreFormats			7
//@}

//! @name Fichiers de points d'arr�t
//@{
#define PAFORMAT "pointsarret"
#define PAEXTENSION "gares.per"

/** D�claration de point d'arr�t
	@code [TypeCode @endcode
	@param Type Type du point d'arr�t, pouvant avoir les valeurs suivantes :
		- @c S : arr�t de passage (correspondance interdite)
		- @c C : arr�t de correspondance (correspondance autoris�e)
		- @c Q : arr�t de correspondance recommand� en courte distance uniquement
		- @c R : arr�t de correspondance recommante pour toutes distances
	@param Code Identifiant num�rique du point d�arr�t dans la base (voir @ref base)
	@warning Le code du point d�arr�t doit �tre strictement compris entre 0 et le num�ro max de point d�arr�t pr�c�demment d�fini.
*/
#define PAFORMATLIGNEArretLogique						TYPESousSection


/** Allocation des quais
	@code NQ nombre @endcode
	@param nombre Nombre de quais d�clar�s par la suite.
	@warning Le nombre de quais doit �tre strictement �gal au nombre de quais d�clar�.
	@todo Supprimer la n�cessit� d'allouer les quais � l'avance
*/
#define PAFORMATLIGNENombreArretPhysiques					0
#define PAFORMATLIGNENombreDescriptions				1

/** Longitude moyenne du point d'arr�t
	@code GX longitude @endcode
	@param longitude Longitude exprim�e en m�tres, selon la projection <b>Lambert Zone II Etendu</b>. Ordres de grandeur :
		- Bayonne : 292075
		- Toulouse : 525861
		- Nice : 995737
*/
#define PAFORMATLIGNEGpsx						2


/** Latitude moyenne du point d'arr�t
	@code GY latitude @endcode
	@param latitude Latitude exprim�e en m�tres, selon la projection <b>Lambert Zone II Etendu</b>. Ordres de grandeur :
		- Bayonne : 1839352
		- Toulouse : 1845706
		- Lille : 2626673
*/
#define PAFORMATLIGNEGpsy						3


/** D�signation de point d'arr�t
	@code DE TypeCommune(Nom @endcode
	@param Type Type de la d�signation par rapport � la commune, l'une des valeurs suivantes :
		- @c 0 : D�signation classique
		- @c 1 : Arr�t principal de commune
		- @c 2 : Arr�t tout lieu
	@param Commune : Nom complet de la commune (faisant office d�identificateur)
	@param Nom : libell� du point d�arr�t au sein de la commune
*/
#define PAFORMATLIGNEDescription					4
#define PAFORMATLIGNEHtml						5


/** D�claration de quai
	Arr�ts de correspondance :
	@code num�ro_quai:dur�e_correspondance1 dur�e_correspondance2 ... description @endcode
	Arr�ts de passage :
	@code num�ro_quai:description @endcode
	@param num�ro_quai Num�ro de quai au sein du point d'arr�t (deux chiffres)
	@param dur�e_correspondance1 Dur�e en minutes vers le quai 1 (deux chiffres)
	@param dur�e_correspondance2 Dur�e en minutes vers le quai 2 (deux chiffres)
	@param ... etc (autant que de quais dans le point d'arr�t)
	@param description D�signation du quai et commandes de description. Les commandes sont pr�c�d�es du signe @c # et requi�rent un param�tre qui suit la commande sans espace :
		- @c GX @ref defGeolocalisation "Longitude" du quai en Lambert II Etendu (8 chiffres)
		- @c GY @ref defGeolocalisation "Latitude" du quai en Lambert II Etendu (8 chiffres)
		- @c PH Photo reli�e au quai (code 8 chiffres relatif au fichier des photos)
*/
#define PAFORMATLIGNEArretPhysique						6
#define PAFORMATLIGNEPostScriptOD					7


/** Photo de point d'arr�t
	@code PH code @endcode
	@param code Code de photo r�f�rence aux fichier des photos
*/
#define PAFORMATLIGNEPhoto						8


/** Allocation des photos de point d'arr�t
	@code NP nombre @endcode
	@param nombre Nombre de photos dans le point d'arr�t
	@todo Supprimer l'allocation des photos (tableau dynamique)
*/
#define PAFORMATLIGNENombrePhotos					9


/** Vitesses maximales pour optimisation de calculs
	@code VM v5v10v15v20v25v30v50v100v150v200vmax @endcode
	@param v5 Vitesse maximale vers tout point situ� � 5 km au plus (en km/h sur 3 chiffres)
	@param v10 Vitesse maximale vers tout point situ� entre 5 km et 10 km (en km/h sur 3 chiffres)
	@param v15 Vitesse maximale vers tout point situ� entre 10 km et 15 km (en km/h sur 3 chiffres)
	@param v20 Vitesse maximale vers tout point situ� entre 15 km et 20 km (en km/h sur 3 chiffres)
	@param v25 Vitesse maximale vers tout point situ� entre 20 km et 25 km (en km/h sur 3 chiffres)
	@param v30 Vitesse maximale vers tout point situ� entre 25 km et 30 km (en km/h sur 3 chiffres)
	@param v50 Vitesse maximale vers tout point situ� entre 30 km et 50 km (en km/h sur 3 chiffres)
	@param v100 Vitesse maximale vers tout point situ� entre 50 km et 100 km (en km/h sur 3 chiffres)
	@param v150 Vitesse maximale vers tout point situ� entre 100 km et 150 km (en km/h sur 3 chiffres)
	@param v200 Vitesse maximale vers tout point situ� entre 150 km et 200 km (en km/h sur 3 chiffres)
	@param vmax Vitesse maximale vers tout point situ� � plus de 200 km (en km/h sur 3 chiffres)
	@warning des vitesses maximales fausses peuvent entra�ner des r�sultats faux o� m�me l'arr�t intempestif des calculs, bien mettre � jour les Vmax apr�s chaque modification de la base d�horaires.
	Les seuils de distance sont d�finis en tant que constantes et correspondent � des segments de march� relevant de moyens de transport diff�rents (urbain, r�gional routier, r�gional ferroviaire, national)
	
	Ces donn�es ne sont pas utilis�es � l'heure actuelle
*/
#define PAFORMATLIGNEVMax						10


/** Allocation des services en gare
	@code NS nombre @endcode
	@param nombre Nombre de services
	@todo Supprimer l'allocation des services (tableau dynamique)
*/
#define PAFORMATLIGNENombreServices				15


/** D�claration de service en gare
	@code SE type photo d�signation @endcode
	@param type Type de service, valeurs suivantes :
		- @c I : Guichet d'information
		- @c V : Guichet de vente
		- @c A : Automate de vente
		- @c W : Guichet d'information vente
		- @c B : Bar
		- @c R : Restaurant
		- @c T : Tabac/Presse
		- @c P : Boulangerie
		- @c D : Distributeur de billets de banque
	@param photo Code de photo du service, r�f�rence au fichier des photos (6 chiffres, 000000 = pas de photo)
	@param d�signation D�signation du service
*/
#define PAFORMATLIGNEService						16
#define PAFORMATLIGNEAlerte						17
#define PAFORMATLIGNEAlerteDebut					18
#define PAFORMATLIGNEAlerteFin					19


/** D�signation du point d'arr�t sur 13 caract�res.
	@code D13 designation @endcode
	@param designation Valeur
*/
#define PAFORMATLIGNEDesignation13					20


/** D�signation du point d'arr�t sur 26 caract�res.
	@code D26 designation @endcode
	@param designation Valeur
*/
#define PAFORMATLIGNEDesignation26					21

#define PAFORMATLIGNENombreFormats					22

#define PAFORMATCOLONNEStandard					0
#define PAFORMATCOLONNEVMax						1
#define PAFORMATCOLONNEServiceType					2
#define PAFORMATCOLONNEServicePhoto				3
#define PAFORMATCOLONNEServiceDesignation			4
#define PAFORMATCOLONNEDesignationLibelle			5
#define PAFORMATCOLONNEDesignationType				6
#define PAFORMATCOLONNEDateAlerte					7
#define PAFORMATCOLONNEAlerte						8
#define PAFORMATCOLONNEIndexArretPhysique					9
#define PAFORMATCOLONNEDelaiCorrespondance			10
#define PAFORMATCOLONNENombreFormats				11
/** @} */

//! \name Format Photos
//@{
#define PHFORMAT "photos"
#define PHEXTENSION "photos.per"

#define PHFORMATLIGNEDesignationGenerale			0
#define PHFORMATLIGNEDesignationLocale				1
#define PHFORMATLIGNEZoneCliquableURL				2
#define PHFORMATLIGNEZoneCliquablePhoto				3
#define PHFORMATLIGNENombreMaps					4
#define PHFORMATLIGNENomFichier					5
#define PHFORMATLIGNENombreFormats					6

#define PHFORMATCOLONNECoordonneesMap				0
#define PHFORMATCOLONNENumeroPhotoLiee				1
#define PHFORMATCOLONNEAltMapPhoto					2
#define PHFORMATCOLONNEAltUrlMapUrl				3
#define PHFORMATCOLONNEStandard					4
#define PHFORMATCOLONNENombreFormats				5
//@}

//! @name Format de fichier des calendriers
//@{
#define JCFORMAT "jours"
#define JCFILEEXTENSION "jours.per"
#define JCEXTENSION ".jours.per"


/** Bornes des calendriers de circulations (obligatoire en premier)
	@code Service=annee_debut/annee_fin @endcode
	@param annee_debut Premi�re ann�e des calendriers sur 4 chiffres obligatoirement
	@param annee_fin Derni�re ann�e des calendriers sur 4 chiffres obligatoirement
	@warning La derni�re ann�e doit �tre sup�rieure ou �gale � la premi�re
	Cette information obligatoire permet l�allocation m�moire&nbsp; des tableaux de binaires contenant les calendriers.
*/
#define JCFORMATLIGNEService	"Service="


/** D�claration de calendrier
	@code [codeLibell� @endcode
	@param code Code du calendrier sur 4 chiffres (voir @ref baseHoraires)
	@param Libell� descriptif du calendrier de longueur quelconque (facultatif)
	@todo Porter le nombre de calendriers possibles � \f$ 10^7 \f$.
*/
#define JCFORMATLIGNECalendrier	TYPESousSection


/** @ref defCategorieCalendrier
	@code =valeur @endcode
	@param valeur @ref defCategorieCalendrier

	L�appel � cette commande est facultatif, la cat�gorie 255 est choisie par d�faut.

*/
#define JCFORMATLIGNECategorie '='


/** Ajout de jour(s) circul�(s) � un calendrier
	@code +date_debut/date_fin/periode @endcode
	@param date_debut Premi�re date ajout�e
	@param date_fin Derni�re date ajout�e (facultatif : d�faut = premi�re date)
	@param periode Nombre de jours entre chaque date ajout�e (facultatif : d�faut = 0)

	Le format d'�criture des dates est en 8 caract�res obligatoirement :
		- soit une date explicite : format AAAAMMJJ
		- soit Date min pour le premier jour possible
		- soit Date max pour le dernier jour possible
		- soit une date g�n�rique : format ****MMJJ

	Dans le cadre d�une commande de type plage de dates g�n�riques, si la date sup�rieure est inf�rieure � la date inf�rieure, alors la date sup�rieure est consid�r�e comme �tant de l�ann�e suivante.

	Exemples pour un environnement 2001/2003 :
		- @c +20020105 donne l'ajout du 5/1/2002
		- @c +20031215/20040501 donne l'ajout du 15/12/2003 au 31/12/2003
		- @c +20031017/20031028/05 donne l'ajout de 17/10/2003, 22/10/2003, 27/10/2003
		- @c +****0105 donne l'ajout de 5/1/2001, 5/1/2002, 5/1/2003
		- @c +****1230/****0101 donne l'ajout de 30/12/2001, 31/12/2001, 1/1/2002, 30/12/2002, 31/12/2002, 1/1/2003, 30/12/2003, 31/12/2003.
		- @c +****1225/****0108/07 donne l'ajout de 25/12/2001, 1/1/2002, 8/1/2002, 25/12/2002, 1/1/2003, 8/1/2003, 25/12/2003

	@warning Il est interdit de coupler date g�n�rique et date absolue dans une m�me commande. (Erreur 06007)
*/
#define JCFORMATLIGNEAjoutDate '+'


/** Suppression de jour(s) circul�(s) � un calendrier
	@code -date_debut/date_fin/periode @endcode
	@param date_debut Premi�re date ajout�e
	@param date_fin Derni�re date ajout�e (facultatif : d�faut = premi�re date)
	@param periode Nombre de jours entre chaque date ajout�e (facultatif : d�faut = 0)

	Le format d'�criture des dates est en 8 caract�res obligatoirement :
		- soit une date explicite : format AAAAMMJJ
		- soit Date min pour le premier jour possible
		- soit Date max pour le dernier jour possible
		- soit une date g�n�rique : format ****MMJJ

	Dans le cadre d�une commande de type plage de dates g�n�riques, si la date sup�rieure est inf�rieure � la date inf�rieure, alors la date sup�rieure est consid�r�e comme �tant de l�ann�e suivante.

	Exemples pour un environnement 2001/2003 :
		- @c -20020105 donne la suppression du 5/1/2002
		- @c -20031215/20040501 donne la suppression du 15/12/2003 au 31/12/2003
		- @c -20031017/20031028/05 donne la suppression de 17/10/2003, 22/10/2003, 27/10/2003
		- @c -****0105 donne la suppression de 5/1/2001, 5/1/2002, 5/1/2003
		- @c -****1230/****0101 donne la suppression de 30/12/2001, 31/12/2001, 1/1/2002, 30/12/2002, 31/12/2002, 1/1/2003, 30/12/2003, 31/12/2003.
		- @c -****1225/****0108/07 donne la suppression de 25/12/2001, 1/1/2002, 8/1/2002, 25/12/2002, 1/1/2003, 8/1/2003, 25/12/2003

	@warning Il est interdit de coupler date g�n�rique et date absolue dans une m�me commande. (Erreur 06007)
*/
#define JCFORMATLIGNESuppressionDate '-'


/** Ajout � un calendrier des dates de circulation d'un autre calendrier d�j� d�clar�
	@code +#code @endcode
	@param code Code du calendrier � lire
    
	@warning L�ajout ne met pas � NON les bits 0 du calendrier inclus. Il ne fait que mettre � OUI les bits 1 du calendrier inclus. 
	@warning Le code de calendrier inclus doit correspondre � un calendrier d�j� d�clar�.

	Table de v�rit� de la commande :
	<table class="tableau">
	<tr><td></td><td colspan="2">Calendrier inclus</td></tr>
	<tr><td>Avant</td><td>0</td><td>1</td></td>
	<tr><td>0</td><td>0</td><td>1</td></tr>
	<tr><td>1</td><td>1</td><td>1</td></tr>
	</table>
*/
#define JCFORMATLIGNEAjoutCalendrier '#'


/** Suppression d'un calendrier des dates de circulation d'un autre calendrier d�j� d�clar�
	@code -#code @endcode
	@param code Code du calendrier � lire
    
	@warning La suppression ne met pas � OUI les bits 0 du calendrier inclus. Il ne fait que mettre � NON les bits 1 du calendrier inclus. 
	@warning Le code de calendrier inclus doit correspondre � un calendrier d�j� d�clar�.

	Table de v�rit� de la commande :
	<table class="tableau">
	<tr><td></td><td colspan="2">Calendrier inclus</td></tr>
	<tr><td>Avant</td><td>0</td><td>1</td></td>
	<tr><td>0</td><td>0</td><td>0</td></tr>
	<tr><td>1</td><td>1</td><td>0</td></tr>
	</table>
*/
#define JCFORMATLIGNESuppressionCalendrier '#'


/** Ex�cution de commandes d'ajout/suppression de dates d�crites dans un fichier
	@code +>fichier @endcode
	@param fichier Chemin du fichier � lire relativement � l'emplacement du fichier courant, sans l'extension .jours.per
    
	@warning Le fichier lu ne doit contenir que des commandes d'ajout/suppression de dates (@ref JCFORMATLIGNEAjoutDate et @ref JCFORMATLIGNESuppressionDate).
*/
#define JCFORMATLIGNEAjoutFichier '>'


/** Ex�cution invers�e de commandes d'ajout/suppression de dates d�crites dans un fichier
	@code ->fichier @endcode
	@param fichier Chemin du fichier � lire relativement � l'emplacement du fichier courant, sans l'extension .jours.per
    @warning Le fichier lu ne doit contenir que des commandes d'ajout/suppression de dates (@ref JCFORMATLIGNEAjoutDate et @ref JCFORMATLIGNESuppressionDate).
	Cette commande ex�cute les commandes inverses de celles contenues dans le fichier (+ devient - et inversement)
*/
#define JCFORMATLIGNESuppressionFichier '>'
//@}


//! @name Format du fichier des r�sas
//@{
#define RESASFORMAT		"resa"
#define RESASEXTENSION	"resa.per"


/** (obligatoire) Nombre de modalit�s de r�servation
	@code Nombre=num�ro_max_code_resa @endcode
	@param num�ro_max_code_resa Plus grand code de r�sa. 
		NB : ce num�ro peut �tre sup�rieur au nombre r�el de r�sas d�clar�es, il n�est en effet pas obligatoire de remplir tous les codes de r�sas.
	@warning Attention � bien modifier le num�ro max de modalit�s de r�servation lors d�un ajout.
	@todo Supprimer le nombre de modalit�s de r�servation (tableau dynamique)

	Ce nombre est n�cessaire pour permettre l�allocation m�moire lors du chargement.
*/
#define RESASFORMATLIGNENombre						"Nombre="


/** D�claration de modalit� de r�servation
	@code [code @endcode
	@param code Code de modalit� de r�servation (voir @ref baseHoraires)
	@warning Le code de la modalit� de r�servation doit �tre strictement compris entre 0 et le num�ro max de modalit� de r�servation d�fini par @ref RESASFORMATLIGNENombre.
*/
#define RESASFORMATLIGNEReservation					TYPESousSection



/** Caract�re obligatoire de la r�servation
	@code TYP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- N : Impossible (le service n'est pas ouvert � la r�servation)
		- F : Facultative (le service peut �tre utilis� sans r�servation, mais la r�servation est ouverte pour garantir une place)
		- C : Obligatoire collectivement (au moins un client doit avoir r�serv� pour que le service soit utilisable)
		- O : Obligatoire (chaque client doit avoir r�serv� pour utiliser le service)
*/
#define RESASFORMATLIGNEType						0


/** D�lai minimal en minutes entre la r�servation et le d�part
	@code MIM valeur @endcode
	@param valeur D�lai minimal en minutes entre la r�servation et le d�part
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinMinutes				1


/** Heure maximale de r�servation le dernier jour de la p�riode de validit�
	@code MIH valeur @endcode
	@param valeur Heure maximale de r�servation le dernier jour de la p�riode de validit�
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinHeureMax			2


/** D�lai minimal en jours entre la r�servation et le d�part
	@code MIJ valeur @endcode
	@param valeur D�lai minimal en jours entre la r�servation et le d�part
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinJours				3


/** D�lai maximal en jours entre la r�servation et le d�part
	@code MAJ valeur @endcode
	@param valeur D�lai maximal en jours entre la r�servation et le d�part
	@warning Le d�lai maximal en jours doit �tre sup�rieur au d�lai minimal en jours
	@see cModaliteReservation::momentDebutReservation
*/
#define RESASFORMATLIGNEDelaiMaxJours				4


/** Description compl�mentaire de la modalit� de r�servation
	@code DES valeur @endcode
	@param valeur Description compl�mentaire de la modalit� de r�servation
*/
#define RESASFORMATLIGNEDoc						5


/** Prix de la r�servation (non g�r�)
	@code PRI valeur @endcode
	@param valeur Prix de la r�servation
*/
#define RESASFORMATLIGNEPrix						6


/** Num�ro de t�l�phone de la centrale de r�servation
	@code TEL valeur @endcode
	@param valeur Num�ro de t�l�phone de la centrale de r�servation
*/
#define RESASFORMATLIGNETel						7


/** Horaires d'ouverture de la centrale de r�servation t�l�phonique
	@code HTL valeur @endcode
	@param valeur Horaires d'ouverture de la centrale de r�servation t�l�phonique
*/
#define RESASFORMATLIGNEHorairesTel				8


/** E-mail o� doivent �tre envoy�s les listes de r�servations (obligatoire pour r�servation en ligne)
	@code EML valeur @endcode
	@param valeur E-mail o� doivent �tre envoy�s les listes de r�servations
*/
#define RESASFORMATLIGNEEMail						9
#define RESASFORMATLIGNEEMailCopie					10

/** Site web de r�servation
	@code URL valeur @endcode
	@param valeur Site web de r�servation
*/
#define RESASFORMATLIGNESiteWeb					11

/** Possibilit� de r�servation en ligne
	@code REL valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : R�servation en ligne impossible
		- 1 : R�servation en ligne possible
*/
#define RESASFORMATLIGNEReservationEnLigne			12


/** Utilisation du champ pr�nom pour la r�servation en ligne
	@code RLP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ pr�nom absent
		- 1 : Champ pr�nom pr�sent, remplissage facultatif
		- 2 : Champ pr�nom pr�sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELPrenom					13


/** Utilisation du champ adresse pour la r�servation en ligne
	@code RLP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ adresse absent
		- 1 : Champ adresse pr�sent, remplissage facultatif
		- 2 : Champ adresse pr�sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELAdresse					14


/** Utilisation du champ e-mail pour la r�servation en ligne
	@code RLE valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ e-mail absent
		- 1 : Champ e-mail pr�sent, remplissage facultatif
		- 2 : Champ e-mail pr�sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELEMail					15


/** Utilisation du champ t�l�phone pour la r�servation en ligne
	@code RLT valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ t�l�phone absent
		- 1 : Champ t�l�phone pr�sent, remplissage facultatif
		- 2 : Champ t�l�phone pr�sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELTelephone				16


/** Utilisation du champ num�ro d'abonn� pour la r�servation en ligne
	@code RLN valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ num�ro d'abonn� absent
		- 1 : Champ num�ro d'abonn� pr�sent, remplissage facultatif
		- 2 : Champ num�ro d'abonn� pr�sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELNumeroAbo				17
#define RESASFORMATLIGNEMax						18
#define RESASFORMATLIGNESeuil						19
#define RESASFORMATLIGNEReferenceALOrigine			20
#define RESASFORMATLIGNENombreFormats				21

#define RESASFORMATCOLONNEStandard					0
#define RESASFORMATCOLONNEHeure					1
#define RESASFORMATCOLONNESNombreFormats			2
//@}

//! @name Format du fichier des v�los
//@{
#define VELOSFORMAT "velo"
#define VELOSEXTENSION "velo.per"
#define VELOSFORMATLIGNEType 						0
#define VELOSFORMATLIGNEContenance 				1
#define VELOSFORMATLIGNEDoc 						2
#define VELOSFORMATLIGNEPrix						3
#define VELOSFORMATLIGNEResa						4
#define VELOSFORMATLIGNENombreFormats				5

#define VELOSFORMATCOLONNEStandard					0
#define VELOSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des personnes handicap�es
//@{
#define HANDICAPESFORMAT "handicape"
#define HANDICAPESEXTENSION "handicape.per"

#define HANDICAPESFORMATLIGNEType					0
#define HANDICAPESFORMATLIGNEContenance				1
#define HANDICAPESFORMATLIGNEDoc					2
#define HANDICAPESFORMATLIGNEPrix					3
#define HANDICAPESFORMATLIGNEResa					4
#define HANDICAPESFORMATLIGNENombreFormats			5

#define HANDICAPESFORMATCOLONNEStandard				0
#define HANDICAPESFORMATCOLONNESNombreFormats		1
//@}

//! @name Format du fichier des tarifs
//@{
#define TARIFSFORMAT "tarif"
#define TARIFSEXTENSION "tarif.per"

#define TARIFSFORMATLIGNEType						0
#define TARIFSFORMATLIGNENbCorrespondance			1
#define TARIFSFORMATLIGNEDuree					2
#define TARIFSFORMATLIGNEPkt						3
#define TARIFSFORMATLIGNEPra						4
#define TARIFSFORMATLIGNEPrb						5
#define TARIFSFORMATLIGNELibelle					6
#define TARIFSFORMATLIGNENombreFormats				7

#define TARIFSFORMATCOLONNEStandard				0
#define TARIFSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des r�seaux
//@{
#define RESEAUXFORMAT "reseaux"
#define RESEAUXEXTENSION "reseaux.per"

#define RESEAUXFORMATLIGNENom						0
#define RESEAUXFORMATLIGNEUrl						1
#define RESEAUXFORMATLIGNEDoc						2
#define RESEAUXFORMATLIGNECartouche				3
#define RESEAUXFORMATLIGNENombreFormats				4

#define RESEAUXFORMATCOLONNEStandard				0
#define RESEAUXFORMATCOLONNESNombreFormats			1
//@}


//! @name Format du repertoire de donnees carto
//@{
#define CARTOEXTENSION "carto"


//!	@name Format du fichier du mat�riel
//@{
#define MATERIELSFORMAT "materiel"
#define MATERIELSEXTENSION "materiel.per"

/** (obligatoire) Libell� simple
	@code NOM libell� @endcode
	@param libell� Libell� simple
*/
#define MATERIELSFORMATLIGNENom					0
#define MATERIELSFORMATLIGNEDoc					1

/** Vitesse maximale des lignes exploit�es avec le mat�riel d�crit
	@code VIT vitesse @endcode
	@param vitesse Vitesse maximale en km/h
*/
#define MATERIELSFORMATLIGNEVitesse				2


/** Libell� du mat�riel � utiliser dans les tableaux d'indicateur papier
	@code IND libell� @endcode
	@param libell� Libell� du mat�riel � utiliser dans les tableaux d'indicateur papier
*/
#define MATERIELSFORMATLIGNEIndicateur				3


/** (obligatoire) Article pour insertion du libell� dans une phrase
	@code ART article @endcode
	@param article Article pour insertion du libell� dans une phrase
	@warning L'article fourni doit inclure l'�ventuel espace situ� apr�s celui-ci.
*/
#define MATERIELFORMATLIGNEArticle					4
#define MATERIELSFORMATLIGNENombreFormats			5

#define MATERIELSFORMATCOLONNEStandard				0
#define MATERIELSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des indicateurs
//@{
#define INDICATEURSFORMAT "indicateurs"
#define INDICATEURSEXTENSION "indicateurs.per"

#define INDICATEURSFORMATLIGNEJC					0
#define INDICATEURSFORMATLIGNEGare					1
#define INDICATEURSFORMATLIGNECP					2
#define INDICATEURSFORMATLIGNENombreFormats			3

#define INDICATEURSFORMATCOLONNEStandard			0
#define INDICATEURSFORMATCOLONNEObligatoire			1
#define INDICATEURSFORMATCOLONNEDepartArrivee		2
#define INDICATEURSFORMATCOLONNENombreFormats		3
//@}



#include "cFichier.h"
#include "cJourCirculation.h"

class cEnvironnement;


/** Fichier d�crivant un calendrier de circulation
*/
class cFichierJoursCirculation : public cFichier
{
	bool 		JCExecuterCommande(cJC& JC, cJC::tSens Sens, cTexte& Tampon, cEnvironnement*);
	TypeSection 	RemplirJC(cJC& JC, cJC::tSens Sens, cTexte& Tampon, cEnvironnement*);

public:
	bool Charge(cEnvironnement*);
	cFichierJoursCirculation(const cTexte& NomFichier) : cFichier(NomFichier, NULL) {}
};

/** Fichier d�crivant les points d'arr�t
*/
class cFichierPointsArret : public cFichier
{
protected:
public:
	bool Charge(cEnvironnement*);
	cFichierPointsArret(const cTexte& __NomFichier, const cTexte& NomFichierFormats) : cFichier(__NomFichier
		, new cFormatFichier(NomFichierFormats, PAFORMAT, PAFORMATLIGNENombreFormats, PAFORMATCOLONNENombreFormats)) {}
};

/** @} */

#endif
