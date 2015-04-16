function connection_add_click()
{
  $('#new_connection').modal('show');
  return false;
}

$(function(){
  $('#connection_add_link').click(connection_add_click);
});

function device_add_click()
{
  $('#new_connection').modal('show');
  return false;
}

$(function(){
  $('#device_add_link').click(device_add_click);
});