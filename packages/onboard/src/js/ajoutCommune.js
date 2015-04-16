define([
	"lib/synthese/js/Synthese",
	"backbone",
], function(Synthese) {

	var AjoutCommuneView = Backbone.View.extend({
		events: {
			"click .item": "itemClicked",
		},
		
		initialize: function(options) {
			this.tsView = options.tsView;
			this.tsModel = options.tsView.model;
			this.nbRefresh = 0;
			
			_.bindAll(this, "init");
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
		
		init: function(sid) {
			console.log("AjoutCommuneView::init");
			this.sid = sid;
			var self = this;
			
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
				console.log("AjoutCommuneView::init::reloadAndMaybeRefresh");
				Synthese.callService('page',{
					'p': '17733374504337480',
					'sid': self.sid,
				}).then(function(content){
					if (!checkContent(content)) {
						writeContent(content);
					}
					self.nbRefresh++;
					if (self.nbRefresh > 60) {
						// Redirection vers la feuille de route
						clearInterval(self.autoRefreshTimeOut);
						self.tsView.selectCourse(sid);
					}
					self.tsView.hideLoader();
				});
			};
			
			reloadAndMaybeRefresh();
			self.nbRefresh = 0;
			self.autoRefreshTimeOut = setInterval(reloadAndMaybeRefresh,10000);
		},
		
		itemClicked: function(event) {
			var element = $(event.currentTarget);
			// Arrêt de l'autorefresh
			clearInterval(this.autoRefreshTimeOut);
			this.tsView.selectArret(element.attr('id'));
		},
	});
	
	return {
		AjoutCommuneView: AjoutCommuneView
	};
});