$(function() {
    $.ajax({
	url: "README.md",
	dataType: 'text',
	success: function(data) {
	    console.log(marked(data))
	    $("#readme").html(marked(data));
	    // $("img").each(function(index) {
	    // 	this.id = index
	    // 	$(this).after("<button type='button' onclick='selectImage(" + index + ")'>" + this.title + "</button><br>");
	    // });
	    // selectImage("0");
	},
	error: function(xhr, status, error) {
	    $("#readme").html("There was a problem fetching the project description. Try looking for it on Github instead!");
	}
    });
});

function selectImage(id) {
    $("img").hide();
    $("img#" + id).show();
}
