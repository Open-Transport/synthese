/** En-t�te classe interpr�teur de textes.
	@file cInterpreteur.h
*/

/** @defgroup m07 07 Interpr�teur de textes
	@{

	@section m07integr Int�gration du module Interpr�teur de textes

	Le module interpr�teur de texte impl�mente une liste permettant la recherche via l'associateur bas� sur un r�seau de neurones.

	Il est pr�vu pour �tre utilis� dans toute liste susceptible d'�tre interrog�e par une requ�te utilisateur inexacte :
		- Liste des communes dans chaque environnement : cInterpreteur<cCommune*>
		- Liste des arr�ts/adresses/lieux dans chaque commune : cInterpreteur<cAccesPADe*>

	Les m�thodes de recherche correspondantes doivent �tre mises � jour pour utiliser l'interpr�teur. Les m�thodes suivantes sont concern�es: 
		- cEnvironnement::TextToCommune
		- cCommune::textToPADe

	Les m�thode d'ajout d'�l�ment sont impact�es par l'ajout d'une commande _interpreteur.Ajout() en fin de code :
		- cEnvironnement::Enregistre(cCommune*)
		- cCommune::addDesignation

	Les m�thodes de suppression seront uitilis�es dans le cadre de la mise en service des modifications temps r�el de la base.
*/


/** Type score de correspondance entre texte entr� et texte trouv�.
*/
typedef int tScore;


/** Classe interpr�teur de textes (template g�n�rique).

	L'interpr�teur g�n�rique fonctionne comme un map o� les objets sont doublement index�s :
		- un inde textuel permet principalement une m�thode d'interrogation d'apr�s un texte entr� inexact
		- un index num�rique simple
	
	Un m�me �l�ment peut �tre index� sous plusieurs indexs textes, mais sous un seul index num�rique.

	La recherche d'un �l�ment peut se faire de deux mani�res :
		- par la fourniture directe de l'index
		- par la recherche pr�alable de l'index d'apr�s un texte � rechercher

	CR : tu g�res tes attributs comme tu veux (avec peut etre une liste normale d'�l�ments en plus pour s'en servir comme tableau avec op�rateur [])
*/
template <class T>
class cInterpreteur<T>
{
protected:
	//! @name Attributs
	//{
		void*		_Reseau;	//!< R�seau de neurones support de l'interpr�tation (CR : met le bon typage du dump : il fait index_texte=>index_numerique)
		vector<T>	_Liste;		//!< Liste d'objets index�e num�riquement
	//}

public:
	//! @name Modificateurs
	//@{
		/** Ajout d'�l�ment dans le r�seau.

			@param __Cle Cl� texte de l'�l�ment
			@param __Element El�ment � ajouter
			@param __Index Index de l'�l�ment (vide/INCONNU = auto : cr�ation d'un index si nouvel �l�ment, rien si �lement d�j� existant)
			@return tIndex L'index de l'�l�ment ajout�, ICONNU si ajout impossible

			Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une cl� texte dans le r�seau vers celui-ci.
			Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'�l�ment dans la liste, puis � l'ajout de la cl� texte dans le r�seau.
			Si l'index n'est pas fourni, et si aucun �l�ment de la liste n'est identique � l'�l�ment fourni, alors l'�l�ment est ajout� en queue de liste
			Si l'index n'est pas fourni et si un �l�ment de la liste est identique, alors une cl� texte est ajout�e au r�seau vers cet �lement

			Dans tous les cas l'index de l'�l�ment est retourn�, qu'il ait �t� cr�� ou non, sauf en cas de probl�me ou la valeur de retour est INCONNU (-1).

		*/
		tIndex Ajout(const cString& __Cle, const T& __Element, tIndex __Index=INCONNU);


		/** Suppression d'un lien de correspondance cl� => �l�ment.

			La suppression par cl� texte ne supprime que le lien texte => �l�ment et non la totalit� des liens vers l'�l�ment point� par la cl� texte.
			
			@warning Un �l�ment peut ne plus poss�der de cl� texte vers celui-ci si la derni�re a �t� supprim�e.

			@param __Index Index de l'�l�ment � supprimer
			@return true si Suppression OK (peut �tre chang�)
		*/
		bool Suppression(const cString& __Cle);


		/** Suppression d'�l�ment dans le r�seau d'apr�s son index.

			La suppression par �l�ment supprime la totalit� des liens vers l'�l�ment.

			@param __Index Index de l'�l�ment � supprimer
			@return true si Suppression OK (peut �tre chang�)
		*/
		bool Suppression(tIndex __Index);
	//@}


	//! @name Accesseurs
	//@{
		/** Accesseur d'apr�s l'index num�rique
			@param __Index Index de l'objet � acc�der
			@return Objet demand�
		*/
		const T& GetElement(tIndex __Index);


		/** Interpr�tation de texte.

			L'interpr�tation s'effectue par action de l'associateur sur le r�seau de neurones

			(d�tailler le fonctionnement � l'int�rieur du source avec tags doxygen pour prise en compte)

			@param __Texte Texte � interpr�ter
			@param __NombreSolutions Nombre de solutions � produire
			@return couple index �l�ment trouv� / score, tri� dans l'ordre dr�croissant de score
		*/
		map<tIndex, tScore> Interprete(const cString& __Texte, tIndex __NombreSolutions);
	//@}


};

/** @} */