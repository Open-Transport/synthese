#include "02_db/DbModuleClass.h"


namespace synthese
{
	namespace map
	{

		/** @defgroup m39 39 Map drawing

			Le tracé de carte se fait à partir de quatre catégories d'éléments d'entrée  :
			- données géographiques du serveur (chargées à l'initialisation) : voir @ref uc10 "liste des données"
			- données géographiques propres à la carte, fournies avec la demande de carte, permettant de tracer sur la carte des éléments supplémentaires provenant par exemple de l'application cliente. Les données pouvant être transmises à la volée sont les mêmes que les @ref uc10 "données du serveur" à l'exception des fonds de carte bitmap
			- définition du contenu de la carte
			- zone de couverture
			- échelle
			- définition de la liste des éléments géographiques à tracer
			- paramètres techniques
			- format du fichier produit
			- paramètres de compression

			Le résultat produit est un résultat contenant l'un ou l'autre ou les deux éléments suivants :
			- carte demandée au format bitmap
			- données vectorielles associées à la carte

			Selon le format choisi, les tables de liens et la carte sont ou non fabriqués :

			<table class="tableau">
			<tr><th>Format</th><th>Table de liens</th><th>Carte</th><th>Utilisation de Ghostscript</th></tr>
			<tr><th>PS</th><td>NON</td><td>OUI</td><td>NON</td></tr>	
			<tr><th>PDF</th><td>OUI</td><td>OUI</td><td>OUI</td></tr>	
			<tr><th>JPEG</th><td>NON</td><td>OUI</td><td>OUI</td></tr>	
			<tr><th>HTML</th><td>OUI</td><td>NON</td><td>NON</td></tr>
			<tr><th>Mapinfo</th><td>OUI</td><td>NON</td><td>NON</td></tr>
			</table>

			@{
		*/

		class MapModule : public db::DbModuleClass
		{
		public:

		    static const std::string PARAM_HTTP_TEMP_DIR;
		    static const std::string PARAM_HTTP_TEMP_URL;
		    static const std::string PARAM_BACKGROUNDS_DIR;
		    

		    void preInit ();
		    
		    
		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name, 
						   const std::string& value);		
		    
		};
	    /** @} */
	}
}


