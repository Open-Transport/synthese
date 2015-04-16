function focus_on_first_field()
{
  $(this).find('input[type="text"]').focus();
}

function resize_content()
{
  var div = $('#content');
  var navbar = $('#navbar');
  div.width($(window).width());
  div.height($(window).height() - navbar.height());
}

function select_current_unit_click()
{
  $('#select_current_unit_modal').modal('toggle');
  return false;
}

$(function(){
//  $('.modal').shown(focus_on_first_field);
  resize_content();
  $(window).resize(resize_content);
  $('#select_current_unit_link').click(select_current_unit_click);
});