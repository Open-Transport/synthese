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

	/** R�sultat */
	typedef pair<size_t, Score> Result;

	/** Cl� */
	typedef std::string Key;

	/** ID inconnu */
	const size_t UNKNOWN_INDEX = -1;


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

	Les m�thodes de suppression seront utilis�es dans le cadre de la mise en service des modifications temps r�el de la base.
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
	typedef std::map<size_t, T> MapType;

protected:
	//! @name Attributs
	//{
		cAssociator		_associator;	//!< R�seau de neurones support de l'interpr�tation (CR : mettre le bon typage : il fait index_texte=>index_numerique)
		MapType	_map;		//!< Liste d'objets index�e num�riquement
	//}

public:

	//!	@name Accesseurs
	//@{
		const MapType& getMap() const { return _map; }

		/** Accesseur d'apr�s l'index num�rique
			@param __Index Index de l'objet � acc�der
			@return Objet demand�
		*/
		T& at(size_t id) const
		{
			return _map[id];
		}


	//@}

	//!	@name Constructeur et destructeur.
	//@{
		Interpretor() {}
		~Interpretor() {}
	//@}


	//!	@name Modificateurs
	//@{
		/** Adding of an element in the list, without specified index.
			@param key Text key of the element
			@param element Element to be added
			@return Attributed index
			@todo Put a critical section
		*/
		size_t add(const Key& key, const T& element)
		{
			size_t id;
			if (_map.size())
			{
				MapType::const_iterator iter = _map.end();
				--iter;
				id = iter->first + 1;
			}
			else
				id = 0;
		
			// Adding of the element
			add(key, element, id);

			// returns the id
			return id;
		}
		

		/** Ajout d'�l�ment dans le r�seau.

			@param key Cl� texte de l'�l�ment
			@param element El�ment � ajouter
			@param id Index de l'�l�ment (vide/INCONNU = auto : cr�ation d'un index si nouvel �l�ment, rien si �lement d�j� existant)

			Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une cl� texte dans le r�seau vers celui-ci.
			Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'�l�ment dans la liste, puis � l'ajout de la cl� texte dans le r�seau.
			Si l'index n'est pas fourni, et si aucun �l�ment de la liste n'est identique � l'�l�ment fourni, alors l'�l�ment est ajout� en queue de liste
			Si l'index n'est pas fourni et si un �l�ment de la liste est identique, alors une cl� texte est ajout�e au r�seau vers cet �lement
		*/
		void add(const Key& key, const T& element, size_t id)
		{

			// storage in map contenor
			_map[id] = element;

			// Etablissement du lien dans le r�seau de neurone
			// TODO CR

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
		bool erase(size_t id)
		{
			// CR : ici l'effacement de toutes les cl�s dans le r�seau
			
			_map.erase(id);
		}
	//@}

	//!	@name Calculateur
	//@{
		/** Interpr�tation de texte.

			L'interpr�tation s'effectue par action de l'associateur sur le r�seau de neurones

			(d�tailler le fonctionnement � l'int�rieur du source avec tags doxygen pour prise en compte)

			@param key Texte � interpr�ter
			@param size Nombre de solutions � produire
			@return couple index �l�ment trouv� / score, tri� dans l'ordre dr�croissant de score

			@todo CR : A impl�menter
		*/
		set<Result> search(const Key& key, size_t size) const
		{
			set<Result> result;

			return result;
		}
	//@}




};

/** @} */

}


#endif
