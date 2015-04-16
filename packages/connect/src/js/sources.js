function source_add_click()
{
 $('#source_add_modal').modal('show');
 return false;
}

function source_remove_click()
{
 $('#source_remove_modal').modal('show');
 return false;
}

function source_objects_update()
{
 $.ajax({
   url: "/connect/ajax/datasource_get_objects?table="+ $('#source_objects_table').val()+"&data_source="+ $('#source_objects_table').data('data_source'),
 }).done(function(result) {
   $('#source_objects_body').html(result);
 });
}

$(function(){
 $('#source_add_link').click(source_add_click);
 $('#source_remove_link').click(source_remove_click);
 $('#source_objects_table').change(source_objects_update);
});