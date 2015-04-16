define([
	"lib/synthese/js/Synthese",
	"lib/bootstrap/js/bootstrap.min",
	"backbone",
], function(Synthese) {

	var AjoutArretView = Backbone.View.extend({
		events: {
			"click .btn-cancel_descent": "cancelDescentClicked",
			"click .btn-info": "createDescentClicked",
			"click #cancelDescent": "deleteDescentClicked",
			"click [data-dismiss='modal']": "cancelDeleteDescentClicked",
		},
		
		initialize: function(options) {
			this.tsView = options.tsView;
			this.tsModel = options.tsView.model;
			this.timeInterval = 3000;
			
			_.bindAll(this, "init", "reloadAndMaybeRefresh", "createDescentClicked", "cancelDescentClicked", "deleteDescentClicked", "cancelDeleteDescentClicked");
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
		
		init: function(sid, comid) {
			console.log("AjoutArretView::init");
			this.comid = comid;
			this.sid = sid;
			var self = this;
			self.reloadAndMaybeRefresh();
			self.autoRefreshTimeOut = setInterval(self.reloadAndMaybeRefresh, self.timeInterval);
		},
		
		writeContent: function(content) {
			$(this.el).find('.content').html(content);
		},
		
		checkContent: function(content) {
			if ($(this.el).find('.content').html() === content) {
				return true;
			}
			else {
				return false;
			}
		},
		
		reloadAndMaybeRefresh: function() {
			console.log("AjoutArretView::init::reloadAndMaybeRefresh");
			var self = this;
			Synthese.callService('page',{
				'p': '17733374504337482',
				'city_id': self.comid,
				'sid': self.sid,
			}).then(function(content){
				if (!self.checkContent(content)) {
					self.writeContent(content);
				}
				self.tsView.hideLoader();
			});
		},
		
		createDescentClicked: function(event) {
			var self = this,
				element = $(event.currentTarget),
				d = new Date(),
				strDate = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate(),
				strHour = d.getHours() + ':' + d.getMinutes(),
				stopId = element.attr('data-stop_id');
			
			if (typeof stopId != 'undefined' && self.sid) {
				clearInterval(self.autoRefreshTimeOut);
				/*datas = {
					a : "ObjectCreate",
					actionParam_table_id: '118',
					actionParam_field_service_id: self.sid,
					actionParam_field_stop_id: element.attr('data-stop_id'),
					actionParam_field_date: strDate,
					actionParam_field_activation_time: strDate + ' ' + strHour,
					actionParam_field_activation_user_id: '7318353689444353',
				};
				$.ajax({
					type: 'GET',
					url: "synthese",
					data: datas
				});*/
				Synthese.callService('page',{
					'p': '17733370209370127',
					'a': 'ObjectCreate',
					'actionParam_table_id': '118',
					'actionParam_field_service_id': self.sid,
					'actionParam_field_stop_id': element.attr('data-stop_id'),
					'actionParam_field_date': strDate,
					'actionParam_field_activation_time': strDate + ' ' + strHour,
					'actionParam_field_activation_user_id': '7318353689444353'
				}).then(function(content){
					console.log('Descente enregistrée');
				});
				self.reloadAndMaybeRefresh();
				self.autoRefreshTimeOut = setInterval(self.reloadAndMaybeRefresh, self.timeInterval);
			}
			return false;
		},
		
		cancelDescentClicked: function(event) {
			console.log('cancelDescentClicked');
			var self = this,
				element = $(event.currentTarget),
				descentId = element.attr('data-descent_id'),
				stopId = element.attr('data-stop_id'),
				modal = $('#cancelDescentModal');
			
			if (typeof descentId != 'undefined' && typeof stopId != 'undefined' && typeof modal != 'undefined') {
				clearInterval(self.autoRefreshTimeOut);
				modal.children('.cancel-descent-btns').children('#cancelDescent').attr('data-descent_id', descentId).attr('data-stop_id', stopId);
				modal.show();
			}
		},
		
		deleteDescentClicked: function(event) {
			console.log('deleteDescentClicked');
			var self = this,
				element = $(event.currentTarget),
				d = new Date(),
				strDate = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate(),
				strHour = d.getHours() + ':' + d.getMinutes(),
				modal = $('#cancelDescentModal'),
				descentId = element.attr('data-descent_id'),
				stopId = element.attr('data-stop_id');
			
			if (typeof descentId != 'undefined' && typeof stopId != 'undefined' && typeof modal != 'undefined') {
				/*datas = {
					a : "ObjectUpdate",
					actionParam_table_id: '118',
					actionParam_field_service_id: self.sid,
					actionParam_field_date: strDate,
					actionParam_field_cancellation_time: strDate + ' ' + strHour,
					actionParam_field_cancellation_user_id: '7318353689444353',
					actionParam_object_id: descentId,
					actionParam_field_stop_id: stopId
				};
				$.ajax({
					type: 'GET',
					url: "synthese",
					data: datas
				});*/
				Synthese.callService('page',{
					'p': '17733370209370127',
					'a': 'ObjectUpdate',
					'actionParam_table_id': '118',
					'actionParam_field_service_id': self.sid,
					'actionParam_field_stop_id': stopId,
					'actionParam_field_date': strDate,
					'actionParam_field_cancellation_time': strDate + ' ' + strHour,
					'actionParam_field_cancellation_user_id': '7318353689444353',
					'actionParam_object_id': descentId
				}).then(function(content){
					console.log('Descente annulée');
				});
				self.$('[data-descent_id=' + descentId + ']').removeClass('btn-success btn-cancel_descent').addClass('btn-info').children('.descent-option').remove();
				modal.hide();
			}
			self.reloadAndMaybeRefresh();
			self.autoRefreshTimeOut = setInterval(self.reloadAndMaybeRefresh, self.timeInterval);
		},
		
		cancelDeleteDescentClicked: function(event) {
			console.log('cancelDeleteDescentClicked');
			var self = this;
			self.reloadAndMaybeRefresh();
			self.autoRefreshTimeOut = setInterval(self.reloadAndMaybeRefresh, self.timeInterval);
		},
	});
	
	return {
		AjoutArretView: AjoutArretView
	};
});