function modal_focus_on_first_field()
{
  var inputs = $(this).find('input[type=text]');
  if(inputs.size())
  {
    inputs.eq(0).focus();
  }
}

$(function(){
  $('div.modal').on('shown',modal_focus_on_first_field);
});