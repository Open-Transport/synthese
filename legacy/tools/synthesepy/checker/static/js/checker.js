$(function() {
  $(".alert").alert();

  $("form").submit(function() {
    setTimeout(function() {
      $("#modal-waiting-dialog").modal({backdrop: true, show: true});
    }, 300);
  });
});
