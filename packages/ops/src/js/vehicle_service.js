function add_vehicle_service_click()
{
  $('#add_vehicle_service_modal').modal('toggle');
  return false;
}

function remove_service_click()
{
  if(confirm("Etes-vous sûr de vouloir supprimer le service ?"))
  {
    var tr = $(this).closest('tr');
    tr.remove();
  }
  return false;
}

function add_services(services)
{
  for(var i=0; i<services.service.length; ++i)
  {
    add_service(services.service[i], false);
  }
}

function add_service(service, auto_sort)
{
  var trs = $('#services tbody tr');
  var pos = trs.size();
  if(auto_sort)
  {
    for(var i=0; i<trs.size(); ++i)
    {
      var tr = trs.eq(i);
      if(tr.data('id')=='pause') continue;
      var departure_schedule = tr.find('td:first').html();
      if(departure_schedule > service.departure_schedule)
      {
        pos= i;
        break;
      }
    }
  }
  var new_row =
   '<tr data-id="'+ service.id +'" data-arrival_schedule="'+ service.arrival_schedule +'">'+
   '<td>'+ service.departure_schedule +'</td>'+
   '<td>'+ service.departure_place_name +'</td>'+
   '<td>'+ ((service.class != 'commercial') ?
            '<i class="icon icon-ban-circle" title="Haut-le-pied"></i>' : 
            service.line
           ) +'</td>'+
   '<td>'+ service.direction +'</td>'+
   '<td><a href="/ops/offer/network/line/vehiclejourney?service='+ service.id +'">'+ service.number +'</a></td>'+
   '<td>'+ '</td>'+
   '<td><a href="#" data-toggle="remove_service_modal"><i class="icon icon-remove"></i></a></td>'+
   '<td>'+ '</td>'+
   '</tr>'
  ;
  if(pos == trs.size())
  {
    var last_arrival_schedule = trs.last().data('arrival_schedule');
    if(last_arrival_schedule < service.departure_schedule)
    {
      new_row =
	   '<tr data-id="pause">'+
	   '<td>'+ last_arrival_schedule +'</td>'+
	   '<td>'+ service.departure_place_name +'</td>'+
	   '<td><i class="icon icon-pause"></i></td>'+
	   '<td></td>'+
	   '<td></td>'+
	   '<td>'+ '</td>'+
	   '<td><a href="#" data-toggle="add_service_modal"><i class="icon icon-plus"></i></a></td>'+
	   '<td>'+ '</td>'+
	   '</tr>'+
	  new_row
      ;
    }
    $('#services tbody').append(new_row);
  }
  else
  {
    var trpos = trs.eq(pos);

        
   
    
    var next_departure_schedule = trpos.find('td:first').html();
    if(service.arrival_schedule < next_departure_schedule)
    {
      new_row +=
	   '<tr data-id="pause">'+
	   '<td>'+ service.arrival_schedule +'</td>'+
	   '<td>'+ service.arrival_place_name +'</td>'+
	   '<td><i class="icon icon-pause"></i></td>'+
	   '<td></td>'+
	   '<td></td>'+
	   '<td>'+ '</td>'+
	   '<td><a href="#" data-toggle="add_service_modal"><i class="icon icon-plus"></i></a></td>'+
	   '<td>'+ '</td>'+
	   '</tr>'
      ;
    }
    trs.eq(pos).before(new_row);
  }
  $('tr[data-id="'+ service.id +'"] a[data-toggle="remove_service_modal"]').click(remove_service_click);
}

function add_service_click()
{
  $('#add_service_modal').modal('toggle');
  return false;
}

function save_click()
{
  var url = '/ops/ajax/vehicle_service_save?vehicle_service_id='+ $(this).attr('data-id');
  url += '&name=' + $(this).data('name');
  url += '&unit_id=' + $(this).attr('data-unit_id');
  url += '&services=';
  var services = $('#services tbody').children();
  services.each(function(index){
    if($(this).attr('data-id') == 'pause') return;
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
     $('#save_link').addClass('hide');
   }).fail(function()
   {
     alert("Erreur lors de la sauvegarde");
   });
  return false;
}

function update_click()
{
  $('#name_field').val($('#save_link').data('name'));
  $('#unit_field option[value="'+ $('#save_link').attr('data-unit_id') +'"]').prop('selected', true);
  $('#vehicle_service_properties_modal').modal('toggle');
  return false;
}

$(function(){
 $('#add_vehicle_service_link').click(add_vehicle_service_click);
 $('#add_service_link').click(add_service_click);
 $('#save_link').click(save_click);
 $('#edit_link').click(update_click);
});