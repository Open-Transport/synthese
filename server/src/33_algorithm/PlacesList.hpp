
/** PlacesList class header.
	@file PlacesList.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_algorithm_PlacesList_hpp__
#define SYNTHESE_algorithm_PlacesList_hpp__

#include <list>
#include <vector>

namespace synthese
{
	namespace algorithm
	{
		/**	Build of the places list of a future schedule sheet corresponding to a journey vector.
			@author Hugues Romain
			@date 2001-2011
			@ingroup m33

			Le but de la methode est de fournir une liste ordonnee de points d'arret de taille minimale determinant les lignes du tableau de fiche horaire.

			Examples of results after journeys addings :

			Pas 0 : Service ABD (adding of B)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td></tr>
			<tr><td>B</td><td>X</td></tr>
			<tr><td>D</td><td>X</td></tr>
			</table>

			Pas 1 : Service ACD (adding of C)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td|</td></tr>
			<tr><td>C</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td></tr>
			</table>

			Pas 2 : Service ACBD (change of the order authorized : descente de B au rang C+1)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>|</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>-</td></tr>
			</table>

			(permutation)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 3 : Service ABCD (change of the order refused : adding of an other C row)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 4 : Service AB->CD (service continu BC)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 5 : Service AED (E insï¿rï¿ avant B pour ne pas rompre la continuitï¿ BC)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
			<tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pour chaque trajet, on procede donc par balayage dans l'ordre des gares existantes. Si la gare a relier nï¿est pas trouvï¿e entre la position de la gare prï¿cï¿dente et la fin, deux solutions :
				- soit la gare nï¿est prï¿sente nulle part (balayage avant la position de la precedente) auquel cas elle est crï¿ï¿e et rajoutï¿e ï¿ la position de la gare prï¿cï¿dente + 1
				- soit la gare est prï¿sente avant la gare prï¿cï¿dente. Dans ce cas, on tente de descendre la ligne de la gare recherchï¿e au niveau de la position de la gare prï¿cï¿dente + 1. On contrï¿le sur chacun des trajets prï¿cï¿dents que la chronologie n'en serait pas affectï¿e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est crï¿ï¿e.

			Contrôle de l'échangeabilité :

			Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$

			Deux lignes l et m sont échangeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respecté.

			Cet ordre s'exprime par la propriété suivante : Si \f$ \Phi \f$ est la permutation pévue, alors

			<img width=283 height=27 src="interface.doxygen_fichiers/image008.gif">

			Il est donc nécessaire à la fois de contrôler la possibilité de permutation, et de la déterminer éventuellement.

			Si <sub><img width=25 height=24
			src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
			précédemment trouvée, et <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿lï¿emplacement de la gare
			souhaitï¿e pour permuter, alors les permutations ï¿ opï¿rer ne peuvent concerner
			que des lignes comprises entre <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿et <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
			autres lignes nï¿influent pas.</p>

			En premier lieu il est nï¿cessaire de dï¿terminer lï¿ensemble
			des lignes ï¿ permuter. Cet ensemble est construit en explorant chaque colonne.
			Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>ï¿est
			lï¿ensemble des lignes ï¿ permuter pour assurer lï¿intï¿gritï¿ des colonnes <sub><img
			width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
			peut dï¿finir cet ensemble en fonction du prï¿cï¿dent <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
			width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub>

			Le but ï¿tant de faire descendre la ligne <sub><img width=24
			height=24 src="interface.doxygen_fichiers/image010.gif"></sub>ï¿vers <sub><img
			width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
			lignes appartenant ï¿ L doivent ï¿tre ï¿changeables avecï¿ les positions <sub><img
			width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
			Lï¿ensemble de ces tests doit ï¿tre rï¿alisï¿. Au moindre ï¿chec, lï¿ensemble de la
			permutation est rendu impossible.

			Lï¿ï¿changeabilitï¿ binaire entre deux lignes l et m revient ï¿
			contrï¿ler la propriï¿tï¿&nbsp;<sub><img width=89 height=28
			src="interface.doxygen_fichiers/image016.gif"></sub>.

			Lï¿ï¿changeabilitï¿ totale sï¿ï¿crit donc <sub><img width=145
			height=28 src="interface.doxygen_fichiers/image017.gif"></sub>

			Lï¿algorithme est donc le suivant&nbsp;:
				- Construction de L
				- Contrï¿le dï¿ï¿changeabilitï¿ binaire pour chaque ï¿lï¿ment de L avec
				sa future position
				- Permutation

			<b>Echange</b>&nbsp;:

			Exemple dï¿ï¿change&nbsp;:

			<table class=MsoNormalTable border=1 cellspacing=0 cellpadding=0 width=340
			style='width:254.95pt;margin-left:141.6pt;border-collapse:collapse;border:
			none'>
			<tr>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			</tr>
			</table>

			<span style='position:relative;z-index:16'><span
			style='left:0px;position:absolute;left:398px;top:-1px;width:67px;height:53px'><img
			width=67 height=53 src="interface.doxygen_fichiers/image018.gif"></span></span><span
			style='position:relative;z-index:13'><span style='left:0px;position:absolute;
			left:371px;top:-1px;width:67px;height:52px'><img width=67 height=52
			src="interface.doxygen_fichiers/image019.gif"></span></span><span
			style='position:relative;z-index:12'><span style='left:0px;position:absolute;
			left:349px;top:-1px;width:62px;height:53px'><img width=62 height=53
			src="interface.doxygen_fichiers/image020.gif"></span></span><span
			style='position:relative;z-index:11'><span style='left:0px;position:absolute;
			left:322px;top:-1px;width:69px;height:52px'><img width=69 height=52
			src="interface.doxygen_fichiers/image021.gif"></span></span><span
			style='position:relative;z-index:10'><span style='left:0px;position:absolute;
			left:269px;top:-1px;width:97px;height:53px'><img width=97 height=53
			src="interface.doxygen_fichiers/image022.gif"></span></span><span
			style='position:relative;z-index:14'><span style='left:0px;position:absolute;
			left:455px;top:-1px;width:37px;height:51px'><img width=37 height=51
			src="interface.doxygen_fichiers/image023.gif"></span></span><span
			style='position:relative;z-index:15'><span style='left:0px;position:absolute;
			left:482px;top:-1px;width:33px;height:51px'><img width=33 height=51
			src="interface.doxygen_fichiers/image024.gif"></span></span><span
			style='position:relative;z-index:6'><span style='left:0px;position:absolute;
			left:248px;top:-1px;width:262px;height:53px'><img width=262 height=53
			src="interface.doxygen_fichiers/image025.gif"></span></span><span
			style='position:relative;z-index:5'><span style='left:0px;position:absolute;
			left:221px;top:-1px;width:206px;height:53px'><img width=206 height=53
			src="interface.doxygen_fichiers/image026.gif"></span></span><span
			style='position:relative;z-index:7'><span style='left:0px;position:absolute;
			left:242px;top:-1px;width:97px;height:52px'><img width=97 height=52
			src="interface.doxygen_fichiers/image027.gif"></span></span><span
			style='position:relative;z-index:9'><span style='left:0px;position:absolute;
			left:216px;top:-1px;width:96px;height:52px'><img width=96 height=52
			src="interface.doxygen_fichiers/image028.gif"></span></span><span
			style='position:relative;z-index:8'><span style='left:0px;position:absolute;
			left:193px;top:-1px;width:96px;height:52px'><img width=96 height=52
			src="interface.doxygen_fichiers/image029.gif"></span></span><span
			style='position:relative;z-index:4'><span style='left:0px;position:absolute;
			left:194px;top:-1px;width:103px;height:52px'><img width=103 height=52
			src="interface.doxygen_fichiers/image030.gif"></span></span>

			<table>
			<tr>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			</tr>
			</table>

		*/
		template<class RowType, class ColType>
		class PlacesList
		{
		public:
			/** Information about a served place, displayed in a schedule sheet presentation.
			*/
			struct PlaceInformation
			{
				RowType place;
				bool isOrigin;
				bool isDestination;

				PlaceInformation(
					RowType _place,
					bool _isOrigin,
					bool _isDestination
				):	place(_place),
					isOrigin(_isOrigin),
					isDestination(_isDestination)
				{}
			};

			/** List of served places, to display in a schedule sheet presentation.
			*/
			typedef std::list<PlaceInformation> List;

		private:

			typedef std::map<ColType, std::vector<typename List::iterator> > CellPositions;

			typedef std::vector<std::pair<ColType, List> > InputLists;

			InputLists _inputLists;

			mutable bool _built;
			mutable List _result;	//!< The result : full list of the served places
			mutable CellPositions _cellPositions;

			typename List::iterator _putPlace(
				typename List::value_type value,
				typename List::iterator minPos
			) const;

			typename List::iterator _getHighestPosition(
				typename List::iterator source,
				typename List::iterator target
			) const;

			bool _canBeSwapped(
				typename List::iterator source,
				typename List::iterator target
			) const;

			void _swap(
				typename List::iterator source,
				typename List::iterator target
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// @dot
			///	digraph {
			///		e0 [label="next place served by the journey"]
			///		e1 [label="find the lowest position with the place"]
			///		e2 [label="has the place been found ?"]
			///		e3 [label="add the place just after the last position used by the journey"]
			///		e4 [label="is the position after the preceding one ?"]
			///		e5 [label=""]
			///		e3 [label="is it the last place ?]
			/// }
			/// @enddot
			void _build() const;

		public:
			PlacesList():
				_built(false)
			{}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds an column at the end of the input list.
				void addList(const typename InputLists::value_type& object);



				//////////////////////////////////////////////////////////////////////////
				/// Removes all columns from the input list.
				void clear();
			//@}

			//! @name Getters
			//@{
				const List& getResult() const;
			//@}
		};



		template<class RowType, class ColType>
		void synthese::algorithm::PlacesList<RowType, ColType>::clear()
		{
			_inputLists.clear();
			_built = false;
		}



		template<class RowType, class ColType>
		typename PlacesList<RowType, ColType>::List::iterator PlacesList<RowType, ColType>::_putPlace(
			typename List::value_type value,
			typename List::iterator minPos
		) const {
			if(!_result.empty())
			{
				typename List::iterator testPos;
				// Search of the place after the minimal position
				for(testPos = minPos;
					testPos != _result.end() && testPos->place != value.place;
					++testPos) ;

				// If found, return of the position
				if(testPos != _result.end())
				{
					return testPos;
				}

				// If not found, search of the place before the minimal position
				if(minPos != _result.begin())
				{
					testPos = minPos;
					for(--testPos;
						testPos != _result.begin() && testPos->place != value.place;
						--testPos) ;

					// If found, try to swap items
					if(testPos != _result.begin() && _canBeSwapped(testPos, minPos))
					{
						_swap(testPos, minPos);
						return testPos;
					}
				}
			}

			// Else insert a new row
			typename List::iterator position;
			if(value.isDestination)
			{
				position = _result.end();
//				typename List::iterator previous(position);
//				--previous;
//				while(previous->isDestination)
//				{
//					--position;
//					--previous;
//				}
			}
			else if(value.isOrigin)
			{
				position = _result.begin();
//				position = minPos;
//				while(position != _result.end() && position->isOrigin)
//				{
//					++position;
//				}
			}
			else
			{
				position = minPos;
			}
			return _result.insert(
				position,
				value
			);
		}



		template<class RowType, class ColType>
		typename PlacesList<RowType, ColType>::List::iterator PlacesList<RowType, ColType>::_getHighestPosition(
			typename List::iterator source,
			typename List::iterator target
		) const {
			typename List::iterator result(source);
			typename List::const_iterator brake(_result.end());
			BOOST_FOREACH(const typename CellPositions::value_type& its, _cellPositions)
			{
				const typename CellPositions::mapped_type& sequence(its.second);

				// Search of the source iterator in the sequence
				typename CellPositions::mapped_type::const_iterator curPos;
				for(curPos = sequence.begin(); curPos != sequence.end() && *curPos != source; ++curPos) ;

				// If not found or if it is the last place, ok
				if(curPos == sequence.end() || curPos + 1 == sequence.end())
				{
					continue;
				}

				// If found, search where the row can be pushed
				typename List::iterator itNewPos;
				for(itNewPos = (*curPos);
					itNewPos != brake && itNewPos != *(curPos+1);
					++itNewPos
				){
					result = itNewPos;
					++result;
					if(result == target)
					{
						break;
					}
				}

				// If next element of the journey was found, it is the new brake
				if(itNewPos == *(curPos + 1))
				{
					brake = *(curPos + 1);
					continue;
				}
			}

			return result;
		}



		template<class RowType, class ColType>
		bool PlacesList<RowType, ColType>::_canBeSwapped(
			typename List::iterator source,
			typename List::iterator target
		) const {
			typename List::iterator maxPos(_getHighestPosition(source, target));
			if(maxPos == target)
			{
				return true;
			}
			typename List::iterator nextMaxPos(maxPos);
			++nextMaxPos;
			if(maxPos != source && nextMaxPos != _result.end() && nextMaxPos != target)
			{
				return _canBeSwapped(maxPos, target);
			}
			return false;
		}



		template<class RowType, class ColType>
		void PlacesList<RowType, ColType>::_swap(
			typename List::iterator source,
			typename List::iterator target
		) const {
			typename List::iterator maxPos(_getHighestPosition(source, target));
			if(maxPos == target)
			{
				_result.splice(target, _result, source);
				return;
			}
			if(maxPos != target)
			{
				_swap(maxPos, target);
				_result.splice(maxPos, _result, source);
			}
		}



		template<class RowType, class ColType>
		void PlacesList<RowType, ColType>::_build() const
		{
			_result.clear();
			_cellPositions.clear();

			BOOST_FOREACH(const typename InputLists::value_type& col, _inputLists)
			{
				typename List::iterator minPos(_result.begin());
				typename CellPositions::mapped_type placePositions;

				BOOST_FOREACH(const typename List::value_type& item, col.second)
				{
					typename List::iterator pos(
						_putPlace(
							item,
							minPos
					)	);
					placePositions.push_back(pos);
					minPos = ++pos;
				}
				_cellPositions.insert(make_pair(col.first, placePositions));
			}
			_built = true;
		}



		template<class RowType, class ColType>
		void PlacesList<RowType, ColType>::addList(
			const typename InputLists::value_type& object
		){
			_inputLists.push_back(object);
			_built = false;
		}



		template<class RowType, class ColType>
		const typename PlacesList<RowType, ColType>::List& PlacesList<RowType, ColType>::getResult(
		) const {
			if(!_built)
			{
				_build();
			}
			return _result;
		}
}	}

#endif // SYNTHESE_algorithm_PlacesList_hpp__
