define([
  "core/js/Synthese"
], function(Synthese) {

function zeroFill(number, length) {
  // http://stackoverflow.com/questions/1267283/how-can-i-create-a-zerofilled-value-using-javascript/1267392#1267392
  var result = number.toString();
  var pad = length - result.length;

  while(pad > 0) {
    result = '0' + result;
    pad--;
  }
  return result;
}

var $dateLabels = $(".dateLabel");
var colonVisible = true;

setInterval(function() {
  var now = new Date(); 
  var label = now.getDate() + "/" + (now.getMonth() + 1) + "/" + now.getFullYear() + " "; 
  label += zeroFill(now.getHours(), 2);
  label += "<span>:</span>";
  label += zeroFill(now.getMinutes(), 2);

  $dateLabels.html(label)
             .find("span")
             .css("visibility", colonVisible ? "visible" : "hidden");
  colonVisible = !colonVisible;
}, 500);

});