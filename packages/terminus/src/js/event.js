var formToSave = false;
var messagesWhichHaveBeenClosed = [];

function openclose()
{
 var div = $(this).siblings('div');
 activate_node($(this), div.hasClass('hide'));
}

function activate_node(icon_selector, value)
{
 var div = icon_selector.siblings('div');
 if(value)
 {
   div.removeClass('hide');
   icon_selector.addClass('icon-chevron-down');
   icon_selector.removeClass('icon-chevron-right');
 }
 else
 {
   icon_selector.addClass('icon-chevron-right');
   icon_selector.removeClass('icon-chevron-down');
   div.addClass('hide');
 }
}


function filter()
{
  var text=$(this).val().toUpperCase();
  var elements=$(this).parent().find('label');
  if(text != '')
  {
    elements.each(function(){
      var txt = $(this).text().toUpperCase();
      $(this).parent().removeClass('search-visible');
      $(this).parent().removeClass('search-invisible');
      if(txt.indexOf(text) !== -1)
      {
        $(this).parentsUntil('div.modal_body').addClass('search-visible');
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
      var parent = $(this).parentsUntil('div.modal_body');
      parent.removeClass('search-visible');
      parent.removeClass('search-invisible');
    });
  }
}

function change_recipient()
{
  var recipient = $(this).attr('factory');
  update_objects_field(recipient, current_message);
  update_object_preview(recipient, current_message);
  activateForm();
  $('#old_recipients_alert').removeClass('hide');
}

function show_recipients_click()
{
  var recipient = $(this).attr('factory');
  show_objects(recipient, current_message);
  return false;
}

function show_objects(recipient, message)
{
  // Clean
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    $(this).get(0).checked = false;
    $(this).removeAttr('link_id');
  });
  
  var links = message[recipient +'_recipient'];
  for(var i=0; i<links.length; ++i)
  {
    var link = links[i];
    if(link.parameter)
    {
      var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][parameter="'+ link.parameter +'"]');
      input.get(0).checked=true;
      input.attr('link_id', link.id);
      input.parentsUntil('.modal_body').each(function(){activate_node($(this).children('.openclose'),true);});
    }
    else
    {
      var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
      input.get(0).checked=true;
      input.attr('link_id', link.id);
      input.parentsUntil('.modal_body').each(function(){activate_node($(this).children('.openclose'),true);});
    }
  }
  $('#m_'+ recipient).modal();
}

function update_object_preview(recipient, message)
{
  var available_recipients = $('input[factory="'+ recipient +'"]');
  var links = message[recipient +'_recipient'];
  var s='';
  if(links.length)
  {
    for(var i=0; i<links.length; ++i)
    {
      var link = links[i];
      if(s.length)
      {
        s += '<br />';
      }
      if(link.parameter)
      {
        s += $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][parameter="'+ link.parameter +'"]').closest('label').text();
      }
      else
      {
        var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
        s += input.closest('label').text();
        if(input.attr('manual_sending'))
        {
          s += ' <a class="btn btn-warning btn-mini" href="#" data-message="'+ message.id +'" id="send_mail'+ link.recipient_id +'">Envoyer</a>';
        }
      }
    }
  }
  else
  {
    s = "Aucun";
  }
  $('#preview_recipients_'+ recipient).html(s);

  for(var i=0; i<links.length; ++i)
  {
    var link = links[i];
    if(link.parameter) continue;
    var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
    if(input.attr('manual_sending'))
    {
      $('#send_mail'+ link.recipient_id).click(send_mail_click);
    }
  }
}

function send_mail_click()
{
  if(!confirm("Etes-vous sûr de vouloir envoyer le message à la liste ?")) return false;
  var recipient = $(this).attr('id').substr(9);
  var msg = $(this).attr('data-message');
  $.get('/terminus/ajax/send_mail?message='+ msg +'&recipient='+ recipient, function(data){
    alert("Message envoyé");
  });
}

function update_objects_field(recipient, message)
{
  var recipients = [];
  
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    if($(this).get(0).checked)
    {
      recipients.push({
        id: $(this).attr('link_id'),
        recipient_id: $(this).attr('value'),
        parameter: $(this).attr('parameter')
      });
    }
  });
  
  message[recipient +'_recipient'] = recipients;
}

function activateForm(myVAR)
{
  formToSave = true;
  $('#record_button').addClass('btn-warning');
  return true;
}

window.onbeforeunload = function (e) {
  if(!formToSave)
  {
    return;
  }
  var message = "Si vous fermez cette fenêtre, les données saisies vont être perdues.",
  e = e || window.event;
  // For IE and Firefox
  if (e) {
    e.returnValue = message;
  }

  // For Safari
  return message;
};

function generate_alternative_click()
{
  if($(this).attr('generate_alternative') == 'all')
  {
    var textAreas = $('#message textarea[field=message_alternative]');

    var confir = false;
    for(var i=0; i<textAreas.length; ++i)
    {
      var field = tinyMCE.get(textAreas.eq(i).attr('id'));
      if(field.getContent({format: 'text'}))
      {
        confir = true;
        break;
      }
    }
    if(confir)
    {
      if(!confirm("Au moins un contenu alternatif est déjà défini. Etes-vous sûr de vouloir écraser tous les contenus alternatifs ?"))
      {
        return false;
      }
    }
    var txt = tinyMCE.get('tinymce').getContent({format: 'text'});
    for(var i=0; i<textAreas.length; ++i)
    {
      var field = tinyMCE.get(textAreas.eq(i).attr('id'));
      var limitedTxt = txt.substring(0, textAreas.eq(i).attr('limit')).replace(/\n/ig,"<br>");
      field.setContent(limitedTxt);
    }
    $('#alternatives textarea.mceEditor').each(update_chars_alternative);
  }
  else
  {
    var textArea = $(this).parents('[type_id]').first().find('textarea');
    var limit = textArea.attr('limit');
    var field = tinyMCE.get(textArea.attr('id'));
    if(field.getContent({format: 'text'}))
    {
      if(!confirm("Un contenu alternatif est déjà défini. Etes-vous sûr de vouloir l'écraser ?"))
      {
        return false;
      }
    }
    var txt = tinyMCE.get('tinymce').getContent({format: 'text'});
    txt = txt.substring(0, limit).replace(/\n/ig,"<br>");
    field.setContent(txt);
    $('#alternatives textarea.mceEditor').each(update_chars_alternative);
  }
  return false;
}

function update_chars_alternative(alternativeEditor)
{
  if (typeof(alternativeEditor)==='undefined') {
    alternativeEditor = tinyMCE.activeEditor;  
  } else {
    alternativeEditor = tinyMCE.get($(this).attr('id'));
  }
  var size = alternativeEditor.getContent({format: 'text'}).length;
  $('#' + alternativeEditor.id).nextAll('[field=counter]').html(size + ' caractère' + (size > 1 ? 's' : ''));
}

function update_chars()
{
  var tinymce = tinyMCE.get('tinymce');
  var size = tinymce.getContent({format: 'text'}).length;
  $('#chars_main').html(size + ' caractère' + (size > 1 ? 's' : ''));
}

function tinymce_event(e)
{
  if(e.type == "keydown" || e.type == "keyup")
  {
    activateForm();
    if (tinyMCE.activeEditor.id == 'tinymce') {
      update_chars();
    } else {
      update_chars_alternative();
    }
  }
}


next_calendar_rank = 0;
calendar_by_rank = [];
function add_calendar(calendar)
{
  calendar.rank = next_calendar_rank;
  calendar_by_rank[calendar.rank] = calendar;
  ++next_calendar_rank;
  
  var calendar_name = calendar.name ? calendar.name : "(sans nom)";

  var s=  
    '<li id="mi_c_'+ calendar.rank +'" class="tree_1">'+
     '<a href="#"><span content="name">' + calendar_name +'</span>' +
     '<div class="pull-right" action="add_message" title="Nouveau message avec calendrier '+ calendar_name +'"><i class="icon-plus"></i></div>'
  ;
  if(!calendar.message.length)
  {
    s += '<div class="pull-right" action="remove_calendar" title="Supprimer le calendrier '+ calendar_name +'"><i class="icon-remove"></i></div>';
  }
  s +=
     '</a>'+
    '</li>';
  $('#menu').append(s);

  $('#change_calendar_list').append('<option value="'+ calendar.rank +'">' + calendar_name +'</option>');

  $('#mi_c_'+ calendar.rank).click(open_calendar_click);
  $('#mi_c_'+ calendar.rank +' div[action=add_message]').tooltip({placement: 'right'});
  $('#mi_c_'+ calendar.rank +' div[action=add_message]').click(create_message_click);
  $('#mi_c_'+ calendar.rank +' div[action=remove_calendar]').tooltip({placement: 'right'});
  $('#mi_c_'+ calendar.rank +' div[action=remove_calendar]').click(remove_calendar_click);

  for(var i=0; i<calendar.message.length; ++i)
  {
    calendar.message[i].calendar_rank = calendar.rank;
    add_message(calendar.message[i]);
  }
}

current_calendar = null;
function open_calendar(calendar)
{
  current_calendar = calendar;
  
  $('#calendar input[field=name]').val(calendar.name);
  
  $('#periods').html('');
  for(var i=0; i<calendar.period.length; ++i)
  {
    add_period(calendar.period[i]);
  }
  
  // Select the corresponding menu item
  $('#menu > li').removeClass('active');
  $('#menu i').removeClass('icon-white');
  $('#mi_c_'+ calendar.rank).addClass('active');
  $('#mi_c_'+ calendar.rank +' i').addClass('icon-white');

}

function date_from_sql(sql)
{
  if(!sql) return null;
  return new Date(
    sql.substr(0,4),
    sql.substr(5,2)-1,
    sql.substr(8,2),
    sql.substr(11,2),
    sql.substr(14,2),
    0, 0);
}

function date_to_sql(date)
{
  if(!date) return '';
  var s = '';
  s += date.getFullYear() +'-';
  if(date.getMonth() < 9) s += '0';
  s += (date.getMonth()+1) +'-';
  if (date.getDate() < 10) s += '0';
  s += date.getDate() +' ';
  if (date.getHours() < 10) s += '0';
  s += date.getHours() +':';
  if (date.getMinutes() < 10) s += '0';
  s += date.getMinutes();
  return s;
}

function close_calendar()
{
  if(current_calendar)
  {
    current_calendar.name= $('#calendar input[field=name]').val();
    current_calendar.period = [];
    var period_rank = 0;
    $('#periods > div').each(function(){
      var start_date_day_parts = $(this).find('input[field=start_date_day]').val().split('/');
      var start_date_hour_parts = $(this).find('input[field=start_date_hour]').val().split(':');
      if(start_date_hour_parts.length < 2)
      {
        start_date_hour_parts = [0, 0];
      }
      var start_date = null;
      if(start_date_day_parts.length >= 3)
      {
        start_date = new Date(start_date_day_parts[2], start_date_day_parts[1]-1, start_date_day_parts[0], start_date_hour_parts[0], start_date_hour_parts[1], 0, 0);
      }

      var end_date_day_parts = $(this).find('input[field=end_date_day]').val().split('/');
      var end_date_hour_parts = $(this).find('input[field=end_date_hour]').val().split(':');
      if(end_date_hour_parts.length < 2)
      {
        end_date_hour_parts = [0, 0];
      }
      var end_date = null;
      if(end_date_day_parts.length >= 3)
      {
        end_date = new Date(end_date_day_parts[2], end_date_day_parts[1]-1, end_date_day_parts[0], end_date_hour_parts[0], end_date_hour_parts[1], 0, 0);
      }

      current_calendar.period.push({
        id: $(this).attr('period_id'),
        rank: period_rank,
        start_date: date_to_sql(start_date),
        end_date: date_to_sql(end_date),
        start_hour: $(this).find('input[field=start_hour]').val(),
        end_hour: $(this).find('input[field=end_hour]').val(),
        date:[]
      });
      ++period_rank;
    });
  }
  current_calendar = null;
}

function remove_calendar(calendar)
{
  close_message();
  close_calendar();
  for(var i=0; i<myEvent.calendar.length; ++i)
  {
    if(myEvent.calendar[i] == calendar)
    {
      myEvent.calendar.splice(i,1);
      break;
    } 
  }
  $('#mi_c_'+ calendar.rank).remove();
  activateForm();
  display('properties');
}

function remove_calendar_click()
{
  var calendar = calendar_by_rank[$(this).closest('li').attr('id').substr(5)];
  if(!confirm("Etes-vous sûr de vouloir supprimer le calendrier "+ calendar.name +" ?")) return false;
  remove_calendar(calendar);
  return false;
}

next_message_rank = 0;
message_by_rank = [];
function add_message(message)
{
  message.rank = next_message_rank;
  message_by_rank[message.rank] = message;
  ++next_message_rank;

  var calendar_item = $('#mi_c_'+ message.calendar_rank);
  var last_item = calendar_item.last('.tree_2');
  var insert_position = last_item ? last_item : calendar_item;
  var message_title = message.title ? message.title : "(sans titre)";
  insert_position.after(
   '<li id="mi_m_'+ message.rank +'" class="tree_2">' +
   '<a href="#"><span content="title">'+ message_title + '</span>' +
   '<div class="pull-right" action="copy_message" title="Dupliquer le message '+ message_title +'"><i class="icon-share"></i></div>' +
   '<div class="pull-right" action="remove_message" title="Supprimer le message '+ message_title +'"><i class="icon-remove"></i></div>' +
   '</a></li>'
  );
  $('#mi_m_'+ message.rank).click(open_message_click);
  var remove_message_link_div = $('#mi_m_'+ message.rank +' div[action=remove_message]');
  remove_message_link_div.tooltip({placement: 'right'});
  remove_message_link_div.click(remove_message_click);
  var copy_message_link_div = $('#mi_m_'+ message.rank +' div[action=copy_message]');
  copy_message_link_div.tooltip({placement: 'right'});
  copy_message_link_div.click(copy_message_click);
}

current_message = null;
function open_message(message)
{
  current_message = message;
  $('#message input[field=title]').val(message.title);
  // When opening a message, format its content, i.e. \" becomes "
  tinyMCE.get('tinymce').setContent(message.content.replace(/\\\"/g,"\""));
  $('#message select[field=level]').val(message.level);
  $('#message input[field=tags]').val(message.tags);
  $('#message [field=displayDuration]').val(message.displayDuration);
  if (message.digitized_version != "")
  {
    $('#message input[field=digitized_version]').val(message.digitized_version);
  }
  else
  {
    if ($('#message input[field=def_digital_msg_url]').val() != undefined)
    {
      $('#message input[field=digitized_version]').val($('#message input[field=def_digital_msg_url]').val());
    }
    else
    {
      $('#message input[field=digitized_version]').val(message.digitized_version);
    }
  }

  $('#change_calendar_list').val(message.calendar_rank);

  if(message.alternative.length)
  {
    $('#table_alternatives > div.row-fluid').each(function(){
      $(this).addClass('hide');
      for(var i=0; i<message.alternative.length; ++i)
      {
        var at = message.alternative[i];
        if(at.type_id == $(this).attr('type_id'))
        {
          $(this).removeClass('hide');
          var text = at.content.replace(/\\\"/g,"\"").replace(/\n/ig,"<br>")
          tinyMCE.get('tinymce_ma' + i).setContent(text);
          break;
        }
      }
    });
    $('#alternatives').removeClass('hide');
    $('#calendar_panel_id').html('4');
  }
  else
  {
    $('#alternatives').addClass('hide');
    $('#calendar_panel_id').html('3');
  }

  update_object_preview('line', message);
  update_object_preview('stoparea', message);
  update_object_preview('displayscreen', message);

  // Select the corresponding menu item
  $('#menu > li').removeClass('active');
  $('#menu i').removeClass('icon-white');
  $('#mi_m_'+ message.rank).addClass('active');
  $('#mi_m_'+ message.rank +' i').addClass('icon-white');
  
  update_chars();
  $('#alternatives textarea.mceEditor').each(update_chars_alternative);
  
  for(var i=0; i < messagesWhichHaveBeenClosed.length; ++i)
  {
    if(messagesWhichHaveBeenClosed[i] == message.id)
    {
      messagesWhichHaveBeenClosed.splice(i,1);
      break;
    }
  }
}

function close_message()
{
  if(current_message)
  {
    current_message.title = $('#message input[field=title]').val();
    // when closing a message, format its content to make it SYNTHESE registrabled, i.e. " becomes \"
    current_message.content = tinyMCE.get('tinymce').getContent().replace(/\"/g,"\\\"");
    current_message.level = $('#message select[field=level]').val();
    current_message.tags = $('#message input[field=tags]').val();
    current_message.displayDuration = $('#message [field=displayDuration]').val();
    if ($('#message [field=digitized_version]').val() == $('#message input[field=def_digital_msg_url]').val())
    {
      current_message.digitized_version = "";
    }
    else
    {
      current_message.digitized_version = $('#message [field=digitized_version]').val();
    }

    $('#table_alternatives > div.row-fluid').each(function(){
      if(!$(this).hasClass('hide'))
      {
        for(var i=0; i<current_message.alternative.length; ++i)
        {
          var at = current_message.alternative[i];
          if(at.type_id == $(this).attr('type_id'))
          {
            at.content = tinyMCE.get('tinymce_ma' + i).getContent({format: 'text'}).replace(/\"/g,"\\\"");
            break;
          }
        }
      }
    });
    
    messagesWhichHaveBeenClosed.push(current_message.id);
  }
  current_message = null;
}

function copy_message_click()
{
  close_message();
  close_calendar();

  var li = $(this).closest('li');
  var message_rank = li.attr('id').substr(5);
  var message = message_by_rank[message_rank];
  
  var new_message = {
   id:"",
   calendar_rank: message.calendar_rank,
   title:"Copie de "+ message.title,
   content: message.content,
   level: message.level,
   tags: message.tags,
   displayDuration: message.displayDuration,
   digitized_version: message.digitized_version,
   alternative:[],
   section: message.section,
   line_recipient:[],
   stoparea_recipient:[],
   displayscreen_recipient:[]
  };
  
  if (new_message.digitized_version == "")
  {
    if ($('#message input[field=def_digital_msg_url]').val() != "")
    {
      new_message.digitized_version = $('#message input[field=def_digital_msg_url]').val();
    }
  }
  
  myEvent.calendar[message.calendar_rank].message.push(new_message);
  add_message(new_message);
  open_message(new_message);
  display('message');
  activateForm();
  $('input[field=title]').focus();
  return false;
}

function build_divs()
{
  for(var i=0; i<myEvent.calendar.length; ++i)
  {
    add_calendar(myEvent.calendar[i]);
  }
}

function remove_message_click()
{
  close_message();
  close_calendar();
  var li = $(this).closest('li');
  var message = message_by_rank[li.attr('id').substr(5)];
  if(!confirm("Etes-vous sûr de vouloir supprimer le message "+ message.title +" ?")) return false;
  remove_message(message);
  activateForm();
  display('properties');
  return false;
}

function remove_message(message)
{
  var calendar = calendar_by_rank[message.calendar_rank];
  
  for(var i=0; i<calendar.message.length; ++i)
  {
    if(calendar.message[i] == message)
    {
      calendar.message.splice(i,1);
      break;
    } 
  }
  $('#mi_m_'+ message.rank).remove();
}

function create_message_click()
{
  close_message();
  close_calendar();

  var calendar_rank = $(this).closest('li').attr('id').substr(5);
  var new_message = {
   id:"",
   calendar_rank: calendar_rank,
   title:"",
   content:"",
   level:10,
   tags:"",
   displayDuration:"",
   digitized_version: $('#message input[field=def_digital_msg_url]').val() != undefined ? $('#message input[field=def_digital_msg_url]').val() : "",
   alternative:[],
   section:[],
   line_recipient:[],
   stoparea_recipient:[],
   displayscreen_recipient:[]
  };
  
  calendar_by_rank[calendar_rank].message.push(new_message);
  add_message(new_message);
  open_message(new_message);
  display('message');
  activateForm();
  $('input[field=title]').focus();
  return false;
}

function open_message_click()
{
  close_message();
  close_calendar();
  open_message(message_by_rank[$(this).attr('id').substr(5)]);
  display('message');
  return false;
}

function open_calendar_click()
{
  close_message();
  close_calendar();
  open_calendar(calendar_by_rank[$(this).attr('id').substr(5)]);
  display('calendar');
  return false;
}

function open_properties_click()
{
  close_message();
  close_calendar();
  
  // Select the corresponding menu item
  $('#menu > li').removeClass('active');
  $('#menu i').removeClass('icon-white');
  $('#mi_properties').addClass('active');
  $('#mi_properties i').addClass('icon-white');

  display('properties');
  return false;
}

function display(screen_id)
{
  // Display the specified div
  $('#main_div > div').addClass('hide');
  $('#'+ screen_id).removeClass('hide');
  
  $('#the_form_display_AUTOGENERATEDFIELDID').val(screen_id);
}

function tiny_mce_init()
{
  update_chars();
  display_init();
}

function save_click()
{
  var event_start_date = null;
  var event_end_date = null;
  var current_message_id = (current_message != null ? current_message.id : 0);
  if(!$('#calendar').hasClass('hide'))
  {
    $('#the_form_display_AUTOGENERATEDFIELDID').val('calendar');
    $('#the_form_rank_to_display_AUTOGENERATEDFIELDID').val(current_calendar.rank);
  }
  else if(!$('#message').hasClass('hide'))
  {
    if (current_message.section.length == 0) {
        current_message.section.push({id : $('[name="section"]').val()});
    } else {
        current_message.section[0].id = $('[name="section"]').val();
    }

    $('#the_form_display_AUTOGENERATEDFIELDID').val('message');
    $('#the_form_rank_to_display_AUTOGENERATEDFIELDID').val(current_message.rank);
  }

  // Prepare the message.content in case of images
  if(myEvent.calendar != undefined)
  {
    for(var i=0; i<myEvent.calendar.length; ++i)
    {
      if (myEvent.calendar[i].message != undefined)
      {
        for(var j=0; j<myEvent.calendar[i].message.length; ++j)
        {
          if(myEvent.calendar[i].message[j].id != current_message_id)
          {
            // If the message has not been opened then closed before creating/modifying an other message
            if(0 > messagesWhichHaveBeenClosed.indexOf(myEvent.calendar[i].message[j].id))
            {
              myEvent.calendar[i].message[j].content = myEvent.calendar[i].message[j].content.replace(/\"/g,"\\\"");
            }
          }
        } 
      }
    }
  }

  if (typeof start_event_date_day != 'undefined')
  {
    if ($('#start_event_date_day').val() != "")
    {
      var startTime = $('#start_event_date_day').val().split('/');
      event_start_date = startTime[2] + '-' + startTime[1] + '-' + startTime[0];
      $('#the_form_actionParam_event_start_date_AUTOGENERATEDFIELDID').val(event_start_date);
    }
  }
  if (typeof end_event_date_day != 'undefined')
  {
    if ($('#end_event_date_day').val() != "")
    {
      var endTime = $('#end_event_date_day').val().split('/');
      event_end_date = endTime[2] + '-' + endTime[1] + '-' + endTime[0];
      $('#the_form_actionParam_event_end_date_AUTOGENERATEDFIELDID').val(event_end_date);
    }
  }

  close_message();
  close_calendar();
  var json = JSON.stringify(myEvent);
  // Put the json image content to its original syntax
  if(myEvent.calendar != undefined)
  {
    for(var i=0; i<myEvent.calendar.length; ++i)
    {
      if (myEvent.calendar[i].message != undefined)
      {
        for(var j=0; j<myEvent.calendar[i].message.length; ++j)
        {
          if(myEvent.calendar[i].message[j].id != current_message_id)
          {
            myEvent.calendar[i].message[j].content = myEvent.calendar[i].message[j].content.replace(/\"/g,"\\\"");
          }
        } 
      }
    }
  }
  $('#the_form_actionParam_json_AUTOGENERATEDFIELDID').val(json);
  formToSave = false;
  $('#the_form').submit();
}

function alert_on_exit()
{
  if(!formToSave) return true;
  if(confirm("Des modifications n'ont pas été enregistrées. Voulez-vous tout de même quitter la page ?"))
  {
    formToSave = false;
    return true;
  }
  return false;
}

function change_message_title()
{
  var title = $('#message input[field=title]').val();
  if(!title) title = "(sans titre)";
  $('#mi_m_'+ current_message.rank +' span[content=title]').html(title);
  activateForm();
}

function change_message_displayduration()
{
  if (typeof $(this).val() != 'undefined') {
    var disp_dur = $(this).val();
    $('#message input[field=displayDuration]').val(disp_dur);
    activateForm();
  }
}

function change_message_digitized_version()
{
  if (typeof $(this).val() != 'undefined') {
    var disp_dur = $(this).val();
    $('#message input[field=digitized_version]').val(disp_dur);
    activateForm();
  }
}

function change_event_title() {
  var title = $(this).val();
  if(!title) title = "(sans titre)";
  $('#the_form').find('input[name=' + $(this).attr('name') + ']').val(title);
  activateForm();
}

function pick_event_date() {
    var dateInput = $(this).find("input")[0];
    change_event_date.apply(dateInput);
}


function is_start_input(inputElement) {
    if ($(inputElement).is('[name]') && $(inputElement).attr('name').indexOf('start') != -1) return true;
    if ($(inputElement).is('[field]') && $(inputElement).attr('field').indexOf('start') != -1) return true;
    return false;
}
    


function change_event_date() {
    var timeInput = $(this).parents('div.controls').find('input')[1];

    if ($(this).val().length != 10) {
        $(this).val('');
        $(timeInput).val('');
    } else if ($(timeInput).val().length == 0) {
        if (is_start_input(timeInput)) {
            $(timeInput).val('00:00');
        } else {
            $(timeInput).val('23:59');
        }
    }
    activateForm();
}

function change_event_time() {
    var dateInput = $(this).parents('div.controls').find('input')[0];

    if ($(this).val().length != 5) {
        if (is_start_input(dateInput)) {
            $(this).val('00:00');
        } else {
            $(this).val('23:59');
        }
    } 
    activateForm();
}


function calendar_rename()
{
  var title = $('#calendar input[field=name]').val();
  if(!title) title = "(sans nom)";
  $('#mi_c_'+ current_calendar.rank +' span[content=name]').html(title);
  $('#change_calendar_list option[value='+ current_calendar.rank +']').html(title);
  activateForm();
}

function new_period_click()
{
  var period ={
    id:"",
    start_date:"",
    end_date:"",
    start_hour:"",
    end_hour:"",
    date:[]
  };
  period.rank = current_calendar.period.push(period) - 1;
  add_period(period);
  return false;
}

function new_calendar_click()
{
  close_message();
  close_calendar();
  
  var calendar ={
    id:"",
    name:"",
    period:[],
    message:[]
  };
  myEvent.calendar.push(calendar);
  add_calendar(calendar);
  open_calendar(calendar);
  
  display('calendar');
  activateForm();
  $('#calendar input[field=name]').focus();
  return false;
}

function format_date(date, month, year) {
  var result = '';
  if (date < 10) result += '0';
  result += date.toString();
  result += '/';
  if (month < 10) result += '0';
  result += month.toString();
  result += '/';
  result += year.toString();
  return result;
}

function format_time(hours, minutes) {
  var result = '';
  if (hours < 10) result += '0';
  result += hours.toString();
  result += ':';
  if (minutes < 10) result += '0';
  result += minutes.toString();
  return result;
}



function add_period(period)
{
  var start_date = null;
  var end_date = null;
  if(period.start_date)
  {
    start_date= date_from_sql(period.start_date);
  }
  if(period.end_date)
  {
    end_date = date_from_sql(period.end_date);
  }
  var s =
   '<div class="well form-horizontal" rank="' + period.rank +'" period_id="'+ period.id +'">' +
    '<h4>Publication phase '+ (period.rank+1) +'<div class="pull-right"><a href="#" class="btn btn-danger btn-mini" id="remove_period_'+ period.rank +'">Supprimer</a></div></h4>' +
    '<div class="control-group">' +
     '<label class="control-label">Début</label>' +
     '<div class="controls">' +
      '<div class="input-append date" data-date="';
       if(start_date)
       {
         s += format_date(start_date.getDate(), start_date.getMonth()+1, start_date.getFullYear());
       }
       s+='" data-date-format="dd/mm/yyyy">' +
       '<input class="input-small" placeholder="JJ/MM/AAAA" type="text" field="start_date_day" value="';
       if(start_date)
       {
         s += format_date(start_date.getDate(), start_date.getMonth()+1, start_date.getFullYear());
       }
       s+= '"/>' +
       '<span class="add-on"><i class="icon-th"></i></span>' +
      '</div> ' +
      '<input type="text" class="input-mini" field="start_date_hour" placeholder="HH:MM" value="';
      if(start_date)
      {
        s += format_time(start_date.getHours(), start_date.getMinutes());
      }
      s+='" />' +
     '</div>' +
    '</div>' +
    '<div class="control-group">' +
     '<label class="control-label">Fin</label>' +
     '<div class="controls">' +
      '<div class="input-append date" data-date="';
       if(end_date)
       {
         s += format_date(end_date.getDate(), end_date.getMonth()+1, end_date.getFullYear());
       }
       s += '" data-date-format="dd/mm/yyyy">' +
       '<input class="input-small" placeholder="JJ/MM/AAAA" type="text" field="end_date_day" value="';
       if(end_date)
       {
         s += format_date(end_date.getDate(), end_date.getMonth()+1, end_date.getFullYear());
       }
       s +='" />' +
       '<span class="add-on"><i class="icon-th"></i></span>' +
      '</div> ' +
      '<input type="text" class="input-mini" field="end_date_hour" placeholder="HH:MM" value="';
      if(end_date)
      {
        s += format_time(end_date.getHours(), end_date.getMinutes());
      }
      s += '" />' +
     '</div>' +
    '</div>' +
    '<div class="control-group">' +
     '<label class="control-label">Plage horaire</label>' +
     '<div class="controls">' +
      '<input type="text" class="input-mini" field="start_hour" placeholder="HH:MM" onkeyup="activateForm()" onpaste="activateForm()" oncut="activateForm()" value="'+ period.start_hour +'" /> à ' +
      '<input type="text" class="input-mini" field="end_hour" placeholder="HH:MM" onkeyup="activateForm()" onpaste="activateForm()" oncut="activateForm()" value="'+ period.end_hour +'" />' +
     '</div>' +
    '</div>' +
   '</div>';
  
  
  $('#periods').append(s);
  $('.date').datepicker().on('changeDate', pick_event_date);
  $('#remove_period_'+ period.rank).click(remove_period_click);
  
  
  $(function(){
     $('#periods').find('div.controls').find("input[field $= 'date_day']").bind('focusout', change_event_date); 
     $('#periods').find('div.controls').find("input[field $= 'date_hour']").bind('focusout', change_event_time); 
  });

}

function remove_period_click()
{
  var period_rank = $(this).closest('div[rank]').attr('rank');
  if(!confirm("Etes-vous sûr de vouloir supprimer la plage horaire ?")) return false;

  $(this).closest('div[rank]').remove();
  current_calendar_rank = current_calendar.rank;
  close_calendar();
  open_calendar(calendar_by_rank[current_calendar_rank]);
  activateForm();
  return false;
}

function drawCalendar()
{
  if(!document.getElementById('_actionParamsd_AUTOGENERATEDFIELDID').value.length ||
     !document.getElementById('_actionParamed_AUTOGENERATEDFIELDID').value.length
  ){
    $('#msg_calendar').html('');
    return;
  }
  var sd=$D(document.getElementById('_actionParamsd_AUTOGENERATEDFIELDID').value);
  var ed=$D(document.getElementById('_actionParamed_AUTOGENERATEDFIELDID').value);
  
  var s_day = sd.getDay();
  if(!s_day) s_day = 7;
  var e_day = ed.getDay();
  if(!e_day) e_day = 7;
  
  var fd = $D();
  fd.setTime(sd.getTime());
  fd.setHours(23);
  fd.setMinutes(59);
  fd.add(-s_day+1,"days");

  var ld = new Date();
  ld.setTime(ed.getTime());
  ld.setHours(0);
  ld.setMinutes(0);
  ld.setMilliseconds(0);
  ld.add(8-e_day,"days");
  
  var first=1;
  var lm='';
  var months= new Array('jan','fev','mars','avr','mai','juin','juil','août','sep','oct','nov','dec');
  s = '<table class="table table-striped"><thead><tr><th></th><th>lu</th><th>ma</th><th>me</th><th>je</th><th>ve</th><th>sa</th><th>di</th></tr></thead><tbody>';
  for(var cd=fd;cd<=ld;cd.add(1,"days"))
  {
    if(cd.getDay()==1)
    {
      if(first)
      {
        first = 0;
      }
      else
      {
        s += '</tr>';
      }
      s += '<tr><td>';
      if(cd.getMonth()!=lm) s += months[cd.getMonth()] +' '+ cd.getFullYear();
      lm = cd.getMonth();
      s += '</td>';
    }
    s += '<td><span class="label label-';
    if(cd >= sd && cd <= ed) s+= 'success'; else s+= 'important';
    s += '">';
    if(cd.getDate() < 10) s += '0';
    s += cd.getDate() + '</td>';
  }
  s += '</tbody></table>';

  $('#msg_calendar').html(s);
}

function change_calendar_click()
{
  remove_message(current_message);
  var new_calendar_rank = $('#change_calendar_list').val();
  current_message.calendar_rank = new_calendar_rank;
  calendar_by_rank[new_calendar_rank].message.push(current_message);
  add_message(current_message);

  $('#change_calendar_list').val(new_calendar_rank);
  activateForm(); 
}

function focus_on_input()
{
  $(this).find('input[type=text]').eq(0).focus();
}


$(function(){
  $('#record_button').click(save_click);
  $('.navbar-inner a').click(alert_on_exit);
  $('#mi_properties').click(open_properties_click);
  $('.openclose').click(openclose);
  $('input[action=search]').keyup(filter);
  $('input[factory]').change(change_recipient);
  $('#new_period').click(new_period_click);
  $('#message a[factory]').click(show_recipients_click);
  $('#message a[generate_alternative]').click(generate_alternative_click);
  $('#message input[field=title]').bind('keyup', change_message_title);
  $('#message input[field=title]').bind('cut', change_message_title);
  $('#message input[field=title]').bind('paste', change_message_title);
  $('#message input[field=displayDuration]').bind('keyup', change_message_displayduration);
  $('#message input[field=displayDuration]').bind('cut', change_message_displayduration);
  $('#message input[field=displayDuration]').bind('paste', change_message_displayduration);
  $('#message input[field=digitized_version]').bind('keyup', change_message_digitized_version);
  $('#message input[field=digitized_version]').bind('cut', change_message_digitized_version);
  $('#message input[field=digitized_version]').bind('paste', change_message_digitized_version);
  $('#calendar input[field=name]').bind('keyup', calendar_rename);
  $('#calendar input[field=name]').bind('cut', calendar_rename);
  $('#calendar input[field=name]').bind('paste', calendar_rename);
  $('div[action=add_calendar]').click(new_calendar_click);
  $('div[action=add_calendar]').tooltip({placement: 'right'});
  $('#change_calendar').click(change_calendar_click);
  $('.modal').bind('shown', focus_on_input);


  // Diffusion page behaviors.
  var diffusionForm = $('#diffusion-properties');
  diffusionForm.find('input[name=actionParamnam]').bind('keyup', change_event_title);
  diffusionForm.find('input[name=actionParamnam]').bind('cut', change_event_title);
  diffusionForm.find('input[name=actionParamnam]').bind('paste', change_event_title);
  diffusionForm.find('input[name=actionParam_event_start_date]').bind('focusout', change_event_date);
  diffusionForm.find('input[name=actionParam_event_start_time]').bind('focusout', change_event_time);
  diffusionForm.find('input[name=actionParam_event_end_date]').bind('focusout', change_event_date);
  diffusionForm.find('input[name=actionParam_event_end_time]').bind('focusout', change_event_time);
  diffusionForm.find('.date').datepicker({format: 'dd/mm/yyyy'}).on('changeDate', pick_event_date);;

  });