function show_save_button()
{
  $('#save_link').removeClass('hide');
}

function add_unit_click()
{
  $('#add_unit_modal').modal('toggle');
  return false;
}

function add_line(line)
{
  var tbody = $('#allowed_lines').find('tbody');
  tbody.append('<tr data-id="'+ line.id +'" data-name="'+ line.name +'" data-short_name="'+ line.short_name +'"><td>'+ line.short_name +' '+ line.name +'</td><td><a href="#" id="remove_line_'+ line.id +'"><i class="icon icon-remove"></i></a></td></tr>');
  $('#lines option[data-id="'+ line.id +'"]').remove();
  $('#remove_line_'+ line.id).click(remove_line);
}

function remove_line()
{
  var tr = $(this).closest('tr');
  $('#lines').append(
   '<option data-id="'+ tr.data('id') +'" data-short_name="'+ tr.data('short_name') +'" data-name="'+ tr.data('name') +'">'+ tr.data('short_name') +' '+ tr.data('name') +'</option>'
  );
  tr.remove();
  show_save_button();
  return false;
}

function add_lines(lines)
{
  for(var i=0; i<lines.line.length; ++i)
  {
    add_line(lines.line[i]);
  }
}

function add_line_click()
{
  var selected_line = $('#lines option:selected');
  var line = {
    "id": selected_line.attr('data-id'),
    "short_name": selected_line.data('short_name'),
    "name": selected_line.data('name')
  };
  add_line(line);
  show_save_button();
  return false;
}

function save_click()
{
  var url = '/ops/ajax/unit_save?unit_id='+ $(this).attr('data-id');
  url += '&name=' + $('#name_field').val() + '&allowed_lines=';
  var lines = $('#allowed_lines tbody').children();
  lines.each(function(index){
    if(index) url += ',';
    url += $(this).attr('data-id');
  });
  $.ajax(url)
   .done(function(data) {
     if(data != '')
     {
       alert("Erreur lors de la sauvegarde");
       return;
     }
     var name = ($('#name_field').val() == '') ? $('#save_link').attr('data-id') : $('#name_field').val();
     $('#displayed_title').html(name);
     $('#units_list li.active span.unit_name').html(name);
     $('#save_link').addClass('hide');
   }).fail(function()
   {
     alert("Erreur lors de la sauvegarde");
   });
  return false;
}

$(function(){
 $('#add_unit_link').click(add_unit_click);
 $('#add_line_link').click(add_line_click);
 $('#name_field').keydown(show_save_button);
 $('#save_link').click(save_click);
});