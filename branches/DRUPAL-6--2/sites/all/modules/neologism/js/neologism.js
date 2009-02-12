if (Drupal.jsEnabled) {
  $(document).ready(
    function neo_autofill() {
      $('#edit-title').bind('blur', function() {
        if ($('#edit-field-label-0-value').val() == '') {
          $('#edit-field-label-0-value').val(this.value);
        }
      });
    }
  );
}

if (Drupal.jsEnabled) {
  $(document).ready(
      function(){
	    $("#tree").treeview({
          persist: "location",
          collapsed: true,
          control: "#treecontrol",
          animated: "fast",
        });
      }
  );
}
