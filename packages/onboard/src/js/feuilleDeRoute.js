define([
	"./jquery.timeline.js",
	"lib/synthese/js/Synthese",
	"backbone",
], function (Timeline, Synthese) {

	var FeuilleDeRouteView = Backbone.View.extend({
		events: {
			"click [data-action='close_action']": "closeAllActionsRunning",
			"click #addDescent": "addDescentClicked",
			"click #nextService:not(.disabled)": "nextServiceClicked",
			"click #buttonUp": "buttonUpClicked",
			"click #buttonDown": "buttonDownClicked",
			"click #call-regul,#emergency-regul": "buttonCallRegulClicked",
			"click #overcharge": "buttonOverchargeClicked",
			"click [data-dismiss='modal']": "modalHide",
			"click #confirmActionModal[data-action='call'],#confirmActionModal[data-action='emergency']": "buttonCallRegulFence",
			"click [data-action='close_actions']": "closeAllActionsRunning",
			"click #wait:not(.disabled)": "buttonWaitClicked",
		},

		initialize: function (options) {
			this.tsView = options.tsView;
			this.tsModel = options.tsView.model;
			this.stopsServe = [];
			this.firstIndexToDisplay = 0;
			this.firstIndexTheorique = 0;
			this.nbrStopsToDisplay = 9;
			this.sid = 0;
			this.timelineBreakpoint = 2;
			this.timelineInit();

			// Variables sur le statut du bus.
			this.stopsServe = []; // Liste des arrêts à desservir.
			this.inStopArea = null; // Permet de savoir si le bus est dans la zone d'un arrêt. Utilisé pour le changement du statut du bus.
			this.previousStop = null; // Permet de connaître l'arrêt précédent desservi.

			_.bindAll(this, "tableStack", "nextServiceClicked", "buttonCallRegulClicked", "timelineInit");
		},

		hide: function () {
			$(this.el).hide();
			this.visible = false;
			// Stop auto refresh.
			clearInterval(this.autoRefreshTimeOut);
		},

		show: function () {
			$(this.el).show();
			this.visible = true;
		},

		getTime: function (param) {
			var date = new Date(),
			    strDate = date.getFullYear() + "-" + (date.getMonth() + 1) + "-" + date.getDate(),
			    strHour = date.getHours() + ':' + ((date.getMinutes() < 10) ? '0' + date.getMinutes() : date.getMinutes()) + ':' + ((date.getSeconds() < 10) ? '0' + date.getSeconds() : date.getSeconds());
			if (param == 1) {
				return strDate;
			} else if (param == 2) {
				return strHour;
			}
			return strDate + ' ' + strHour;
		},

		init: function (sid, svid) {
			//console.log("FeuilleDeRouteView::init");
			var self = this;
			this.sid = sid;

			function stopListProcess(sid) {
				var stopsServe = [];
				$.ajaxSetup({async: false});
				var d = new Date();
				var strDate = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate();
				var rqStops = Synthese.callService('service_detail', {
					'roid': sid,
					'read_reservations_from_day': strDate,
					'read_descents_from_day': strDate,
					'output_format': 'json',
				});
				if (!rqStops.status == 200) {return stopsServe;}
				try {
					var serviceDetail = JSON.parse(rqStops.responseText);
				} catch (e) {}
				if (serviceDetail) {
					if (serviceDetail.service_detail) {
						if (serviceDetail.service_detail.stop) {
							serviceDetail.service_detail.stop.forEach(function (stop) {
								// Vérification de présence de réservation.
								stopWithReservation(stop, 'departure');
								stopWithReservation(stop, 'arrival');
								// Initialisation de la montée.
								if (typeof stop.departure_time == 'undefined') {
									stop.type_mounted = 1;
								} else if (stop.departure_time && (typeof stop.with_reservation == 'undefined')) {
									stop.type_mounted = 2;
								} else if (stop.with_departure_reservations) {
									stop.type_mounted = 3;
								}
								// Initialisation de la descente.
								if (typeof stop.arrival_time == 'undefined') {
									stop.type_descent = 1;
								} else if (stop.arrival_time && (typeof stop.with_reservation == 'undefined')) {
									stop.type_descent = 2;
								} else if (stop.with_arrival_reservations) {
									stop.type_descent = 3;
								} else if (stop.with_descent == 1) {
									stop.type_descent = 4;
								}

								// On définit si l'arrêt doit être desservi.
								if ((typeof(stop.type_mounted) != 'undefined' && stop.type_mounted != 1)
								    || (typeof(stop.type_descent) != 'undefined' && stop.type_descent != 1)) {
									stopsServe.push(stop);
								}
							});
						}
					}
				}
				return stopsServe;
			};

			function compareArray(a1, a2) {
				if (!a1 && !a2)
					return true;

				if ((a1 && !a2) || (!a1 && a2))
					return false;

				if (a1.length != a2.length)
					return false;

				for (var i = 0, l = a1.length; i < l; i++) {
					if (a1[i] instanceof Array && a2[i] instanceof Array) {
						// recursif
						if (!compareArray(a1[i], a2[i]))
							return false;
					}
					else if (JSON.stringify(a1[i]) !== JSON.stringify(a2[i])) {
						return false;
					}
				}
				return true;
			};

			function stopWithReservation(stop, type) {
				if (stop['reservation_at_' + type]) {
					var resa_ids = [];
					stop['reservation_at_' + type].forEach(function (resa) {
						// Si la réservation a comme statut 0.
						if (resa.status[0].status == 0 || resa.status[0].status == 19) {
							stop['with_' + type + '_reservations'] = true;
							resa_ids.push(resa.transaction_id);
						}
					});
					if (resa_ids.length) {
						stop[type + '_transactions_ids'] = resa_ids;
					}
				}
			};

			function reloadAndMaybeRefresh() {
				//console.log("FeuilleDeRouteView::init::reloadAndMaybeRefresh");
				var newTabStops = stopListProcess(self.sid);
				if (newTabStops.length > 0) {
					if (compareArray(newTabStops, self.stopsServe)) {
						// les données n'ont pas changé
						// on calcule l'index théorique à afficher, s'il a changé on raffraichit
						var newIndexTheorique = self.calculIndexToDisplayWithVehicleInformation(self.stopsServe, self.inStopArea, self.firstIndexTheorique, self.previousStop);
						if (newIndexTheorique != self.firstIndexTheorique) {
							if (newIndexTheorique > self.stopsServe.length - 9) {
								self.firstIndexToDisplay = self.stopsServe.length - 9;
							}
							else {
								self.firstIndexToDisplay = newIndexTheorique;
							}
							self.firstIndexTheorique = newIndexTheorique;
							$(self.el).find('#table-loader').show();
							$(self.el).find('#stopBus').hide();
							self.removeStops($(self.el).find('#stopBus'));
							self.tableStack(self.stopsServe, self.firstIndexToDisplay, $(self.el).find('#stopBus'),self.firstIndexTheorique);
							$(self.el).find('#table-loader').hide();
							$(self.el).find('#stopBus').show();
						}
					}
					else {
						// La liste des arrêt à desservir a été modifiée on doit faire le refresh selon la logique précédente.
						self.stopsServe = newTabStops;
						self.firstIndexTheorique = self.calculIndexToDisplayWithVehicleInformation(self.stopsServe, self.inStopArea, self.firstIndexTheorique, self.previousStop);
						var nbrToServe = self.stopsServe.length;
						if (nbrToServe >= self.nbrStopsToDisplay && self.firstIndexTheorique > nbrToServe - self.nbrStopsToDisplay) {
							self.firstIndexToDisplay = self.stopsServe.length - 9;
						}
						else {
							self.firstIndexToDisplay = self.firstIndexTheorique;
						}
						//self.firstIndexTheorique = self.firstIndexToDisplay;
						$(self.el).find('#table-loader').show();
						$(self.el).find('#stopBus').hide();
						self.removeStops($(self.el).find('#stopBus'));
						self.tableStack(self.stopsServe, self.firstIndexToDisplay, $(self.el).find('#stopBus'), self.firstIndexTheorique);
						$(self.el).find('#table-loader').hide();
						$(self.el).find('#stopBus').show();
					}
				}
			};

			this.initButtonsCall();

			this.nextServicesInit(sid, svid);
			// On vide le tableau des arrêts.
			this.removeStops($(self.el).find('#stopBus'));

			// On initialise la liste des arrêts à desservir.
			this.stopsServe = stopListProcess(sid);

			// On remplit le tableau des arrêts et on l'affiche.
			this.tableStack(self.stopsServe, this.firstIndexToDisplay, $(self.el).find('#stopBus'), this.firstIndexTheorique);

			// Rafraichissement du tableau toutes les 5 secondes.
			self.autoRefreshTimeOut = setInterval(reloadAndMaybeRefresh, 5000);

			self.tsView.hideLoader();
		},

		// Calcul l'index du prochain stop à afficher en fonction de l'état du véhicule.
		calculIndexToDisplayWithVehicleInformation: function(stops, inStopArea, currentIndex, previousStop) {

			var self = this,
			    indexToReturn = -1;

			// Récupération des informations du véhicule.
			var rqVehicleInformation = Synthese.callService('vehicle_informations', {'output_format': 'json'});
			if (!rqVehicleInformation.status == 200) {return;}
			var vehicleInformation = null;
			try {
				vehicleInformation = JSON.parse(rqVehicleInformation.responseText);
			} catch (e) {}

			// On vérifi que le service retourne les données.
			if (vehicleInformation && vehicleInformation.vehicle_informations) {
				var vehicle = vehicleInformation.vehicle_informations;

				if (vehicle.stop) {
					// On récupère les infos du stop courant.
					var currentStopId = vehicle.stop[0].id,
					    currentPlaceId = vehicle.stop[0].place_id,
					    idsToServe = [];

					// On récupère la liste des identifiant des arrêts à desservir.
					stops.forEach(function(stop, index) {
						if (typeof stop.stop_point_id != 'undefined' && parseInt(stop.stop_point_id) != 0) {
							idsToServe.push(stop.stop_point_id);
						} else {
							idsToServe.push(stop.stop_area_id);
						}
					});


					var stopIndex = idsToServe.indexOf(currentStopId),
					    placeIndex = idsToServe.indexOf(currentPlaceId);

					// Si le stop courant fait parti de la liste des arrêts à desservir, on traite.
					if (currentStopId != -1 && (stopIndex != -1 || placeIndex != -1)) {

						// C'est dans ce if que je ne suis pas sûr de mon travail.
						// Il n'y a pas la prise en compte de l'ancien arrêt pour savoir si on passe d'un TAD a un TAD/arret classique.

						// Si l'arrêt courant est dans une zone TAD.
						if (placeIndex != -1 && parseInt(stops[placeIndex].is_area)) {
							// L'index à retourner est l'index de l'arrêt trouvé grace au currentPlaceId.
							indexToReturn = placeIndex;
							// Changement du previousStop du bus.
							self.previousStop = stops[indexToReturn];
						}
						// Sinon, si le bus sort de la zone d'arrêt d'un arrêt classique.
						// Ou, l'arrêt courant à changé sans changement du stop_in_area.
						else if ( (inStopArea != null && (inStopArea == 1 && parseInt(vehicle.in_stop_area) == 0))
						    ||
						    (inStopArea == parseInt(vehicle.in_stop_area) && (previousStop != null && stopIndex != -1 && previousStop.stop_point_id != stops[stopIndex].stop_point_id)) ) {
							// L'index à retourner est le suivant.
							indexToReturn = stopIndex + 1;
							// Changement du previousStop du bus.
							self.previousStop = stops[indexToReturn];
						}
						// Si le bus passe d'une zone TAD à un arrêt classique.
						else if (previousStop != null && parseInt(previousStop.is_area) && stopIndex != -1 && stops[stopIndex].is_area == 0 ) {
							// L'index à retourner est le stop courant.
							indexToReturn = stopIndex;
						}
						// Si le bus entre dans une zone d'arrêt.
						else if (inStopArea != null && (inStopArea == 0 && parseInt(vehicle.in_stop_area) == 1)) {
							// On ne change pas l'index courant.
							indexToReturn = currentIndex;
						}
						// Sinon, l'index est le même.
						else {
							indexToReturn = currentIndex;
						}

						// Changement du inStopArea du bus.
						self.inStopArea = parseInt(vehicle.in_stop_area);

						// On met à jour le statut du bouton Attendre et de la Timeline.
						if (stopIndex != -1) {
							self.checkWaitAbsence(stops[stopIndex], parseInt(vehicle.in_stop_area));
							self.timelineArrowMove(vehicle.next_stop_found_time.split(" ")[1], stops[stopIndex].departure_time, stops[stopIndex].arrival_time);
						}
						else if (placeIndex != -1) {
							self.checkWaitAbsence(stops[placeIndex], parseInt(vehicle.in_stop_area));
							self.timelineArrowMove(vehicle.next_stop_found_time.split(" ")[1], stops[placeIndex].departure_time, stops[placeIndex].arrival_time);
						}
					}
				}
			}
			if (indexToReturn == -1) {
				indexToReturn = currentIndex;
			}
			return indexToReturn;
		},

		tableStack: function (stops, indexToStart, table, indexTheorique) {
			var self = this;

			if (table.length) {
				var items = [];
				var lastCityName = "";
				var lastRank = -1;
				var firstLineWritten = 0;
				var lastIndexWritten = 0;
				var highLightArea = false;
				stops.forEach(function (stop, index) {
					if (index < indexToStart) {
						// Cet arrêt n'est pas le premier à afficher
						if (firstLineWritten == 0) {
							// on affiche une ligne vide pour montrer qu'on n'est pas en haut du tableau
							var row = '<tr>' +
							    '<td class="min"></td>' +
							    '<td class="max"></td>' +
							    '<td class="stop-name with-city-name"><strong>...</strong></td>' +
							    '<td class="mounted"></td>' +
							    '<td class="descent"></td>' +
							    '</tr>';
							items.push(row);
							firstLineWritten = 1;
						}
					}
					else if (index < indexToStart + 10 - firstLineWritten) {
						// On ajoute une ligne dans le tableau.
						var name = stop.stop_name;
						if (lastCityName != stop.city_name) {
							name += '<span class="city-name">' + stop.city_name + '</span>';
							var withCityName = true;
							lastCityName = stop.city_name;
						}

						var highlight = (indexTheorique == index);
						var startToHighLight = false;
						if (highlight && (stop.is_area === '1')) {
							highLightArea = true;
							startToHighLight = true;
						}
						if (highLightArea && (lastRank == stop.rank)) {
							highlight = true;
						}
						else if (highLightArea && !startToHighLight) {
							highLightArea = false;
						}
						var row = '<tr' + ' id="rank' + stop.rank + '"' + ' data-stop_rank="' + stop.rank +
						    '" data-stop_area_id="' + stop.stop_area_id + '" data-stop_id="' + stop.stop_id + '" ' +
						    (highlight ? ' class="info"' : '') +
						    '>' +
						    '<td class="min">' + self.formatTime(stop.departure_time, highlight) + '</td>' +
						    '<td class="max">' + self.formatTime(stop.arrival_time, highlight) + '</td>' +
						    '<td class="stop-name with-city-name">' + (highlight ? '<strong>' : '') + stop.stop_name + (highlight ? '</strong>' : '') + '<span class="city-name">' + (highlight ? '<strong>' : '') + stop.city_name + (highlight ? '</strong>' : '') + '</span>' + '</td>' +
						    '<td class="mounted">' + self.createMDCell(stop.type_mounted) + '</td>' +
						    '<td class="descent">' + self.createMDCell(stop.type_descent) + '</td>' +
						    '</tr>';

						items.push(row);
						lastIndexWritten = index;
					}
					lastRank = stop.rank;
				});
				if (lastIndexWritten < stops.length - 1) {
					var row = '<tr>' +
					    '<td class="min"></td>' +
					    '<td class="max"></td>' +
					    '<td class="stop-name with-city-name"><strong>...</strong></td>' +
					    '<td class="mounted"></td>' +
					    '<td class="descent"></td>' +
					    '</tr>';
					items.push(row);
				}
				// On remplit le tableau des arrêts à desservir.
				table
				    .children('tbody')
				    .append(items.join(''));
			}
		},

		createMDCell: function (type) {
			if (type == 1) {
				return '<span class="icon icon-ban-circle"></span>';
			} else if (type == 2) {
				return '<span class="icone-mandatory"></span>';
			} else if (type == 3) {
				return '<span class="resa-option">R</span>';
			} else if (type == 4) {
				return '<span class="descent-option">D</span>';
			} else {
				return '';
			}
		},

		formatTime: function (time, bold) {
			if (typeof time != 'undefined')
				return '<span class="small_hour">' + (bold ? '<strong>' : '') + time.substr(0, 2) + (bold ? '</strong>' : '') + '</span>' + (bold ? '<strong>' : '') + time.substr(3, 2) + (bold ? '</strong>' : '');
			return '';
		},

		removeStops: function (table) {
			table.children('tbody').empty();
		},

		nextServicesInit: function (sid, svid) {
			var self = this;
			Synthese.callService('object', {
				'roid': svid,
				'additional_parameters': '1',
				'output_format': 'json',
			}).then(function (content) {
				    if (content.object) {
					    if (content.object.services) {
						    var services = content.object.services.split(','),
							nextServ = services[$.inArray(sid, services) + 1];
						    if (typeof nextServ != 'undefined') {
							    var d = new Date(),
								strDate = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate();
							    $.when(
								    Synthese.callService('services_list', {
									    'roid': nextServ,
									    'display_date': strDate,
									    'output_format': 'json',
								    }),
								    Synthese.callService('PTRouteDetailFunction', {
									    'roid': nextServ,
									    'output_format': 'json',
								    })
								).then(function (servicesList, routeDetail) {
									var htmlString = null;
									if (routeDetail[0].route.direction && '' != routeDetail[0].route.direction && servicesList[0].services.service[0].number && '' != routeDetail[0].route.direction) {
										htmlString = routeDetail[0].route.direction + '&nbsp;-&nbsp;' + servicesList[0].services.service[0].number;
									} else if (routeDetail[0].route.direction && '' != routeDetail[0].route.direction) {
										htmlString = routeDetail[0].route.direction;
									} else if (servicesList[0].services.service[0].number && '' != servicesList[0].services.service[0].number) {
										htmlString = servicesList[0].services.service[0].number;
									}
									var htmlString = htmlString + '<br/>' + servicesList[0].services.service[0].departure_schedule.substr(0, 5);
									var htmlTitle = 'Course suivante';
									self.$('#nextService').attr('data-service_id', nextServ).removeClass('disabled').children('.service-infos').html(htmlString);
									self.$('#nextService').children('#nextServiceTxt').html(htmlTitle);
								});
						    } else {
							    self.$('#nextService').removeAttr('data-service_id').removeClass('disabled');
							    self.$('#nextService').children('#nextServiceTxt').html('Fin de service');
							    self.$('#nextService').children('.service-infos').html('');
						    }
					    }
				    }
			    });
		},

		addDescentClicked: function (event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			// On remonte à la vue parente la page souhaitée
			this.tsView.selectCommune();
		},

		nextServiceDisplay: function (target) {
			var self = this;
			var element = $(self.el).find('#' + target);
			if (typeof element.attr('data-service_id') != 'undefined') {
				this.tsView.selectCourse(element.attr('data-service_id'));
			} else {
				this.tsView.btnBackClicked();
			}
		},

		nextServiceClicked: function (event) {
			var self = this,
			    intersynthese = JSON.parse(self.tsView.readCookie('intersyntheseCall'));
			if (intersynthese) {
				self.modalDisplay('closeAllActions', 'close_actions', 'nextService');
			} else {
				self.nextServiceDisplay('nextService');
			}
		},

		buttonUpClicked: function (event) {
			if (this.firstIndexToDisplay > 0) {
				this.firstIndexToDisplay--;
				this.removeStops($(this.el).find('#stopBus'));
				this.tableStack(this.stopsServe, this.firstIndexToDisplay, $(this.el).find('#stopBus'), this.firstIndexTheorique);
			}
		},

		buttonDownClicked: function (event) {
			if (this.firstIndexToDisplay < this.stopsServe.length - 9) {
				this.firstIndexToDisplay++;
				this.removeStops($(this.el).find('#stopBus'));
				this.tableStack(this.stopsServe, this.firstIndexToDisplay, $(this.el).find('#stopBus'), this.firstIndexTheorique);
			}
		},

		buttonOverchargeClicked: function (event) {
		},

		buttonWaitClicked: function (event) {
			var element = $(event.currentTarget),
			    self = this,
			    rank = element.attr('data-stop_rank'),
			    stopResa = [],

			rqVehicleInformation = Synthese.callService('vehicle_informations', {'output_format': 'json',});
			if (!rqVehicleInformation.status == 200) { return; }
			var vehicleInformation = null;
			try {
				vehicleInformation = JSON.parse(rqVehicleInformation.responseText);
			} catch (e) {}

			var stopAreaId = vehicleInformation.vehicle_informations.stop[0].place_id;

			// Si le bus est bien près d'un arrêt.
			if (typeof stopAreaId != 'undefined') {
				self.stopsServe.forEach(function(stop, index) {
					// Si l'arrêt est trouvé dans la liste des arrêt de la course.
					if (stop.stop_area_id == stopAreaId) {
						var transactions = [];
						// On récupère les réservations de l'arrêt.
						stop.reservation_at_departure.forEach(function (reservation) {
							// If status is OPTION or ACKNOWLEDGED_OPTION.
							if (reservation.status[0].status == 0 || reservation.status[0].status == 1) {
								transactions.push(reservation.transaction_id);
							}
						});
						self.removeTransactions(transactions, element);
					}
				});
			}
		},

		removeTransactions: function (transactions, btn) {
			var self = this;
			transactions.forEach(function (transaction) {
				Synthese.callService('page', {
					'p': '17733370209370127',
					'a': 'cancelreservation',
					'actionParamrt': transaction,
					'absence': 1
				}).then(function (content, status) {
					    console.log('Reservation supprimé');
				    });
			});
			self.modalHide();
			// Le bouton d'absence client redevient inactif.
			btn.removeClass('btn-danger').addClass('disabled')
			    .removeAttr('data-stop_rank')
			    .children('.hour-text')
			    .html('');
			var text = btn.children('.text');
			text.html(text.attr('data-text'));
		},

		initButtonsCall: function () {
			var self = this,
			    intersynthese = JSON.parse(self.tsView.readCookie('intersyntheseCall'));
			if (intersynthese) {
				var priority = intersynthese.priority;
				switch (priority) {
					case 0:
						var btn = $(this.el).find('#call-regul');
						break;
					case 1:
						var btn = $(this.el).find('#emergency-regul');
						break;
				}
				self.buttonCallRegulStatusChange(btn, intersynthese.status, intersynthese.object_id, intersynthese.queue_ids);
				if (intersynthese.status == 'sending') {
					self.interSyntheseCall = setTimeout(function () {
						self.isSynchronisingCallRegul(btn, intersynthese.queue_ids)
					}, 0);
				}
			}
		},

		buttonCallRegulStatusChange: function (btn, state, objectId, queueIds) {
			switch (state) {
				case 'ready':
				default:
					var text = btn.children('.text').attr('data-text');
					btn.attr('data-status', 'receipt')
					    .removeClass('disabled btn-success disabled').addClass('btn-primary')
					    .removeAttr('data-object_id').removeAttr('data-queue_ids')
					    .attr("data-status", "ready")
					    .children('.text').html(text);
					btn.children('.icon-refresh').hide();
					break;
				case 'sending':
					btn.attr('data-status', 'sending')
					    .addClass('disabled').removeClass('btn-primary btn-success')
					    .attr('data-object_id', objectId)
					    .attr('data-queue_ids', queueIds)
					    .children('.text').text("En cours d'envoi");
					btn.children('.icon-refresh').show();
					break;
				case 'receipt':
					var text = btn.children('.text').attr('data-text');
					btn.attr('data-status', 'receipt')
					    .removeClass('disabled').addClass('btn-success')
					    .children('.text').html(text + '<br/>Demande reçue');
					btn.children('.icon-refresh').hide();
					var object_id = btn.attr('data-object_id');
					if (typeof object_id == 'undefined' || object_id == false && typeof objectId != 'undefined') {
						btn.attr('data-object_id', objectId);
					}
					var queue_ids = btn.attr('data-queue_ids');
					if (typeof queue_ids == 'undefined' || queue_ids == false && typeof queueIds != 'undefined') {
						btn.attr('data-queue_ids', queueIds);
					}
					break;
				case 'disable':
					btn.addClass('disabled').removeClass('btn-primary btn-success');
					break;
			}
		},

		buttonCallRegulClicked: function (event) {
			var element = $(event.currentTarget);
			var type = element.attr('data-type');
			if (element.attr('data-status') == 'ready') {
				if (typeof type != 'undefined') {
					var intersynthese = JSON.parse(self.tsView.readCookie('intersyntheseCall'));
					if (intersynthese) {
						var cancelBtn = null;
						if(intersynthese.priority == 0) {
							cancelBtn = $(this.el).find('#call-regul');
						} else if(intersynthese.priority == 1) {
							cancelBtn = $(this.el).find('#emergency-regul');
						}
						this.buttonCallRegulStatusChange(cancelBtn);
						var priority = +!intersynthese.priority;
						this.buttonCallRegulObjectUpdate(element, intersynthese.object_id, intersynthese.queue_ids, priority);
					} else {
						this.buttonCallRegulCreate(element, type, 0);
					}
				} else {
					console.log("The vehicle id isn't initiate");
				}
			} else {
				// afficher popup
				this.modalDisplay('callInterSynthese', type, null);
			}
		},

		buttonCallRegulCreate: function (btn, type, priority) {
			var self = this,
			    rqVehicleInformation = Synthese.callService('vehicle_informations', {
				    'output_format': 'json',
			    });
			if (!rqVehicleInformation.status == 200) {
				return;
			}
			var vehicleInformation = null;
			try {
				vehicleInformation = JSON.parse(rqVehicleInformation.responseText);
			} catch (e) {
			}
			if (vehicleInformation.vehicle_informations.id) {
				var date = new Date(),
				    strDate = date.getFullYear() + "-" + (date.getMonth() + 1) + "-" + date.getDate(),
				    strHour = date.getHours() + ':' + ((date.getMinutes() < 10) ? '0' + date.getMinutes() : date.getMinutes()) + ':' + ((date.getSeconds() < 10) ? '0' + date.getSeconds() : date.getSeconds());

				Synthese.callService('inter_synthese_object_create', {
					'table_id': 119,
					'field_vehicle_id': vehicleInformation.vehicle_informations.id,
					'field_call_time': strDate + ' ' + strHour,
					'field_priority': priority,
					'getqueueid': 1,
					'output_format': 'json',
				}).then(function (content, status) {
					    if (status == "success") {
						    self.buttonCallRegulStatusChange(btn, 'sending', content.object_and_intersynthese_keys.id, content.object_and_intersynthese_keys.queue_ids);
						    var cookie = {
							    'status': 'sending',
							    'priority': priority,
							    'object_id': content.object_and_intersynthese_keys.id,
							    'queue_ids': content.object_and_intersynthese_keys.queue_ids
						    };
						    self.tsView.setCookie('intersyntheseCall', JSON.stringify(cookie));
						    self.interSyntheseCal = setTimeout(function () {
							    self.isSynchronisingCallRegul(btn, content.object_and_intersynthese_keys.queue_ids)
						    }, 0);
					    }
				    });
			}
		},

		buttonCallRegulFence: function (event) {
			var self = this,
			    element = $(event.currentTarget),
			    btn = $('.btn[data-type="' + element.attr('data-action') + '"]');
			self.buttonCallRegulObjectUpdate(btn, btn.attr('data-object_id'), btn.attr('data-queue_ids'), null, this.getTime());
			self.modalHide();
		},

		buttonCallRegulObjectUpdate: function (btn, objectId, queueIds, priority, time) {
			var self = this;
			var options = {'object_id': objectId, 'oldqueueids': queueIds, 'output_format': 'json'};
			if (time != 'undefined' && time != null) {
				options['field_closure_time'] = time;
			}
			if (priority != 'undefined' && priority != null) {
				options['field_priority'] = priority;
			}
			Synthese.callService('inter_synthese_object_update', options)
			    .then(function (content, status) {
				    if (status == "success") {
					    // Stop l'ancien timeout.
					    clearTimeout(self.interSyntheseCall);
					    if (time != 'undefined' && time != null) {
						    // On ferme le bouton.
						    self.buttonCallRegulStatusChange(btn);
						    self.tsView.setCookie('intersyntheseCall', '', 0);
					    } else {
						    self.buttonCallRegulStatusChange(btn, 'sending', objectId, content.intersynthese_keys.queue_ids);
						    var cookie = {
							    'status': 'sending',
							    'priority': priority,
							    'object_id': objectId,
							    'queue_ids': content.intersynthese_keys.queue_ids
						    };
						    self.tsView.setCookie('intersyntheseCall', JSON.stringify(cookie));
						    self.interSyntheseCall = setTimeout(function () {
							    self.isSynchronisingCallRegul(btn, content.intersynthese_keys.queue_ids)
						    }, 0);
					    }
				    }
			    });
		},

		isSynchronisingCallRegul: function (btn, queueIds) {
			var self = this;
			Synthese.callService('is_synchronising', {
				'queue_ids': queueIds,
				'output_format': 'json',
			}).then(function (content, status) {
				    if (status == "success") {
					    if (content.inter_synthese_synchronisation.is_synchronising == 0) {
						    self.buttonCallRegulStatusChange(btn, 'receipt');
						    var cookie = {
							    'status': 'receipt',
							    'priority': (btn.attr('data-type') == 'emergency') ? 1 : 0,
							    'object_id': btn.attr('data-object_id'),
							    'queue_ids': queueIds
						    };
						    self.tsView.setCookie('intersyntheseCall', JSON.stringify(cookie));
					    } else {
						    self.interSyntheseCall = setTimeout(function () {
							    self.isSynchronisingCallRegul(btn, queueIds)
						    }, 4000);
					    }
				    }
			    });
		},

		checkWaitAbsence: function (stop, inStopArea) {
			if (typeof stop.departure_time != 'undefined') {
				var self = this,
				    time = self.getTime(2),
				    btnWait = self.$('#wait');
				if (Date.parse('01/01/2014 ' + time) < Date.parse('01/01/2014 ' + stop.departure_time)) {
					//console.log('EN AVANCE!!');
					btnWait.addClass('btn-danger').addClass('disabled')
					    .removeAttr('data-stop_rank')
					    .children('.hour-text')
					    .html('<sup>' + stop.departure_time.substr(0, 2) + '</sup>' + stop.departure_time.substr(3, 2));
					var text = btnWait.children('.text');
					text.html(text.attr('data-text'));
				} else if (typeof stop.with_departure_reservations != 'undefined' && inStopArea) {
					// S'il y a une réservation et que le bus est encore dans la zone d'arrêt.
					var transaction = null;
					stop.reservation_at_departure.forEach(function (reservation) {
						// If OPTION or ACKNOWLEDGED_OPTION
						if (reservation.status[0].status == 0 || reservation.status[0].status == 1) {
							if (transaction != null) {
								transaction = transaction + ',' + reservation.transaction_id;
							} else {
								transaction = reservation.transaction_id;
							}
						}
					});
					btnWait.removeClass('disabled btn-danger')
					    .attr('data-stop_rank', stop.rank)
					    .children('.text')
					    .html('Absence Client ?');
					btnWait.children('.hour-text').html('');
				} else {
					btnWait.removeClass('btn-danger').addClass('disabled')
					    .removeAttr('data-stop_rank')
					    .children('.hour-text')
					    .html('');
					var text = btnWait.children('.text');
					text.html(text.attr('data-text'));
				}
			}
		},

		timelineInit: function () {
			$('#timeline-three-widget').timelinePlugin({
				statusNumber: 3,
				breakpointSlot: this.timelineBreakpoint,
				status: {
					late: {
						pourcent: 26,
						type: 'warning',
						text: 'Retard',
						textColor: '#000',
						nbParts: 2,
						arrowColor: '#f0ad4e'
					},
					standard: {
						pourcent: 48,
						type: 'neutral',
						text: 'Conforme',
						textColor: '#000',
						nbParts: 3,
						arrowColor: '#000'
					},
					ahead: {
						pourcent: 26,
						type: 'danger',
						text: 'Avance',
						textColor: '#fff',
						nbParts: 2,
						arrowColor: '#d9534f'
					}
				}
			});
			$('#timeline-two-widget').timelinePlugin({
				statusNumber: 3,
				breakpointSlot: $.data(document, "breakpointSlot"),
				status: {
					late: {
						pourcent: 28,
						type: 'warning',
						text: 'Retard',
						textPosition: 25,
						textColor: '#000',
						nbParts: 1,
						arrowColor: '#f0ad4e'
					},
					standard: {
						pourcent: 22,
						type: 'neutral',
						textColor: '#000',
						nbParts: 3,
						arrowColor: '#000'
					},
					ahead: {
						pourcent: 50,
						type: 'danger',
						text: 'Avance',
						textColor: '#fff',
						nbParts: 2,
						arrowColor: '#d9534f'
					}
				}
			});

			$('#timeline-three-widget').css('display', 'none');
			$('#timeline-two-widget').css('display', 'none');
		},

		timelineArrowMove: function (time, departure, arrival) {
			if (departure && arrival && (departure != arrival)) {
				var id = 'timeline-three-widget';
			} else {
				var id = 'timeline-two-widget';
			}
			var timeline = $('#' + id);
			if (timeline.is(':hidden')) {
				timeline.parent().children('.timeline').css('display', 'none');
				timeline.css('display', 'block');
			}
			departure = departure || null;
			arrival = arrival || null;
			timeline.timelinePlugin('updateStatusArrow', time, departure, arrival);
		},

		timelineArrowDisable: function () {
			var timeline = $('#timeline-three-widget');
			if (timeline.is(':hidden')) {
				timeline.parent().children('.timeline').css('display', 'none');
				timeline.css('display', 'block');
			}
			timeline.timelinePlugin('disableArrow');
		},

		modalDisplay: function (identifier, action, target) {
			var self = this,
			    modal = $(self.el).find('#' + identifier);
			modal.parent().append('<div class="modal-backdrop fade in"></div>');
			modal.addClass('in').show().find('#confirmActionModal').attr('data-action', action).attr('data-target', target);
		},

		modalHide: function () {
			$(this.el).find('.modal-backdrop').remove();
			$(this.el).find('.modal').removeClass('in').attr('aria-hidden', true).hide();
		},

		closeAllActionsRunning: function (event) {
			var self = this,
			    element = $(event.currentTarget),
			    target = element.attr('data-target');

			// Annulation de la synchro et suppression du cookie liés aux boutons d'appel régulation.
			clearTimeout(self.interSyntheseCall);
			self.tsView.setCookie('intersyntheseCall', '', 0);
			var btnCall = $(this.el).find('#call-regul');
			if (btnCall.attr('data-status') != 'ready') {
				self.buttonCallRegulObjectUpdate(btnCall, btnCall.attr('data-object_id'), btnCall.attr('data-queue_ids'), null, this.getTime());
			}
			var btnEmergency = $(this.el).find('#emergency-regul');
			if (btnEmergency.attr('data-status') != 'ready') {
				self.buttonCallRegulObjectUpdate(btnEmergency, btnEmergency.attr('data-object_id'), btnEmergency.attr('data-queue_ids'), null, this.getTime());
			}

			// Annulation de la synchro et suppression du cookie liés au bouton de surcharge.
			self.buttonCallRegulStatusChange($(this.el).find('#overcharge'));

			if (typeof target != 'undefined') {
				self.modalHide();
				if (target == 'nextService') {
					self.nextServiceDisplay(target);
				} else if (target == 'zoneBack') {
					self.tsView.btnBackClicked();
				}
			}
			return false;
		}

	});

	return {
		FeuilleDeRouteView: FeuilleDeRouteView
	};
});