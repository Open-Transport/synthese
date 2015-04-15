function openModal() {
  eval($(this).attr('data-action')+"Form")($(this));
  return false;
}

function addFolderForm(elem) {
  console.log('addFolderForm');
  var parent = elem.attr('data-parent'),
      modal = $('#'+elem.attr('data-action')+'Modal');
  switch(parent) {
    case 'pi':
      modal.find('[name="actionParampi"]').removeAttr('disabled');
      modal.find('[name="actionParamsi"]').attr('disabled', true);
      break;
    case 'si':
    default:
      modal.find('[name="actionParamsi"]').removeAttr('disabled');
      modal.find('[name="actionParampi"]').attr('disabled', true);
      break;
  }
  modal.modal('show');
}

function removeFolderForm(elem) {
  console.log('removeFolderForm');
  var modal = $('#'+elem.attr('data-action')+'Modal');
  modal.modal('show');
}

function renameFolderForm(elem) {
console.log('renameFolderForm');
  var modal = $('#'+elem.attr('data-action')+'Modal');
  modal.find('#renameFieldModal').val(elem.parent('a').text());
  modal.modal('show');
}

function removeFileForm(elem) {
  console.log('removeFileForm');
  var modal = $('#'+elem.attr('data-action')+'Modal');
  console.log(elem.attr('id'));
  modal.find('[name="actionParam_object_id"]').val(elem.attr('id'));
  modal.modal('show');
}

function uploadFileForm(elem) {
  console.log('uploadFileForm');
  var modal = $('#'+elem.attr('data-action')+'Modal');
  modal.modal('show');
  
}

$(function(){
  $('[data-action]').click(openModal);
  
  $('[name="addFolder"]').validate();
});