function show_current_time()
{
  var now = new Date();
  var trs = $('#reservations tbody tr');
  var lastrow = 0;
  var lastdate = new Date();
  lastdate.setHours(0);
  lastdate.setMinutes(0);
  for(var i=0; i<trs.size(); ++i)
  {
    var hourtxt = trs.eq(i).find('td').eq(2).text().trim();
    if(hourtxt == '') continue;
    var hourdate = new Date(now);
    hourdate.setHours(hourtxt.substr(0,2));
    hourdate.setMinutes(hourtxt.substr(2,2));
    if(hourdate <= now && hourdate > lastdate)
    {
      lastrow = new Number(i);
      lastdate = new Date(hourdate);
    }
    else if(hourdate > now)
    {
      break;
    }
  }
  if(i == trs.size())
  {
    lastrow = i;
    trs.last().children('td').css('border-bottom','5px solid blue');
  }
  else
  {
    trs.eq(lastrow).children('td').css('border-top','5px solid blue');
  }

  var call = 0;
  var cells = '';
  for(; i<trs.size(); ++i)
  {
    var tds = trs.eq(i).children('td');
    if(tds.eq(4).find('.resa').size())
    {
      call = 1;
      cells += tds.eq(6).text() + "M ";
    }
    if(tds.eq(5).find('.resa').size())
    {
      var warn = tds.eq(5).find('.warning').size();
      if(warn)
      {
        cells += '<span class="warning">';
      }
      cells += tds.eq(6).text() + "D";
      if(warn)
      {
        cells += '!</span>';
      }
      cells += ' ';
    }
  }
  
  if(call)
  {
    $('#reading').html('OUI : '+ cells);
  }
  else
  {
    $('#reading').html('NON');
  }
}

$(function(){
  show_current_time();
});