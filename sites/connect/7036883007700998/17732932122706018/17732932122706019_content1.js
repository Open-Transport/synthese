$(function(){

 $('.date').datepicker();
 $('.ppv').popover();

 $('#update_parameters').submit(function(){
   var form=document.getElementById('update_parameters');
   var parameterStr = '';
   for(var i=0; i<form.elements.length; ++i)
   {
     var input = form.elements[i];
     if(input.getAttribute('is_parameter'))
     {
       if(parameterStr.length)
       {
         parameterStr += '\&';
       }
       parameterStr += input.name +'='+ input.value;
     }
   }
   form.elements['actionParam_field_parameters'].value = parameterStr;
 });

 $('#doc_switch').click(function(){
   var val = ($('#doc_view').css('display')=='block');
   $('#doc_view').css('display', val ? 'none' : 'block');
   $('#doc_edit').css('display', val ? 'block' : 'none');
   $('#doc_cancel').css('display', val ? 'block' : 'none');
   $('#doc_switch').text(val ? 'Enregistrer' : 'Modifier');
   if(!val)
   {
     var ed = tinyMCE.get('mce_editor');
     ed.save();
     var url = $('#mce_editor').attr('save_url');
     $('#doc_view').html(ed.getContent());
     $.ajax({
       url: url,
       data: 'actionParam_field_documentation='+ encodeURIComponent(ed.getContent())
     }).done(function() {
       $(this).addClass("done");
     });
   }
 });

 $('#doc_cancel').click(function(){
   var ed = tinyMCE.get('mce_editor');
   ed.setContent($('#doc_view').html());
   $('#doc_view').css('display', 'block');
   $('#doc_edit').css('display', 'none');
   $('#doc_cancel').css('display', 'none');
   $('#doc_switch').text('Modifier');
 });
 
 tinyMCE.init({
   mode : "specific_textareas",
   editor_selector : "mceEditor",
   theme : "advanced",
   entites:'',
   entity_encoding:'named',
   setup : function(ed) {ed.onBeforeSetContent.add(function(ed, o) {o.content = o.content.replace(/<\?/gi, "&lt;?");o.content = o.content.replace(/\?>/gi, "?&gt;");});},
   plugins : "safari,,save,style,layer,table,advhr,advimage,advlink,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template",
   theme_advanced_buttons1 : "bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,|,styleselect,formatselect,fontselect,fontsizeselect",
   theme_advanced_buttons2 : "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview,|,forecolor,backcolor",
   theme_advanced_buttons3 : "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,iespell,media,advhr,|,print,|,ltr,rtl,|,fullscreen",
   theme_advanced_buttons4 : "insertlayer,moveforward,movebackward,absolute,|,styleprops,|,cite,abbr,acronym,del,ins,attribs,|,visualchars,nonbreaking,template,blockquote",
   theme_advanced_toolbar_location : "top",
   theme_advanced_toolbar_align : "left",
   theme_advanced_statusbar_location : "bottom",
   theme_advanced_resizing : true,
   width:940,
   height:400
 });
});