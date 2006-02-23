/** En-t�te classe interpr�teur de textes.
	@file Interpretor.h
*/

#ifndef SYNTHESE_INTERPRETOR_H
#define SYNTHESE_INTERPRETOR_H

#include <utility>
#include <string>
#include <map>
#include <set>
#include "cAssociator.h"

namespace interpretor
{

	/** Score d'un r�sultat (� d�placer dans associator je pense) */
	typedef int Score;

	/** Index */
	typedef int Index;

	/** R�sultat */
	typedef pair<Index, Score> Result;

	/** Cl� */
	typedef string Key;

	/** ID inconnu */
	const Index UNKNOWN_INDEX = -1;


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
class Interpretor
{
public:
	typedef std::map<Index, T> MapType;

protected:
	//! @name Attributs
	//{
		cAssociator		_associator;	//!< R�seau de neurones support de l'interpr�tation (CR : met le bon typage du dump : il fait index_texte=>index_numerique)
		MapType	_map;		//!< Liste d'objets index�e num�riquement
	//}

public:

	//!	@name Accesseurs
	//@{
		const MapType& getMap() const { return _map; }
	//@}

	//!	@name Constructeur et destructeur.
	//@{
		Interpretor() {}
		~Interpretor() {}
	//@}










/** Ajout d'�l�ment dans le r�seau.

	@param key Cl� texte de l'�l�ment
	@param element El�ment � ajouter
	@param id Index de l'�l�ment (vide/INCONNU = auto : cr�ation d'un index si nouvel �l�ment, rien si �lement d�j� existant)
	@return tIndex L'index de l'�l�ment ajout�, ICONNU si ajout impossible

	Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une cl� texte dans le r�seau vers celui-ci.
	Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'�l�ment dans la liste, puis � l'ajout de la cl� texte dans le r�seau.
	Si l'index n'est pas fourni, et si aucun �l�ment de la liste n'est identique � l'�l�ment fourni, alors l'�l�ment est ajout� en queue de liste
	Si l'index n'est pas fourni et si un �l�ment de la liste est identique, alors une cl� texte est ajout�e au r�seau vers cet �lement

	Dans tous les cas l'index de l'�l�ment est retourn�, qu'il ait �t� cr�� ou non, sauf en cas de probl�me ou la valeur de retour est INCONNU (-1).

	@todo Mettre une section critique + CR impl�menter le lien r�seau neurone
*/
Index add(const Key& key, const T& element, Index id)
{
	_map[id] = element;

	// Etablissement du lien dans le r�seau de neurone


	return id;
}


/** Suppression d'un lien de correspondance cl� => �l�ment.

	La suppression par cl� texte ne supprime que le lien texte => �l�ment et non la totalit� des liens vers l'�l�ment point� par la cl� texte.
	
	@warning Un �l�ment peut ne plus poss�der de cl� texte vers celui-ci si la derni�re a �t� supprim�e.

	@param __Index Index de l'�l�ment � supprimer
	@return true si Suppression OK (peut �tre chang�)

	@todo CR : Impl�menter, �a ne concerne que le r�seau de neurone
*/
void erase(const Key& key)
{
	
}


/** Suppression d'�l�ment dans le r�seau d'apr�s son index.

	La suppression par �l�ment supprime la totalit� des liens vers l'�l�ment.

	@param id Index de l'�l�ment � supprimer
	@return true si Suppression OK (peut �tre chang�)

	@todo CR : faire la partie r�seau neurone
*/
bool erase(Index id)
{
	// CR : ici l'effacement de toutes les cl�s dans le r�seau
	
	_vector.erase(id);
}


/** Accesseur d'apr�s l'index num�rique
	@param __Index Index de l'objet � acc�der
	@return Objet demand�
*/
T& operator[](Index id)
{
	return _vector.operator [](id);
}


/** Interpr�tation de texte.

	L'interpr�tation s'effectue par action de l'associateur sur le r�seau de neurones

	(d�tailler le fonctionnement � l'int�rieur du source avec tags doxygen pour prise en compte)

	@param key Texte � interpr�ter
	@param size Nombre de solutions � produire
	@return couple index �l�ment trouv� / score, tri� dans l'ordre dr�croissant de score

	@todo CR : A impl�menter
*/
set<Result> search(const Key& key, Index size) const
{
	set<Result> result;

	return result;
}








};

/** @} */

}


#endif
