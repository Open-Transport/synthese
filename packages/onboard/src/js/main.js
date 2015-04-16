define([
	"lib/synthese/js/Synthese",
	"./unitesExploitations.js",
	"./servicesVoitures.js",
	"./courses.js",
	"./feuilleDeRoute.js",
	"./ajoutCommune.js",
	"./ajoutArret.js",
	"backbone",
], function(Synthese,unitesExploitations,servicesVoitures,courses,feuilleDeRoute,ajoutCommune,ajoutArret, Backbone) {

	var Onboard = Backbone.Model.extend({
	});

	var OnboardView = Backbone.View.extend({
		events: {
			"click .btn-back": "btnBackClicked",
		},

		hideLoader: function() {
			$('#loader').hide();
		},

		selectUniteExploitation: function(id) {
			// On a sélectionné une unité d'exploitation
			// on doit enregistrer cet état dans les données persistentes puis afficher la page
			this.setCookie('pageDisplayed','servicesVoitures');
			this.setCookie('unid',id);
			var self = this;
			$('#loader').show(1,function(){
				self.displayServicesVoitures(id);
			});
		},

		selectServiceVoiture: function(id) {
			// On a sélectionné un service voiture
			// on doit enregistrer cet état dans les données persistentes puis afficher la page
			this.setCookie('pageDisplayed','courses');
			this.setCookie('svid',id);
			var self = this;
			$('#loader').show(1,function(){
				self.displayCourses(id);
			});
		},

		selectCourse: function(id) {
			// On a sélectionné un service voiture
			// on doit enregistrer cet état dans les données persistentes puis afficher la page
			this.setCookie('pageDisplayed','feuilleDeRoute');
			this.setCookie('sid',id);
			var self = this;
			$('#loader').show(1,function(){
				/*datas = {
				 a : "VehiclePositionUpdateAction",
				 actionParamsi: id,
				 actionParam_set_as_current_position: 1,
				 actionParamst: 4,
				 };
				 $.ajax({
				 type: 'GET',
				 url: "synthese",
				 data: datas
				 });*/
				Synthese.callService('page',{
					'p': '17733370209370127',
					'a': 'VehiclePositionUpdateAction',
					'actionParamsi': id,
					'actionParam_set_as_current_position': 1,
					'actionParamst': 4
				}).then(function(content){
					    console.log('Entrée dans une course');
				    });
				self.displayFeuilleDeRoute(id);
			});
		},

		selectCommune: function() {
			// On a sélectionné un service voiture
			// on doit enregistrer cet état dans les données persistentes puis afficher la page
			this.setCookie('pageDisplayed','ajoutCommune');
			var self = this;
			$('#loader').show(1,function(){
				self.displayCommunes();
			});
		},

		selectArret: function(id) {
			// On a sélectionné un service voiture
			// on doit enregistrer cet état dans les données persistentes puis afficher la page
			this.setCookie('pageDisplayed','ajoutArret');
			this.setCookie('comid',id);
			var self = this;
			$('#loader').show(1,function(){
				self.displayArrets();
			});
		},

		displayUnitesExploitations: function() {
			console.log("displayUnitesExploitations");
			this.unitesExploitationsView.init();
			this.$("#titleOfPage").html("Unités d'exploitation");
			this.$("#zoneBack").hide();

			// Hide et show des vues
			this.servicesVoituresView.hide();
			this.unitesExploitationsView.show();
		},

		displayServicesVoitures: function(id) {
			console.log("displayServicesVoitures");
			// Changement du titre
			Synthese.callService('object',{
				'roid': id,
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function(content) {
				    if (content.object) {
					    if (content.object.name) {
						    $("#titleOfPage").html("Services Voitures " + content.object.name);
					    }
				    }
			    });
			this.$("#txtBack").html("Unités");
			this.$("#zoneBack").show();

			// Init de la vue Services Voitures
			this.servicesVoituresView.init(id);
			// Hide de la vue des unités d'exploitation
			this.unitesExploitationsView.hide();
			// Hide de la vue des courses
			this.coursesView.hide();
			// Affichage de la page
			this.servicesVoituresView.show();
		},

		displayCourses: function(id) {
			console.log("displayCourses");
			// Changement du titre
			Synthese.callService('object',{
				'roid': id,
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function(content) {
				    if (content.object) {
					    if (content.object.name) {
						    $("#titleOfPage").html("Courses " + content.object.name).show();
					    }
				    }
			    });

			var unid=this.readCookie('unid');
			// Changement du texte du bouton 'back'
			Synthese.callService('object',{
				'roid': unid,
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function(content) {
				    $("#zoneBack").hide();
				    if (content.object) {
					    if (content.object.name) {
						    $("#txtBack").html(content.object.name);
					    }
				    }
				    $("#zoneBack").removeAttr('data-action').show();
			    });

			// Init de la vue Courses
			this.coursesView.init(id);
			// Hide de la vue des services voitures
			this.servicesVoituresView.hide();
			// Hide de la vue feuille de route
			this.feuilleDeRouteView.hide();
			// Affichage de la page
			this.coursesView.show();
		},

		displayFeuilleDeRoute: function(sid) {
			console.log("displayFeuilleDeRoute");
			var svid = this.readCookie('svid');
			// Changement du bouton retour
			$("#zoneBack").hide();
			$.when(
				Synthese.callService('object',{
					'roid': svid,
					'additional_parameters': '1',
					'output_format': 'json',
				}),
				Synthese.callService('object',{
					'roid': sid,
					'additional_parameters': '1',
					'output_format': 'json',
				}),
				Synthese.callService('PTRouteDetailFunction',{
					'roid': sid,
					'output_format': 'json',
				})
			    ).then(function(serviceVehicule, service, routeDetail) {
				    if (serviceVehicule[0].object) {
					    if (serviceVehicule[0].object.name) {
						    $("#txtBack").html(serviceVehicule[0].object.name);
					    }
				    }
				    $("#zoneBack").attr('data-action', 'close_actions').show();
				    var serviceNumber = service[0].object.service_number;
				    $("#titleOfPage").html(routeDetail[0].route.direction + ' ' + serviceNumber.slice(0,2) + '/' + serviceNumber.slice(2, serviceNumber.length)).show();
			    });

			// Init de la vue Feuille de Route
			this.feuilleDeRouteView.init(sid, svid);
			// Hide de la vue des courses
			this.coursesView.hide();
			// Hide de la vue des communes;
			this.ajoutCommuneView.hide();
			// Affichage de la page
			this.feuilleDeRouteView.show();
		},

		displayCommunes: function() {
			console.log("displayCommunes");
			this.ajoutCommuneView.init(this.readCookie('sid'));
			this.$("#titleOfPage").html("Ajout d'une descente").show();
			var sid=this.readCookie('sid');
			// Changement du texte du bouton 'back'
			Synthese.callService('object',{
				'roid': sid,
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function(content) {
				    $("#zoneBack").hide();
				    if (content.object) {
					    if (content.object.service_number) {
						    $("#txtBack").html("Course " + content.object.service_number);
					    }
				    }
				    $("#zoneBack").removeAttr('data-action').show();
			    });

			// Hide et show des vues
			this.feuilleDeRouteView.hide();
			this.ajoutArretView.hide();
			this.ajoutCommuneView.show();
		},

		displayArrets: function() {
			console.log("displayArrets");
			this.ajoutArretView.init(this.readCookie('sid'), this.readCookie('comid'));
			Synthese.callService('object',{
				'roid': this.readCookie('comid'),
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function(content) {
				    if (content.object) {
					    if (content.object.city_name) {
						    $("#titleOfPage").html("Ajout d'une descente - commune de " + content.object.city_name).show();
					    }
				    }
			    });
			this.$("#txtBack").html("Communes");

			// Hide et show des vues
			this.ajoutCommuneView.hide();
			this.ajoutArretView.show();
		},

		btnBackClicked: function(event) {
			var self = this;
			if (typeof event != 'undefined') {
				var element = $(event.currentTarget);
				intersynthese = JSON.parse(self.readCookie('intersyntheseCall'));
				if (intersynthese && element.attr('data-action') == "close_actions" && typeof self.feuilleDeRouteView.modalDisplay === "function") {
					self.feuilleDeRouteView.modalDisplay('closeAllActions', 'close_actions', 'zoneBack');
					return;
				}
			}
			$('#loader').show(1,function() {
				if (self.readCookie('pageDisplayed') === "servicesVoitures") {
					self.setCookie('pageDisplayed','unitesExploitations');
					self.displayUnitesExploitations();
				}
				if (self.readCookie('pageDisplayed') === "courses") {
					self.setCookie('pageDisplayed','servicesVoitures');
					self.displayServicesVoitures(self.readCookie('unid'));
				}
				if (self.readCookie('pageDisplayed') === "feuilleDeRoute") {
					self.setCookie('pageDisplayed','courses');
					/*datas = {
					 a : "VehiclePositionUpdateAction",
					 actionParamsi: 0,
					 actionParam_set_as_current_position: 1,
					 actionParamst: 999,
					 };
					 $.ajax({
					 type: 'GET',
					 url: "synthese",
					 data: datas
					 });*/
					Synthese.callService('page',{
						'p': '17733370209370127',
						'a': 'VehiclePositionUpdateAction',
						'actionParamsi': 0,
						'actionParam_set_as_current_position': 1,
						'actionParamst': 999
					}).then(function(content){
						    console.log('Sortie de course');
					    });
					self.displayCourses(self.readCookie('svid'));
				}
				if (self.readCookie('pageDisplayed') === "ajoutCommune") {
					self.setCookie('pageDisplayed','feuilleDeRoute');
					self.displayFeuilleDeRoute(self.readCookie('sid'));
				}
				if (self.readCookie('pageDisplayed') === "ajoutArret") {
					self.setCookie('pageDisplayed','ajoutCommune');
					self.displayCommunes();
				}
			});
		},

		setCookie: function(name, value, minutes, path, domain, secure) {
			var expires, date;
			if (typeof minutes == "number") {
				date = new Date();
				date.setTime( date.getTime() + (minutes*60*1000) );
				expires = date.toGMTString();
			}
			document.cookie = name + "=" + value +
			    ((expires) ? "; expires=" + expires : "") +
			    ((path) ? "; path=" + path : "") +
			    ((domain) ? "; domain=" + domain : "") +
			    ((secure) ? "; secure" : "");
		},

		readCookie: function(name) {
			var nameEQ = name + "=";
			var ca = document.cookie.split(';');
			for (var i = 0; i < ca.length; i++) {
				var c = ca[i];
				while (c.charAt(0) == ' ') c = c.substring(1, c.length);
				if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
			}
			return null;
		},

		initialize: function(options) {

			var self = this;
			// Functions
			function goHome() {
				console.log("goHome " + Date.now());
				$('#loader').show(1,function() {
					// Ici on met toute l'intelligence du lancement de l'application :
					//  lecture des cookies pour initialiser les attributs...
					var allCookiesToStart = false;
					if (self.readCookie('pageDisplayed') === "servicesVoitures") {
						if (self.readCookie('unid')) {
							self.displayServicesVoitures(self.readCookie('unid'));
							allCookiesToStart = true;
						}
					}
					else if (self.readCookie('pageDisplayed') === "courses") {
						if (self.readCookie('svid') && self.readCookie('unid'))
						{
							self.displayCourses(self.readCookie('svid'));
							allCookiesToStart = true;
						}
					}
					else if (self.readCookie('pageDisplayed') === "feuilleDeRoute"
					    || self.readCookie('pageDisplayed') === "ajoutCommune"
					    || self.readCookie('pageDisplayed') === "ajoutArret") {
						if (self.readCookie('svid') && self.readCookie('unid') && self.readCookie('sid'))
						{
							self.displayFeuilleDeRoute(self.readCookie('sid'));
							allCookiesToStart = true;
						}
					}

					if (!allCookiesToStart) {
						self.coursesView.hide();
						self.displayUnitesExploitations();
					}
				});
			}

			function getTime(hourId, minutesId, colons) {
				$(window).data({"DataGetTime": setInterval(function () {
					var date = new Date(),
					    hour = date.getHours(),
					    minutes = ((date.getMinutes()<10)? '0' + date.getMinutes(): date.getMinutes()),
					    seconds = ((date.getSeconds()<10)? '0' + date.getSeconds(): date.getSeconds());

					var htmlHour = $(hourId).html();
					var htmlMinutes = $(minutesId).html();
					if (hour != htmlHour) {
						$(hourId).html(hour);
					}
					if (minutes != htmlMinutes) {
						$(minutesId).html(minutes);
					}
					// Clignotement des deux points.
					var $colons = $("#colon-hour");
					if ($colons.css('visibility') == 'hidden') {
						$colons.css('visibility', 'visible');
					} else {
						$colons.css('visibility', 'hidden');
					}
				}, 1000)});
			}

			// Initialize views
			this.unitesExploitationsView = new unitesExploitations.UnitesExploitationsView({
				el: this.$("#unitesExploitation"),
				tsView: this,
			});
			this.servicesVoituresView = new servicesVoitures.ServicesVoituresView({
				el: this.$("#servicesVoitures"),
				tsView: this,
			});
			this.coursesView = new courses.CoursesView({
				el: this.$("#courses"),
				tsView: this,
			});
			this.feuilleDeRouteView= new feuilleDeRoute.FeuilleDeRouteView ({
				el: this.$("#feuilleDeRoute"),
				tsView: this,
			});
			this.ajoutCommuneView= new ajoutCommune.AjoutCommuneView ({
				el: this.$("#ajoutCommune"),
				tsView: this,
			});
			this.ajoutArretView= new ajoutArret.AjoutArretView ({
				el: this.$("#ajoutArret"),
				tsView: this,
			});

			// Start the clock
			var $clock = $('#clock');
			if ($clock.length) {
				getTime("#hours", "#minutes", "#colon-hour");
			}

			// Launch the app
			goHome();
		}
	});

	// Démarrage de l'appli
	window.tsView = new OnboardView({
		model: new Onboard(),
		el: document.body
	});
});