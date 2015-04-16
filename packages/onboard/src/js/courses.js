define([
	"lib/synthese/js/Synthese",
	"backbone",
], function(Synthese) {

	var CoursesView = Backbone.View.extend({
		events: {
			"click .item": "itemClicked",
			"click #buttonUp": "buttonUpClicked",
			"click #buttonDown": "buttonDownClicked",
			"click .clickable-row": "courseClicked",
			"click .btn-shortcuts": "shortcutClicked",
		},
		
		initialize: function(options) {
			this.tsView = options.tsView;
			this.tsModel = options.tsView.model;
			this.tabCourses = [];
			this.firstIndexToDisplay = 0;
			this.firstIndexToHighLight = 0;
			this.firstIndexTheorique = 0;
			this.nbPauses = 0;
		},
		
		hide: function() {
			$(this.el).hide();
			this.visible = false;
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
		},
		
		show: function() {
			$(this.el).show();
			this.visible = true;
		},
		
		init: function(id) {
			console.log("CoursesView::init");
			var self=this;
			
			function reloadCourses(callback) {
				console.log("CoursesView::init::reloadCourses");
				// tableaux des courses :
				var tabCourses = [];
				
				// On récupère les infos du service véhicule
				Synthese.callService('VehicleServicesList',{
					'roid': id,
					'output_format': 'json',
				}).then(function(content){
					if (!content.vehicleServices) {
						return tabCourses;
					}
					if (!content.vehicleServices.vehicleService) {
						return tabCourses;
					}
					$.ajaxSetup({async: false});
					content.vehicleServices.vehicleService.forEach(function(vs){
						vs.service.forEach(function(service){
							if (service['class'] === "deadRun") {
								var rqDeadRun = Synthese.callService('object',{
									'roid': service.id,
									'output_format': 'json',
								});
								if (!rqDeadRun.status == 200) {return;}
								try {
									var deadRun = JSON.parse(rqDeadRun.responseText);
									tabCourses[tabCourses.length] = deadRun;
								} catch (e) {}
							}
							else {
								var d = new Date();
								var strDate = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate();
								var rqService = Synthese.callService('services_list',{
									'roid': service.id,
									'display_date': strDate,
									'output_format': 'json',
								});
								if (!rqService.status == 200) {return;}
								try {
									var service = JSON.parse(rqService.responseText);
									console.log(service);
									tabCourses[tabCourses.length] = service;
								} catch (e) {}
								var rqRoute = Synthese.callService('PTRouteDetailFunction',{
									'roid': tabCourses[tabCourses.length-1].services.service[0].id,
									'output_format': 'json',
								});
								if (!rqRoute.status == 200) {return;}
								var route = JSON.parse(rqRoute.responseText);
								tabCourses[tabCourses.length-1].services.service[0].direction = route.route.direction || ' ';
							}
						});
					});
					$.ajaxSetup({async: true});
					// Appel du callback
					callback(tabCourses);
				});
			};
			
			function compareArray(a1,a2) {
				if (!a1 && !a2)
					return true;
				
				if ((a1 && !a2) || (!a1 && a2))
					return false;
				
				if (a1.length != a2.length)
					return false;
				
				for (var i = 0, l=a1.length; i < l; i++) {
					if (a1[i] instanceof Array && a2[i] instanceof Array) {
						// recursif
						if (!compareArray(a1[i],a2[i]))
							return false;
					}
					else if (JSON.stringify(a1[i]) !== JSON.stringify(a2[i])){
						return false;
					}
				}
				return true;
			};
			
			function reloadAndMaybeRefresh() {
				console.log('CoursesView::init::reloadAndMaybeRefresh');
				
				function maybeRefresh(newTabCourses) {
					if (newTabCourses.length > 0) {
						if (compareArray(newTabCourses,self.tabCourses)){
							// les données n'ont pas changé
							// on calcule l'index théorique à afficher, s'il a changé on raffraichit
							var newIndexTheorique = self.calculIndexToDisplayWithDate(self.tabCourses);
							if (newIndexTheorique != self.firstIndexTheorique) {
								self.firstIndexToDisplay = newIndexTheorique;
								self.firstIndexTheorique = newIndexTheorique;
								self.firstIndexToHighLight = self.calculIndexToHighLightWithDate(self.tabCourses);
								$(self.el).find('#services').hide();
								self.removeServices($(self.el).find('#services'));
								self.createShortcuts(self.tabCourses, self.firstIndexToDisplay, $(self.el).find('#shortcuts'), self);
								self.addServices(self.tabCourses, self.firstIndexToDisplay, self.firstIndexToHighLight, $(self.el).find('#services'), self);
								$(self.el).find('#services').show();
							}
						}
						else {
							// on doit faire le refresh
							self.tabCourses = newTabCourses;
							self.firstIndexToDisplay = self.calculIndexToDisplayWithDate(self.tabCourses);
							self.firstIndexToHighLight = self.calculIndexToHighLightWithDate(self.tabCourses);
							self.firstIndexTheorique = self.firstIndexToDisplay;
							$(self.el).find('#services').hide();
							self.removeServices($(self.el).find('#services'));
							self.createShortcuts(self.tabCourses, self.firstIndexToDisplay, $(self.el).find('#shortcuts'), self);
							self.addServices(self.tabCourses, self.firstIndexToDisplay, self.firstIndexToHighLight, $(self.el).find('#services'), self);
							$(self.el).find('#services').show();
						}
					}
				}
				
				reloadCourses(maybeRefresh);
			};
			
			function displayFirstTime(tabCourses) {
				self.firstIndexToDisplay = self.calculIndexToDisplayWithDate(tabCourses);
				self.firstIndexToHighLight = self.calculIndexToHighLightWithDate(tabCourses);
				self.firstIndexTheorique = self.firstIndexToDisplay;
				self.removeServices($(self.el).find('#services'));
				self.createShortcuts(tabCourses, self.firstIndexToDisplay, $(self.el).find('#shortcuts'),self);
				self.addServices(tabCourses, self.firstIndexToDisplay, self.firstIndexToHighLight, $(self.el).find('#services'),self);
				self.tabCourses = tabCourses;
				self.tsView.hideLoader();
			};
			
			reloadCourses(displayFirstTime);
			self.autoRefreshTimeOut = setInterval(reloadAndMaybeRefresh,60000);
		},
		
		calculIndexToDisplayWithDate: function(services) {
			// Calcul l'index de la prochaine course en fonction de l'heure courante
			var date = new Date();
			var currentIndex = 0;
			var indexToReturn = -1;
			
			services.forEach(function(service) {
				var departure_schedule;
				if (service.services) {
					departure_schedule = service.services.service[0].departure_schedule;
				}
				if (service.object) {
					departure_schedule = service.object.departure_schedule;
				}
				var heure = departure_schedule.substr(0,2);
				var minute = departure_schedule.substr(3,2);
				var seconde = departure_schedule.substr(6,2);
				var dateService = new Date();
				dateService.setHours(heure);
				dateService.setMinutes(minute);
				dateService.setSeconds(seconde);
				if (date < dateService) {
					if (indexToReturn == -1) {
						indexToReturn = currentIndex;
					}
				}
				currentIndex++;
			});
			
			if (indexToReturn > 0) {
				indexToReturn--;
			}
			if (indexToReturn == -1) {
				indexToReturn = services.length -1;
			}
			return indexToReturn;
		},
		
		calculIndexToHighLightWithDate: function(services) {
			// Calcul l'index de la prochaine course en fonction de l'heure courante
			var date = new Date();
			var currentIndex = 0;
			var indexToReturn = -1;
			
			services.forEach(function(service) {
				var departure_schedule;
				if (service.services) {
					departure_schedule = service.services.service[0].departure_schedule;
				}
				if (service.object) {
					departure_schedule = service.object.departure_schedule;
				}
				var heure = departure_schedule.substr(0,2);
				var minute = departure_schedule.substr(3,2);
				var seconde = departure_schedule.substr(6,2);
				var dateService = new Date();
				dateService.setHours(heure);
				dateService.setMinutes(minute);
				dateService.setSeconds(seconde);
				if (date < dateService) {
					if (indexToReturn == -1) {
						indexToReturn = currentIndex;
					}
				}
				currentIndex++;
			});
			if (indexToReturn == -1) {
				indexToReturn = services.length -1;
			}
			
			return indexToReturn;
		},
		
		addServices: function(services, index, indexToHighLight, table, self) {
			var items = [],
				lastArrivalSchedule = null,
				currentIndex = 0,
				nbPauses = 0,
				lastIndex = 0;
			var firstLineWritten = 0;
			
			services.forEach(function(service) {
				var departure_schedule,
					arrival_schedule;
				if (service.services) {
					departure_schedule = service.services.service[0].departure_schedule;
					arrival_schedule = service.services.service[0].arrival_schedule;
				}
				if (service.object) {
					departure_schedule = service.object.departure_schedule;
					arrival_schedule = service.object.arrival_schedule;
				}
				if (currentIndex < index) {
					// Ce service n'est pas à afficher
					lastArrivalSchedule = arrival_schedule;
					if (firstLineWritten == 0) {
						// on affiche une ligne vide pour montrer qu'on n'est pas en haut du tableau
						var row = '<tr>' +
							'<td><strong>...</strong></td>' +
							'<td><strong>...</strong></td>' +
							'<td><strong>...</strong></td>' +
							'<td><strong>...</strong></td>' +
							'</tr>';
						items.push(row);
						firstLineWritten = 1;
					}
				}
				else if (currentIndex < index + 10 - 1 - nbPauses)
				{
					if (lastArrivalSchedule) {
						if (lastArrivalSchedule < departure_schedule) {
							// Création d'une ligne de pause
							items.push(self.createPauseRow(service, lastArrivalSchedule));
							nbPauses++;
						}
						lastArrivalSchedule = arrival_schedule;
					}
					else {
						lastArrivalSchedule = arrival_schedule;
					}
					if (currentIndex < index + 10 - 1 - nbPauses) {
						if (currentIndex == indexToHighLight) {
							items.push(self.createServiceRow(service, true));
						}
						else {
							items.push(self.createServiceRow(service, false));
						}
					}
					lastIndex = currentIndex;
				}
				currentIndex++;
			});
			
			// Si on est à la fin des courses, on insère une nouvelle ligne de pause
			if (lastIndex == services.length) {
				items.push(self.createPauseRow(services[services.length-1], lastArrivalSchedule));
			}
			if (lastIndex < currentIndex - 1) {
				var row = '<tr>' +
					'<td><strong>...</strong></td>' +
					'<td><strong>...</strong></td>' +
					'<td><strong>...</strong></td>' +
					'<td><strong>...</strong></td>' +
					'</tr>';
				items.push(row);
			}
			
			table.children('tbody')
				.append(items.join(''))
				.children('tr').not('[data-type="pause"],[data-type="deadRun"]');
		},
		
		createShortcuts: function(services, index, div, self) {
			
			function getTextLink(direction, number, departure_schedule) {
				var htmlString = null;
				if (typeof direction !== 'undefined' && typeof number !== 'undefined') {
					htmlString = direction + '&nbsp;-&nbsp;' + number;
				} else if (typeof direction !== 'undefined') {
					htmlString = direction ;
				} else if (typeof direction !== 'undefined') {
					htmlString = number;
				}
				return htmlString + '<br/>' + departure_schedule.substr(0,5);
			};
			
			var currentService = services[index];
			var indexCopy = index;
			// On remonte au précédent service en cas de deadrun à cet instant
			while (!currentService.services && indexCopy > 0){
				currentService = services[indexCopy-1];
				indexCopy--;
			}
			var nextService = null;
			if (services.length > index + 1){
				nextService = services[index+1];
			}
			// On descend jusqu'au prochain service en cas de deadrun juste après
			indexCopy = index;
			while (nextService && !nextService.services && indexCopy < services.length - 1){
				nextService = services[indexCopy+1];
				indexCopy++;
			}
			var currentId,
				currentDirection,
				currentNumber,
				currentText,
				currentDeparture_schedule,
				nextId,
				nextDirection,
				nextNumber,
				nextText,
				nextDeparture_schedule;
			if (currentService.services) {
				currentId = currentService.services.service[0].id;
				currentNumber = currentService.services.service[0].number;
				currentDeparture_schedule = currentService.services.service[0].departure_schedule;
				currentDirection = currentService.services.service[0].direction;
			}
			div.children('.shortcuts-inner').empty();
			falseShortLeft = $('<a>',{
				'html': '&nbsp;',
				'href': '#',
				'class': 'btn btn-shortcuts btn-shortcuts-false btn-primary'
			});
			falseShortRight = $('<a>',{
				'html': '&nbsp;',
				'href': '#',
				'class': 'btn btn-shortcuts btn-shortcuts-false btn-primary'
			});
			div.children('.shortcuts-inner').append(falseShortLeft);
			div.children('.shortcuts-inner').append(falseShortRight);
			var shortcuts = div.children('.shortcuts-inner').children();
			if (nextService == null) {
				shortcuts.last().html('&nbsp;').attr('href', '#')
					.attr('class', 'btn-shortcuts-false');
			} else {
				if (self.firstIndexToHighLight == 0) {
					shortcuts.last().html(getTextLink(currentDirection, currentNumber, currentDeparture_schedule))
						.attr("data-roid", currentId)
						.attr("href", '#');
				} else if (nextService.services) {
					nextId = nextService.services.service[0].id;
					nextNumber = nextService.services.service[0].number;
					nextDeparture_schedule = nextService.services.service[0].departure_schedule;
					nextDirection = nextService.services.service[0].direction;
					shortcuts.last().html(getTextLink(nextDirection, nextNumber, nextDeparture_schedule))
						.attr("data-roid", nextId)
						.attr("href", '#');
				}
			}
			if (self.firstIndexToHighLight == 0) {
				shortcuts.first().html('&nbsp;').attr('href', '#')
					.attr('class', 'btn-shortcuts-false');
			} else {
				shortcuts.first()
					.html(getTextLink(currentDirection, currentNumber, currentDeparture_schedule))
					.attr("data-roid", currentId)
					.attr("href", '#')
					.removeClass("btn-shortcuts-false");
			}
		},
		
		createPauseRow: function(service, departureTime) {
			var id,
				departure_place_name;
			if (service.services) {
				id = service.services.service[0].id;
				departure_place_name = service.services.service[0].departure_place_name;
			}
			// If service is a deadrun.
			if (service.object) {
				id = service.object.id;
				departure_place_name = service.object.departure_place_name;
			}
			return '<tr id="' + id + '-p" data-type="pause">' +
				'<td>' + departureTime.substr(0,5) + '</td>' +
				'<td>' + departure_place_name + '</td> '+
				'<td><i class="icon icon-pause" title="Haut-le-pied"></i></td>' +
				'<td></td>' +
				'</tr>';
		},
		
		createServiceRow: function(service, nextService) {
			var classes = null,
				direction,
				id,
				departure_schedule,
				type,
				departure_place_name,
				number;
			
			// If class service is commercial.
			if (service.services) {
				if (nextService) {
					classes = "clickable-row info";
				}
				else {
					classes = "clickable-row";
				}
				id = service.services.service[0].id;
				departure_schedule = service.services.service[0].departure_schedule;
				type = 'commercial';
				departure_place_name = service.services.service[0].departure_place_name;
				number = service.services.service[0].number;
				direction = service.services.service[0].direction;
			}
			// If service is a deadrun.
			if (service.object) {
				direction = 'Wxxx';
				id = service.object.id;
				departure_schedule = service.object.departure_schedule;
				type = 'deadRun';
				departure_place_name = service.object.departure_place_name;
				number =service.object.service_number;
			}
			
			var row = '<tr id="' + id + '" data-departure_schedule="' + departure_schedule
				+ '" sid="' + id + '" data-type="' + type;
			if (classes !== null) {
				row += '" class="' + classes;
			}
			row += '">' + '<td class="departure">'+ departure_schedule.substr(0,5) + '</td>' +
				'<td class="place">'+ departure_place_name + '</td>' +
				'<td class="direction">'+ direction + '</td>' +
				'<td class="course"><span class="btn btn-primary btn-service">'+ number + '</span></td>' +
				'</tr>';
			
			return row;
		},
		
		removeServices: function(table) {
			table.children('tbody').empty();
		},
		
		itemClicked: function(event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			// On remonte à la vue parente la page souhaitée
			this.tsView.selectCourse(element.attr('id'));
		},
		
		courseClicked: function(event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			// On remonte à la vue parente la page souhaitée
			this.tsView.selectCourse(element.attr('id'));
		},
		
		shortcutClicked: function(event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			// On remonte à la vue parente la page souhaitée
			this.tsView.selectCourse(element.attr('data-roid'));
		},
		
		buttonUpClicked: function(event) {
			if (this.firstIndexToDisplay > 0) {
				this.firstIndexToDisplay--;
				$(this.el).find('#services').hide();
				this.removeServices($(this.el).find('#services'));
				this.addServices(this.tabCourses, this.firstIndexToDisplay, this.firstIndexToHighLight, $(this.el).find('#services'),this, false);
				$(this.el).find('#services').show();
			}
		},
		
		buttonDownClicked: function(event) {
			if (this.firstIndexToDisplay < this.tabCourses.length - 9) { 
				this.firstIndexToDisplay++;
				$(this.el).find('#services').hide();
				this.removeServices($(this.el).find('#services'));
				this.addServices(this.tabCourses, this.firstIndexToDisplay, this.firstIndexToHighLight, $(this.el).find('#services'),this, false);
				$(this.el).find('#services').show();
			}
		},
	});
	
	return {
		CoursesView: CoursesView
	};
});