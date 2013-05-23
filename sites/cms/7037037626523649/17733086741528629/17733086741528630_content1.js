function autosize()
{
  $('#main').width($(window).width()-$('#menu').width());
  $('#editor').width($('#main').width());
  var h = $(window).height() - $('.navbar').height();
  $('#editor').height(h);
  $('#menu_content > div').height(h - $('#menu_title').height() - 8);
}

function save_file(editor, file)
{
  var file_id = editAreaLoader.getCurrentFile(editor).id;
  var result = $.post(
    '/cms/ajax/save_file_content?roid='+ file_id,
    'content='+ encodeURIComponent(file)
  ).fail(function(){ alert("Erreur pendant la sauvegarde"); })
  .done(function(){
    populate_menu_js(file);
    editAreaLoader.setFileEditedMode(editor, file_id, false);
  });
}

function load_file_click()
{
  var file_id = $(this).attr('file_id');
  var file_type = $(this).attr('file_type');
  var file_syntax='html';
  switch(file_type)
  {
    case 'application/javascript': file_syntax='js'; break;
    case 'text/css': file_syntax='css'; break;
  }
  var file_title = $(this).find('a').text();
  $.get(
    '/cms/ajax/get_file_content?roid='+ file_id,
    function(data) {
      editAreaLoader.openFile('editor',{
        id: file_id,
        title: file_title,
        text: data,
        syntax: file_syntax,
        do_highlight: true
    });
  });
}

cursor_positions = {};

function switch_off_file(file)
{
  cursor_positions[file.id] = editAreaLoader.getSelectionRange('editor');
}

function switch_on_file(file)
{
  $('#menu_content li').removeClass('active');
  var active_item = $('#menu_content li[file_id='+ file.id +']');
  active_item.addClass('active');
  $('#menu_title li[file_type]').addClass('hide');
  $('#menu_title li[file_type='+ file.syntax +']').removeClass('hide');
  if(file.syntax == 'js')
  {
    populate_menu_js(file.text);
  }
  editAreaLoader.setSelectionRange('editor', cursor_positions[file.id]['start'], cursor_positions[file.id]['end']);
}

function populate_menu_js(content)
{
  var functions = [];
  var s = 0;
  while(true)
  {
    var i = content.indexOf("function ", s);
    if(i == -1) break;
    var j = content.indexOf("(", i);
    if(j == -1) break;
    var function_name = content.substring(i+9, j);
    if(function_name)
    {
      functions.push({
        pos: i,
        name: function_name
      });
    }
    s = j+2;
    if(s > content.length) break;
  }
  var s = "";
  functions.sort(function(a,b){return a.name>b.name;});
  for(var i=0; i<functions.length; ++i)
  {
    s += '<li position="'+ functions[i].pos +'"><a href="#">'+ functions[i].name +'</a></li>';
  }
  $('#menu_js ul').html(s);
  $('#menu_js li').click(js_position_click);
}

function js_position_click()
{
  var pos = $(this).attr('position');
  editAreaLoader.setSelectionRange('editor', pos, pos);
}

function change_menu()
{
  var active = $(this).attr('file_type');
  $('#menu_title li').removeClass('active');
  if(active)
  {
    $('#menu_title li[file_type='+ active +']').addClass('active');
  } else {
    $('#menu_title li[files]').addClass('active');
  }
  $('#menu_content > div').addClass('hide');
  if(active)
  {
    $('#menu_'+ active).removeClass('hide');
  } else {
    $('#menu_files').removeClass('hide');
  }
}

function close_file(file)
{
  if(file.edited)
  {
    return confirm("Les modifications sur le fichier "+ file.title +" vont être perdues. Etes-vous sûr de vouloir le fermer ?");
  }
  return true;
}

function check_before_closing_window()
{
  var files = editAreaLoader.getAllFiles('editor');
  var ok=true;
  for(id in files)
  {
    if(files[id].edited)
    {
      ok = false;
      break;
    }
  }
  if(!ok)
  {
      return "Au moins un fichier n'a pas été enregistré. Etes-vous sûr de vouloir fermer la fenêtre ?";
  }
}

$(function(){
	editAreaLoader.init({
		id : "editor",
		is_multi_files: true,
		allow_toggle: false,
		toolbar: 'save,|,search, go_to_line, |, undo, redo, |, select_font,|, change_smooth_selection, highlight, reset_highlight, word_wrap, |, help',
		save_callback: 'save_file',
		EA_file_switch_off_callback: 'switch_off_file',
		EA_file_switch_on_callback: 'switch_on_file',
		EA_file_close_callback: 'close_file'
	});
	$(window).resize(autosize);
	autosize();
	$('#menu_content li').click(load_file_click);
	$(window).bind('beforeunload', check_before_closing_window);
	$('#menu_title li').click(change_menu);
});