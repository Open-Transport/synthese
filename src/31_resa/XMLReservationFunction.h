
//////////////////////////////////////////////////////////////////////////////////////////
/// XMLReservationFunction class header.
///	@file XMLReservationFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_XMLReservationFunction_H__
#define SYNTHESE_XMLReservationFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		/** XMLReservationFunction Function class.
			@author Hugues Romain
			@date 2009
			@ingroup m31Functions refFunctions

			Parameters :
				- fonction=XMLReservationFunction
				- sid=<session id> : ID of the current session
				- roid=<id reservation> : Reservation ID (can be the BookReservationAction created id)
			
			R�ponse

			Description

			Une r�servation est d�crite comme suit :

			(sch�ma)

				- id : identificateur de la r�servation dans la base de donn�es de SYNTHESE (vide si
			r�servation non effectu�e ou non trouv�e)
				- customerId : identificateur du client dans la base de donn�es de SYNTHESE
				- cancellationDeadLine : date/heure limite d�annulation de la r�servation
				- departureStop : nom complet (commune + nom) de l�arr�t de d�part du trajet
			r�serv�
				- arrivalStop : nom complet (commune + nom) de l�arr�t d�arriv�e du trajet r�serv�
				- travelDate : date/heure du d�part du trajet r�serv�
				- customerName : nom complet (pr�nom + nom) du client ayant r�serv�
				- customerPhone : num�ro de t�l�phone du client ayant r�serv�
				- status : statut de la r�servation (texte)
				- canBeCancelled (bool�en) : indique si la r�servation peut �tre annul�e
				- seats : nombre de places r�serv�es
				- cancellationDateTime (optionnel) : sa pr�sence indique que la r�servation a �t�
			annul�e. Sa valeur indique la date et l�heure de l�annulation
				- le d�tail du trajet r�serv� est d�crit par une succession d�objets chunk d�crivant
			chaque �tape du trajet :
					- departurePlaceName : nom complet de l�arr�t de d�part du tron�on
					- departureDateTime : date/heure de d�part du tron�on
					- arrivalPlaceName : nom complet de l�arr�t d�arriv�e du tron�on
					- arrivalDateTime : date/heure d�arriv�e du tron�on
					- lineNumber : num�ro de la ligne emprunt�e

		*/
		class XMLReservationFunction:
			public util::FactorableTemplate<server::Function,XMLReservationFunction>
		{
		public:
			
		protected:
			boost::shared_ptr<const ReservationTransaction> _resa;

			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
		public:
			XMLReservationFunction();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_XMLReservationFunction_H__
