jQuery.fn.sortElements = (function(){
 
    var sort = [].sort;
 
    return function(comparator, getSortable) {
 
        getSortable = getSortable || function(){return this;};
 
        var placements = this.map(function(){
 
            var sortElement = getSortable.call(this),
                parentNode = sortElement.parentNode,
 
                // Since the element itself will change position, we have
                // to have some way of storing its original position in
                // the DOM. The easiest way is to have a 'flag' node:
                nextSibling = parentNode.insertBefore(
                    document.createTextNode(''),
                    sortElement.nextSibling
                );
 
            return function() {
 
                if (parentNode === this) {
                    throw new Error(
                        "You can't sort elements if any one is a descendant of another."
                    );
                }
 
                // Insert before flag:
                parentNode.insertBefore(this, nextSibling);
                // Remove flag:
                parentNode.removeChild(nextSibling);
 
            };
 
        });
 
        return sort.call(this, comparator).each(function(i){
            placements[i].call(getSortable.call(this));
        });
 
    };
 
})();

function resa_filter_click()
{
    if($('#call_view_link').children('i').first().css('visibility') == 'visible')
    {
      switch_call_view();
    }

  switch_resa_filter();
}

function switch_resa_filter()
{
  var trs = $('#vehicle_journeys tbody tr.summary');
  var resa_filter = $('#resa_filter_link').children('i').first();
  if(resa_filter.css('visibility') == 'visible')
  {
    trs.each(function(){
      $(this).css('display','');
    });
    resa_filter.css('visibility','hidden');
  }
  else
  {
    trs.each(function(){
      $(this).css('display', ($(this).children().eq(2).html() != '') ? '' : 'none');
    });
    resa_filter.css('visibility','visible');
  }
  return false;
}

function call_view_click()
{
    if($('#resa_filter_link').children('i').first().css('visibility') == 'visible')
    {
      switch_resa_filter();
    }

  switch_call_view();
}

function switch_call_view()
{
  var trs = $('#vehicle_journeys tbody tr');
  var call_view = $('#call_view_link').children('i').first();
  if(call_view.css('visibility') == 'visible')
  {
    $('.detail').css('display','none');
    $('.summary').css('display','');
    call_view.css('visibility','hidden');
    sort_time();
  }
  else
  {
    $('.detail').css('display','');
    $('.summary').css('display','none');
    sort_details();
    call_view.css('visibility','visible');
  }
  return false;
}

function sort_details()
{
  $('#vehicle_journeys tbody tr').sortElements(function(a, b){
    return ($(a).children().eq(0).text() > $(b).children().eq(0).text()) ? 1 : -1;
  });
}

function sort_time()
{
  $('#vehicle_journeys tbody tr').sortElements(function(a, b){
    return ($(a).children().eq(3).text() > $(b).children().eq(3).text()) ? 1 : -1;
  });
}

$(function(){
  $('#resa_filter_link').click(resa_filter_click);
  $('#call_view_link').click(call_view_click);
});