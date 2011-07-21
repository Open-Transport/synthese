if (!window.Synthese)
  window.Synthese = {};

Synthese.ieFix = function() {
  // Workaround for IE 7 and below.
  // It does two things:
  // 1) Fix IE overflow (Based on script from Remy Sharp http://remysharp.com/2008/01/21/fixing-ie-overflow-problem/)
  // 2) Adds a class of "ie7OrLess" to the body so that we can hide the tables used
  //    for keeping the header column fixed on the left. (because IE7 doesn't support
  //    setting cell width unless the table is in fixed layout mode).
  if (!window.attachEvent || (document.documentMode && document.documentMode > 7))
    return;

  document.body.className = (document.body.className ? " " : "") + "ie7OrLess";

  // find every element to test
  var all = document.getElementsByTagName('*'), i = all.length;
  
  // fast reverse loop
  while (i--) {
    var el = all[i];
    // if the scrollWidth (the real width) is greater than
    // the visible width, then apply style changes
    if (el.scrollWidth > el.offsetWidth) {
      el.style['paddingBottom'] = '20px';
      el.style['overflowY'] = 'hidden';
    }
  }
}
