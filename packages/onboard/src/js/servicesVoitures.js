define([
	"lib/synthese/js/Synthese",
	"backbone",
], function(Synthese) {
	
	var ServicesVoituresView = Backbone.View.extend({
		events: {
			"click .item": "itemClicked",
		},
		
		initialize: function(options) {
			this.tsView = options.tsView;
			this.tsModel = options.tsView.model;
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
		
		init: function(idUniteExploitation) {
			console.log("ServicesVoituresView::init");
			var self=this;
			
			function writeContent(content) {
				$(self.el).find('.content').html(content);
			};
			
			function checkContent(content) {
				if ($(self.el).find('.content').html() === content) {
					return true;
				}
				else {
					return false;
				}
			};
			
			function reloadAndMaybeRefresh() {
				console.log("UnitesExploitationsView::init::reloadAndMaybeRefresh");
				Synthese.callService('page',{
					'p': '17733374504337468',
					'unid': idUniteExploitation,
				}).then(function(content) {
					if (!checkContent(content)) {
						writeContent(content);
					}
					self.tsView.hideLoader();
				});
			}
			
			reloadAndMaybeRefresh();
			self.autoRefreshTimeOut = setInterval(reloadAndMaybeRefresh,10000);
		},
		
		itemClicked: function(event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			// On remonte à la vue parente la page souhaitée
			this.tsView.selectServiceVoiture(element.attr('id'));
		},
	});
	
	return {
		ServicesVoituresView: ServicesVoituresView
	};
});