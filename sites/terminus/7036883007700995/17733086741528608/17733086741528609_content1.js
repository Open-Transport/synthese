var formToSave = false;

function openclose()
{
 var div = $(this).siblings('div');
 var status = div.css('display');
 div.css('display',status=='none'?'block':'none');
 if($(this).hasClass('icon-chevron-right'))
 {
   $(this).addClass('icon-chevron-down');
   $(this).removeClass('icon-chevron-right');
 }
 else
 {
   $(this).addClass('icon-chevron-right');
   $(this).removeClass('icon-chevron-down');
 }
}


function filter()
{
  var text=$(this).val();
  var elements=$(this).next().find('label');
  if(text != '')
  {
    elements.each(function(){
      if($(this).text().indexOf(text) !== -1)
      {
        $(this).parent().addClass('search-visible');
      }
      else
      {
        $(this).parent().addClass('search-invisible');
      }
    });
  }
  else
  {
    elements.each(function(){
      $(this).parent().removeClass('search-visible');
      $(this).parent().removeClass('search-invisible');
    });
  }
}

function change_recipient()
{
  var recipient = $(this).attr('factory');
  update_objects_field(recipient);
  update_object_preview(recipient, current_message_rank);
  activateForm();
}

$(function(){
  $('.openclose').click(openclose);
  $('.search-query').keyup(filter);
  $('input[factory]').change(change_recipient);
});

function show_objects(recipient)
{
  // Clean
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    $(this).get(0).checked = false;
  });
  
  var objects = document.getElementById('input_'+ current_message_rank +'_recipients_'+ recipient).value;
  if(objects)
  {
    var parts = objects.split(',');
    for(j=0;j<parts.length;++j)
    {
      var params = parts[j].split('|');
      if(params.length>1)
      {
        $('input[factory="'+recipient+'"][value="'+ params[0] +'"][parameter="'+ params[1] +'"]').get(0).checked=true;
      }
      else
      {
        $('input[factory="'+recipient+'"][value="'+ params[0] +'"][noparam]').get(0).checked=true;
      }
    }
  }
  $('#m_'+ recipient).modal();
}

function update_object_preview(recipient, message_rank)
{
  var available_recipients = $('input[factory="'+ recipient +'"]');
  var objects = document.getElementById('input_'+ message_rank +'_recipients_'+ recipient).value;
  if(objects)
  {
    var parts = objects.split(',');
    var s='';
    for(j=0;j<parts.length;++j)
    {
      if(s.length) s += ", ";
      var params = parts[j].split('|');
      if(params.length>1)
      {
        s += $('input[factory="'+recipient+'"][value="'+ params[0] +'"][parameter="'+ params[1] +'"]').closest('label').text();
      }
      else
      {
        s += $('input[factory="'+recipient+'"][value="'+ params[0] +'"][noparam]').closest('label').text();
      }
    }
  }
  else
  {
    s = "Aucun";
  }
  $('#preview_'+ message_rank+'_recipients_'+ recipient).html(s);
}

function update_objects_field(recipient)
{
  var s='';
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    if($(this).get(0).checked)
    {
      if(s.length) s += ',';
      s += $(this).attr('value');
      if($(this).attr('parameter'))
      {
        s += '|' + $(this).attr('parameter');
      }
    }
  });
  document.getElementById('input_'+ current_message_rank +'_recipients_'+ recipient).value=s;
}

function activateForm(field)
{
  formToSave = true;
  $('#record_button').addClass('btn-primary');
  return true;
}