function openModal() {
  var modal = $('#'+$(this).attr('data-form-action')+'Modal');
  var folderid = $(this).attr('data-id') ? $(this).attr('data-id') : $(this).parents('[data-id]').attr('data-id');
  if (typeof folderid != 'undefined') {
    modal.find('[name="roid"]').val(folderid);
    modal.find('[name="roid"]').removeAttr('disabled');
  } else {
    modal.find('[name="roid"]').attr('disabled', true);
  }
  modal.find('[name="view_mode"]').val($('#view-mode').attr('data-active-mode'));

  eval($(this).attr('data-form-action')+"Form")($(this), folderid, modal);
  return false;
}

function addFolderForm(elem, folderid, modal) {
  var parent = elem.attr('data-parent');
  modal.find('[name="actionParampi"]').val(folderid);
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

function removeFolderForm(elem, folderid, modal) {
  modal.find('[name="actionParam_object_id"]').val(folderid);
  modal.modal('show');
}

function renameFolderForm(elem, folderid, modal) {
  modal.find('#renameFieldModal').val(elem.parents('a').attr('data-title'));
  modal.find('[name="actionParamwp"]').val(folderid);
  modal.modal('show');
}

function removeFileForm(elem, folderid, modal) {
  modal.find('[name="actionParam_object_id"]').val(elem.attr('id'));
  modal.modal('show');
}

function uploadFileForm(elem, folderid, modal) {
  modal.find('[name="actionParam_up_id"]').val(folderid);
  modal.modal('show');
}

$(function(){
  $('[data-form-action]').click(openModal);
  $(document).on('click', '[data-form-action]', openModal);
  $('[name="addFolder"]').validate();
});
