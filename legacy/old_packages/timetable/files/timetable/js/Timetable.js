define([
  "core/js/Synthese",
  "backbone",
  "timetable/vendor/epoch/epoch_classes"
], function(Synthese) {

var TimetableView = Backbone.View.extend({

  events: {
    "click .ttnote": "noteClick",
    "click .caltitleclose": "noteCloseClick"
  },

  initialize: function() {
    this.buildCalendars();
  },

  buildCalendars: function() {
    var self = this;
    this.$(".cal-data").each(function(index, calendarScript) {
      self.buildCalendar(
          calendarScript.parentNode,
          jQuery.parseJSON($(calendarScript).text()));
    });
  },

  buildCalendar: function(targetElement, calendarData) {
    var calId = "cal_" + parseInt(Math.random() * 10000);
    function toDate(dateArray) {
      return new Date(dateArray[0], dateArray[1] - 1, dateArray[2]);
    }

    var calendar = new Epoch(calId, "flat", targetElement, true,
        toDate(calendarData.start),
        toDate(calendarData.end));
    var dates = [];
    _.each(calendarData.days, function(day) {
      if (day === null)
        return;
      var date = toDate(day.date);
      date.selected = day.selected;
      dates.push(date);
    }, this);
    calendar.addDates(dates);
  },

  getCalendarDiv: function(number) {
    return this.$(".caldiv-" + number).get(0);
  },

  showCalendar: function(number) {
    var cd=this.getCalendarDiv(number)
    console.log(cd)
    $(this.getCalendarDiv(number)).show();
  },

  hideCalendars: function() {
    this.$(".caldiv").hide();
  },

  noteClick: function(event) {
    this.showCalendar($(event.target).data("number"));
  },

  noteCloseClick: function(event) {
    this.hideCalendars();
  }
});


// Initialize all timetables on the page
$(function() {
  $(".timetable").each(function(index, timetableDiv) {

    new TimetableView({
      el: timetableDiv
    });

  });
})

return {};

});
