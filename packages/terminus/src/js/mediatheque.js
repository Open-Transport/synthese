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

  var modalForm = window[elem.attr('data-form-action')+"Form"];
  if (typeof modalForm != 'undefined') {
    modalForm(elem, folderid, modal);
  }

  modal.on("shown.bs.modal", function() {
    // set focus on data-form-default
    modal.find('button[data-form-default="true"]').focus();
  });

  modal.modal('show');
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
}

function removeFolderForm(elem, folderid, modal) {
  modal.find('[name="actionParam_object_id"]').val(folderid);
}

function renameFolderForm(elem, folderid, modal) {
  modal.find('#renameFieldModal').val(elem.parents('a').attr('data-title'));
  modal.find('[name="actionParamwp"]').val(folderid);
}

function removeFileForm(elem, folderid, modal) {
  modal.find('[name="actionParam_object_id"]').val(elem.attr('id'));
}

function uploadFileForm(elem, folderid, modal) {
  modal.find('[name="actionParam_up_id"]').val(folderid);
  modal.find('[name="actionParam_content"]').attr('accept', $('#selectMediaLibrary').children('option:selected').attr('data-accepted-types'));
}

function unlockFolder() {
  var elem = $(this).parents('[data-id]');
  var folderId = elem.attr('data-id');
  $.post('/terminus/ajax/set_folder_lock_removal?roid=' + folderId,
         'value=removable').done(function() {
           elem.find('[data-action="unlockFolder"]:first').hide()
           elem.find('[data-action="lockFolder"]:first').show()
           elem.find('[data-form-action="removeFolder"]:first').show()
         });
}

function lockFolder() {
  var elem = $(this).parents('[data-id]');
  var folderId = elem.attr('data-id');
  $.post('/terminus/ajax/set_folder_lock_removal?roid=' + folderId,
         'value=').done(function() {
           elem.find('[data-action="unlockFolder"]:first').show()
           elem.find('[data-action="lockFolder"]:first').hide()
           elem.find('[data-form-action="removeFolder"]:first').hide()
         });
}

$(function(){
  $(document).on('click', '[data-form-action]', openModal);
  $(document).on('click', '[data-action="unlockFolder"]', unlockFolder);
  $(document).on('click', '[data-action="lockFolder"]', lockFolder);
  $('[name="addFolder"]').validate();
});
