# Glossaire Terminus

----------
### Autre diffuseur   
Exemples : composant d'un site web, application mobile, ...

*t106_custom_broadcast_points*

----------
### Bibliothèque   
Regroupe les **modèles d'évènements** existants, evenutuellement en **dossiers de modèle d'évènements**.

C'est essentiellement une facilité de gestion. Autrement dit, même si cela serait moins pratique aujourd'hui, on pourrait tout à fait se passer de la bibliotheque d'un point de vue opérationnel.

----------
### Borne   
Un type specifique de **diffuseur** dont la spécifité est d'être
physiquement attaché à une entité transport en commun (bus, arrêt de
bus, ...)

*t041_display_screens*

----------
### Calendrier   
Raccourci pour "Calendrier de diffusion".
Regroupe entre 0 et n **plages de diffusion**.

Si aucune plage de diffusion n'est définie, cela est équivalent à une seule plage de diffusion n'ayant pas de date de début/fin  renseignée.

*t110_scenario_calendars*

----------
### Diffuseur   
Abstraction définissant un point de diffusion de **message**.

----------
### Dossier de modèles d'évènements
Sert à regrouper des **modèles d'évènements**.
N'a de sens qu'à l'intérieur de la **bibliotheque**.

*t051_scenario_folder*

----------
### Evenement
Un groupement logique de **messages** associés à une actualité (incident,
information urgente ou normale et planifiée)  
Parfois le mot **scénario** est utilisé à la place.

*t039_scenarios*

----------
### Listes de diffusion
Exemples : mailing list, liste de contacts SMS...

Automatique ou manuel ? Non défini.

Non implementé aujourd'hui.

----------
### Message 
Une information, textuelle ou multimédia, à afficher sur un **diffuseur**.

Un contenu multimédia peut-être difffusé par URL ou via la Médiathèque.
Un message est obligatoirement associé a un **calendrier d'application**.

*t003_alarms*

----------
### Modèle d'évènement
Un **évènement** particulier servant de template pour créer un évènement.
Contrairement à un évènement, un modèle d'évènement n'a ni **période d'application**, ni **calendrier**.  

*t039_scenarios (+flag is_template)*

----------
### Modèle de message
Un **message** particulier servant de template pour créer un message.
Tout comme le message, le modèle de message a un **calendrier d'application**.

*t003_alarms (+flag is_template)*

----------
### Période d'application   
Dates de début et de fin de l'évènement.

----------
### Plage de diffusion
Une période de temps continue définie par des dates/heures de début et
de fin.

Date/heure de début non renseignée => "maintenant"
Date/heure de fin non renseignée => "pour toujours"

La diffusion effective sera le resultat de la combinaison des plages
de diffusion (au sens ensembliste)

*t104_message_application_periods*

----------
### Scénario
Autre mot pour **évènement**, plutôt employé côté implémentation.

----------
### Section   
Un regroupement logique d'**évènements** destiné à une unité
d'exploitation specifique, et donc suivant une politique de droits
d'accès appropriée.

Les sections sont organisées à plat, pas de niveau hiérarchique.
Un **message** peut etre actif sur plusieurs sections à la fois.

La politique de droits est definie via le mecanisme classique de
Synthese (user/rights dans admin).

*t109_messages_sections*

----------
### Type de borne
Identifie le profil matériel d'une **borne** (ie vendeur/modele...).

*t036_display_types*

----------
### Type de message
Restrictions sur le contenu du message (nombre de caractères). 
Ne s'applique que sur les **messages** alternatifs.
Ne semble pas très utilisé aujourd'hui ; mérite clarification

*t101_message_types*

