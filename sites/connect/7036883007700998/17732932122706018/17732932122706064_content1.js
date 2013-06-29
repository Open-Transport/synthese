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

$(function(){
 $('#source_add_link').click(source_add_click);
 $('#source_remove_link').click(source_remove_click);
});