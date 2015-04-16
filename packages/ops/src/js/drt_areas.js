function show_save_button()
{
  $('#save_link').removeClass('hide');
}

function remove_stop()
{
  var tr = $(this).closest('tr');
  tr.remove();
  show_save_button();
  return false;
}

function add_stops(stops)
{
  for(var i=0; i<stops.stop.length; ++i)
  {
    add_stop(stops.stop[i]);
  }
}

function add_stop(stop)
{
  var tbody = $('#stops').find('tbody');
  tbody.append('<tr data-id="'+ stop.id +'" data-name="'+ stop.name +'"><td>'+ stop.name +'</td><td><a href="#" id="remove_stop_'+ stop.id +'"><i class="icon icon-remove"></i></a></td></tr>');
  $('#remove_stop_'+ stop.id).click(remove_stop);
}

function update_stop_autocomplete(event, ul)
{
  $("#new_stop_field").val(ul.item.value);
  $("#new_stop_field").attr('data-id', ul.item.id);
  return false;
}

function add_stop_click()
{
  add_stop({"id": $("#new_stop_field").attr('data-id'), "name":$("#new_stop_field").val() });
  show_save_button();
  return false;
}

function save_click()
{
  var url = '/ops/ajax/drt_area_save?area_id='+ $(this).attr('data-id');
  url += '&name=' + $('#name_field').val() + '&stops=';
  var stops= $('#stops tbody').children();
  stops.each(function(index){
    if(index) url += ',';
    url += $(this).attr('data-id');
  });
  $.ajax(url)
   .done(function(data) {
     if(data != '')
     {
       alert("Erreur lors de la sauvegarde : "+ data);
       return;
     }
     var name = ($('#name_field').val() == '') ? $('#save_link').attr('data-id') : $('#name_field').val();
     $('#displayed_title').html(name);
     $('#areas_list li.active span.unit_name').html(name);
     $('#save_link').addClass('hide');
   }).fail(function()
   {
     alert("Erreur lors de la sauvegarde");
   });
  return false;
}

function add_area_click()
{
  $('#add_area_modal').modal('toggle');
  return false;
}

$(function(){
 $('#add_area_link').click(add_area_click);
 $('#name_field').keydown(show_save_button);
 $('#add_stop_link').click(add_stop_click);
 $('#save_link').click(save_click);
 $('#stops tbody').sortable({ "update": show_save_button });
 $( "#new_stop_field" ).autocomplete({
   source: "/ops/ajax/get_stop_areas",
   minLength: 2,
   select: update_stop_autocomplete
  });
});