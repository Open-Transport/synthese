function stream_add_click()
{
 $('#stream_add_modal').modal('show');
 return false;
}

function stream_remove_click()
{
 $('#stream_remove_modal').modal('show');
 return false;
}

$(function(){
 $('#stream_add_link').click(stream_add_click);
 $('#stream_remove_link').click(stream_remove_click);
});