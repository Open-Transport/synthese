var NewsTickerView = Backbone.View.extend({

  events: {
    "click .prevLink": "prevLinkClick",
    "click .nextLink": "nextLinkClick"
  },

  REFRESH_DELAY_MS: 3000,

  initialize: function(options) {
    var itemsPerGroup = options.itemsPerGroup || 3;

    var groups = [[]];
    this.$("li").each(function(index, li) {
      var lastGroup = groups[groups.length - 1];
      if (lastGroup.length == itemsPerGroup) {
        lastGroup = [];
        groups[groups.length] = lastGroup;
      }
      lastGroup.push(li);
    });

    // Nothing to do with only one group.
    if (groups.length == 1) {
      this.$(".navigation").hide();
      return;
    }

    var $itemsContainer = this.$(".itemsContainer");
    $itemsContainer.empty();

    this.ulGroups = _.map(groups, function(group, index) {
      var $ul = $("<ul></ul>");
      _.each(group, function(li) {
        $(li).appendTo($ul);
      })
      $ul.appendTo($itemsContainer);
      return $ul;
    });

    this.goToIndex(0);
    this.$(".total").text(this.ulGroups.length);

    $(this.el).show();
    this.resetInterval();
  },

  goToIndex: function(index) {
    if (this.index !== undefined)
      this.ulGroups[this.index].fadeOut();
    this.ulGroups[index].fadeIn();
    this.index = index;
    this.$(".curIndex").text(this.index + 1);
  },

  moveIndex: function(offset) {
    this.goToIndex((this.index + offset + this.ulGroups.length) % this.ulGroups.length);
  },

  resetInterval: function() {
    if (this.interval)
      clearInterval(this.interval);
    var self = this;
    this.interval = setInterval(function() {
      self.moveIndex(1);
    }, this.REFRESH_DELAY_MS);
  },

  prevLinkClick: function(event) {
    event.stopPropagation();
    event.preventDefault();
    this.resetInterval();
    this.moveIndex(-1);
  },

  nextLinkClick: function(event) {
    event.stopPropagation();
    event.preventDefault();
    this.resetInterval();
    this.moveIndex(1);
  }
});
