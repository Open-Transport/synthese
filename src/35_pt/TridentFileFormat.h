
/** tridentexport class header.
	@file tridentexport.h

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


#ifndef SYNTHESE_IMPEX_TRIDENTEXPORT_H
#define SYNTHESE_IMPEX_TRIDENTEXPORT_H

#include "FileFormatTemplate.h"

#include <iostream>

#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}
	
	namespace pt
	{
		class ServiceDate;

		//////////////////////////////////////////////////////////////////////////
		/// Trident/Chouette file format.
		/// @ingroup m35File refFile
		///
		/// The Trident format is the French standard for public transport theoretical data exchange, based upon <a href="http://www.transmodel.org/fr/">Transmodel</a>.
		///
		/// The Trident format is defined by several XSD schemes :
		///	<ul>
		///		<li><a href="include/35_pt/chouette/Chouette.xsd">Chouette.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Global_schema.xsd">trident_Global_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Location_schema.xsd">trident_Location_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_PT_schema.xsd">trident_PT_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/chouette.zip">ZIP archive containing all Trident XSD files</a></li>
		/// </ul>
		///
		/// @ref TridentFileFormat::_parse
		///
		/// @subsection trident2tisseo Tiss�o Trident Extension
		///
		/// The Tiss�o extension adds some features to the original Trident format (see details below).
		///
		/// The Tiss�o extended Trident format is defined by several XSD schemes :
		///	<ul>
		///		<li><a href="include/35_pt/trident2tisseo/tisseo-chouette-extension.xsd">tisseo-chouette-extension.xsd</a></li>
		///		<li><a href="include/35_pt/trident2tisseo/Chouette.xsd">Chouette.xsd</a></li>
		///		<li><a href="include/35_pt/trident2tisseo/trident/trident_Global_schema.xsd">trident_Global_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2tisseo/trident/trident_Location_schema.xsd">trident_Location_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2tisseo/trident/trident_PT_schema.xsd">trident_PT_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2tisseo/trident2tisseo.zip">ZIP archive containing all Trident XSD files</a></li>
		/// </ul>
		///
		/// <h3>Non concurrence entre lignes</h3>
		///
		/// Deux lignes peuvent donner lieu � une r�gle de non concurrence sur des origines-destinations desservies en commun :
		///	<ul>
		///		<li>Ligne � utiliser</li>
		///		<li>Ligne � ne pas utiliser</li>
		///		<li>D�lai min entre les heures de d�part des services (vide = non concurrence permanente)</li>
		///	</ul>
		///
		/// @image html line-conflict-type.png
		/// 
		/// <h3>Propri�t�s additionnelles du service</h3>
		///
		/// <ul>
		///		<li>PMR Oui/Non</li>
		///		<li>R�servation obligatoire pour PMR uniquement (pointe vers une modalit� de r�servation)</li>
		///		<li>V�lo Oui/Non</li>
		///		<li>R�servation obligatoire pour v�lo (pointe vers une modalit� de r�servation)</li>
		///		<li>Renvoi : Le format Trident permet de pr�ciser un commentaire au niveau de la course, mais ne fait pas r�f�rence � un renvoi. Il est donc n�cessaire d��tendre le type de la course pour d�finir un ou plusieurs renvois.</li>
		///	</ul>
		///
		/// @image html tisseo-vehicle-journey.png
		///
		/// <h3>Sens aller-retour</h3>
		///
		/// Le champ wayBack est utilis� diff�remment par rapport � Trident : 1 indique le sens aller, 2 indique le sens retour.
		///
		/// <h3>Lignes de transport � la demande zonales</h3>
		///
		/// Les composants d�un itin�raire d�une ligne de transport � la demande zonale peuvent �tre de deux natures :
		///	<ul>
		///		<li>Un arr�t classique (couvert par Trident actuel)</li>
		///		<li>Une zone de transport � la demande, elle-m�me compos�e d�arr�ts classiques</li>
		/// </ul>
		///
		/// Les zones de transport � la demande sont d�finies comme suit :
		/// <ul>
		///		<li>Compos�e de n arr�ts</li>
		///		<li>Desserte interne � la zone autoris�e ou non</li>
		///		<li>Dur�e de desserte interne</li>
		///	</ul>
		///
		/// Les r�gles classiques de gestion des composants d�itin�raire sont valables sur les zones, dont notamment :
		/// <ul>
		///		<li>Une heure d�arriv�e par service</li>
		///		<li>Une heure de d�part par service</li>
		///		<li>Possibilit�s d�ITL sur une s�rie de zones</li>
		///	</ul>
		///
		/// Il est propos� d�utiliser les objets PTLink, standard, en faisant pointer les liens startOfLink et endOfLink vers des ID de nouveaux objets DRTStopArea, d�finis comme suit :
		///	<ul>
		///		<li>Les horaires sont stock�s comme � l�accoutum�e en faisant pointer les stopPointId vers les ID des DRTStopArea correspondants.</li>
		///		<li>Pour mod�liser la desserte interne, on place deux fois la zone � la suite dans le descriptif d�itin�raire.</li>
		///	</ul>
		///
		/// @image html drt-stop-area.png
		///
		/// <h3>Modalit�s de r�servation</h3>
		///
		/// Les modalit�s de r�servation permettent de d�finir la plage temporelle pendant laquelle un service � r�servation obligatoire peut �tre r�serv�.
		///
		/// Les caract�ristiques des modalit�s de r�servations sont les suivantes :
		/// <ul>
		///		<li>Caract�re obligatoire de la r�servation (Obligatoire, Facultatif au d�part de certains arr�ts et obligatoires aux autres, ou Facultatif)</li>
		///		<li>Choix T0 = d�part du client ou d�part de l�origine</li>
		///		<li>Dur�e minimale en minutes avant T0</li>
		///		<li>Dur�e minimale en jours avant T0</li>
		///		<li>Heure limite de r�servation si la veille de T0</li>
		///		<li>Dur�e maximale en jours avant T0</li>
		///		<li>Num�ro de t�l�phone � joindre (vide = pas de centre d�appels)</li>
		///		<li>Horaires d�ouverture du centre d�appels (en texte)</li>
		///		<li>URL du site Internet de r�servation (vide = pas de site Internet de r�servation)</li>
		///	</ul>
		///
		/// @image html reservation-rule.png
		///
		/// <h3>Caract�ristiques particuli�res des services � r�servation</h3>
		///
		/// Les services � r�servation n�cessitent d�ajouter les caract�ristiques suivantes :
		/// <ul>
		///		<li>Modalit� de r�servation</li>
		///		<li>Liste des arr�ts permettant la r�servation facultative (cf modalit�s de r�servation)</li>
		///		<li>Nombre de places limit� (Si oui, valeur = nombre de places maximal)</li>
		/// </ul>
		///
		/// @image html drt-vehicle-journey.png
		///
		/// <h3>Arr�t principal de commune</h3>
		///
		/// Pouvoir d�finir un ou plusieurs arr�ts principaux par commune, qui sont choisis par d�faut dans les syst�mes d�information lorsque l�utilisateur ne saisit par d�arr�t
		///	On d�finit pour chaque commune un objet de type Area incluant les arr�ts consid�r�s comme principaux.
		///
		/// @image html city-main-stops.png
		///
		/// <h3>Accessibilit� PMR</h3>
		///
		/// <ul>
		///		<li>Oui/Non</li>
		/// </ul>
		///
		/// Il est propos� d�ajouter un champ au StopPoint :
		///
		/// @image html tisseo-stop-point.png
		///
		/// <h3>Horaire d�exploitation</h3>
		///
		/// Les horaires fournis � chaque franchissement d�arr�t peuvent provenir de l�exploitation (source logiciel m�tier) ou �tre interpol�es. Dans certains cas il peut �tre utile de savoir d�o� provient l�horaire. 
		///
		/// <ul>
		///		<li>Oui/Non</li>
		/// </ul>
		///
		/// Il est propos� d�ajouter deux champs � l�objet PTLink (un pour l�arr�t de d�part, un pour l�arr�t d�arriv�e). Les champs sont facultatifs : l�absence de valeur est � interpr�ter comme suit :
		/// <ul>
		///		<li>Oui si l�arr�t est une extr�mit� de l�itin�raire</li>
		///		<li>Non sinon</li>
		/// </ul>
		///
		/// @image html tisseo-pt-link.png
		///
		class TridentFileFormat
		:	public impex::FileFormatTemplate<TridentFileFormat>
		{
		public:
			static const std::string PARAMETER_IMPORT_STOPS;
		    
		private:
			//! @name Import parameters
			//@{
				bool		_importStops;
			//@}

			//! @name Export parameters
			//@{
				util::RegistryKeyType	_commercialLineId;
			//@}

			//! @name Format parameters
			//@{
				const bool				_withTisseoExtension;
			//@}

			//! @name Temporary data
			//@{
				std::vector<boost::shared_ptr<pt::ServiceDate> > _serviceDates;
			//@}
		
		protected:
				


			//////////////////////////////////////////////////////////////////////////
			/// Trident file import.
			/// @param filePath path of the file to import
			/// @param os stream to write information messages on
			/// @param key unused parameter
			/// @author Hugues Romain
			///
			/// The Trident file import loads the following objects :
			///	<ul>
			///		<li>Commercial stop points (PublicTransportStopZoneConnectionPlace) : city, name, specific transfer delays (only if stop import mode)</li>
			///		<li>Physical stop points (PhysicalStop) : commercial stop point (only at physical stop creation), x, y, name (only if stop import mode)</li>
			/// </ul>
			virtual void _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				std::string key = std::string()
			);



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// @return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual server::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void _setFromParametersMap(const server::ParametersMap& map);
		
		public:

			TridentFileFormat(
				util::Env* env = NULL,
				util::RegistryKeyType lineId = UNKNOWN_VALUE,
				bool withTisseoExtension = false
			);
			~TridentFileFormat();

			/** -> ChouettePTNetwork
			 */
			virtual void build(std::ostream& os);
			
			virtual void save(std::ostream& os) const;

			//! @name Setters
			//@{
				void setImportStops(bool value);
			//@}

			//! @name Getters
			//@{
				bool getImportStops() const;
			//@}

		private:
		    
			static std::string TridentId (const std::string& peer, const std::string clazz, const uid& id);
			static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
			static std::string TridentId (const std::string& peer, const std::string clazz, const util::Registrable& obj);
			static std::string GetCoordinate(const double value);
		};
	}
}

#endif
