# Glossaire Terminus

----------
### Autre diffuseur

Ces diffuseurs n'ont pas d'actions propres dans le serveur Synthese.  Ce sont
des identifiants qui permettent à un système externe d'interroger les messages
qui les concernent.

Exemples : composant d'un site web, application mobile, ...

*t106_custom_broadcast_points*

----------
### Bibliothèque

Regroupe les **modèles d'évènements** existants, éventuellement en **dossiers
de modèle d'évènements**.

C'est essentiellement une facilité de gestion. Autrement dit, même si cela
serait moins pratique aujourd'hui, on pourrait tout à fait se passer de la
bibliothèque d'un point de vue opérationnel.

----------
### Borne

Un type spécifique de **diffuseur** dont la spécifité est d'être physiquement
attaché à une entité transport en commun (bus, arrêt de bus, ...)

*t041_display_screens*

----------
### Calendrier

Raccourci pour "Calendrier de diffusion".
Regroupe entre 0 et n **plages de diffusion**.

Si aucune plage de diffusion n'est définie, cela est équivalent à une seule
plage de diffusion n'ayant pas de date de début/fin renseignée.

*t110_scenario_calendars*

----------
### Diffuseur

Abstraction définissant un point de diffusion de **message**.

----------
### Dossier de modèles d'évènements

Sert à regrouper des **modèles d'évènements**.
N'a de sens qu'à l'intérieur de la **bibliothèque**.

*t051_scenario_folder*

----------
### Évènement

Un groupement logique de **messages** associés à une actualité (incident,
information urgente ou normale et planifiée)
Parfois le mot **scénario** est utilisé à la place.

*t039_scenarios*

----------
### Fournisseurs de notification

Diffuseur actif généralisant l'émission de notifications sur différents
protocoles.

Les protocoles disponibles sont:
- File pour la génération de fichier local,
- Email pour l'envoi d'emails par SMTP,
- HTTP pour l'émission de requêtes GET ou POST

*t107_notification_providers*
*t108_notification_events*

----------
### Message 

Une information, textuelle ou multimédia, à afficher sur un **diffuseur**.

Un contenu multimédia peut être diffusé par URL ou via la Médiathèque.
Un message est obligatoirement associé à un **calendrier d'application**.

*t003_alarms*

----------
### Modèle d'évènement

Un **évènement** particulier servant de template pour créer un évènement.
Contrairement à un évènement, un modèle d'évènement n'a ni **période
d'application**, ni **calendrier**.

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

La diffusion effective sera le résultat de la combinaison des plages de
diffusion (au sens ensembliste)

*t104_message_application_periods*

----------
### Scénario

Autre mot pour **évènement**, plutôt employé côté implémentation.

----------
### Section

Un regroupement logique d'**évènements** destiné à une unité d'exploitation
spécifique, et donc suivant une politique de droits d'accès appropriée.

Les sections sont organisées à plat, sans niveau hiérarchique.  Un
**évènement** peut être actif sur plusieurs sections à la fois.

A noter qu'un **message** a également une section (une seule) qui correspond à
la section depuis laquelle il a été créé.

La politique de droits est définie via le mécanisme classique de Synthese
(user/rights dans /admin).

*t109_messages_sections*

----------
### Type de borne

Identifie le profil matériel d'une **borne** (i.e. vendeur/modèle...).

*t036_display_types*

----------
### Type de message

Un type de message assure une restriction sur le contenu du message (nombre de
caractères) sans autorisé la saisie HTML.

Lorsqu'un diffuseur sélectionné pour un évènement requiert un type de message
particulier, l'éditeur la saisie d'un **message** alternatif (variante de
contenu).

*t101_message_types*
