function openModal() {
  var elem = $(this);
  var modal = $('#'+elem.attr('data-form-action')+'Modal');
  var folderid = elem.attr('data-id') ? elem.attr('data-id') : elem.parents('[data-id]').attr('data-id');
  if (typeof folderid != 'undefined') {
    modal.find('[name="roid"]').val(folderid);
    modal.find('[name="roid"]').removeAttr('disabled');
  } else {
    modal.find('[name="roid"]').attr('disabled', true);
  }
  modal.find('[name="view_mode"]').val($('#view-mode').attr('data-active-mode'));
  modal.find('[name="medialibrary_id"]').val($('#selectMediaLibrary').attr('data-id'));

  eval(elem.attr('data-form-action')+"Form")(elem, folderid, modal);
  return false;
}

function addFolderForm(elem, folderid, modal) {
  var parent = elem.attr('data-parent'),
      parampi = modal.find('[name="actionParampi"]'),
      paramsi = modal.find('[name="actionParamsi"]');
  parampi.val(folderid);
  paramsi.val($('#selectMediaLibrary').attr('data-website-id'));
  switch(parent) {
    case 'pi':
      parampi.removeAttr('disabled');
      paramsi.attr('disabled', true);
      break;
    case 'si':
    default:
      paramsi.removeAttr('disabled');
      parampi.attr('disabled', true);
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
  modal.find('[name="actionParam_content"]').attr('accept', $('#selectMediaLibrary').children('option:selected').attr('data-accepted-types'));
  modal.modal('show');
}

$(function(){
  $(document).on('click', '[data-form-action]', openModal);
  $('[name="addFolder"]').validate();
});
