




$(function() {
	function format(row) {
    var comment = row.comment ? "* " + row.comment : '';
		return " &lt;" + row.subject + "&gt;" + row.label + comment;
	}
solr_property = "http://vmgal49.nuigalway.ie:8983/solr/select?fq=type:property&indent=on&version=2.2&start=0&rows=10&fl=*%2Cscore&qt=standard&wt=json&explainOther=&hl.fl=&json.wrf=?";
solr_class = "http://vmgal49.nuigalway.ie:8983/solr/select?fq=type:class&indent=on&version=2.2&start=0&rows=10&fl=*%2Cscore&qt=standard&wt=json&explainOther=&hl.fl=&json.wrf=?";

	$(".autocomplete-property").autocomplete(solr_property, {
		multiple: false,
		width: 410,
    dataType: "jsonp", 
		parse: function(data) {
//console.log(data.response.docs);
			return $.map(eval(data.response.docs), function(row) {
				return {
					data: row,
					value: row.subject,
					result: row.subject
				}
			});
		},
		formatItem: function(item) {
			return format(item);
		}
	}).result(function(e, item) {
		$("#content").append("<p>selected " + format(item) + "</p>");
	});


	$(".autocomplete-class").autocomplete(solr_class, {
		multiple: false,
		width: 410,
    dataType: "jsonp", 
		parse: function(data) {
//console.log(data.response.docs);
			return $.map(eval(data.response.docs), function(row) {
				return {
					data: row,
					value: row.subject,
					result: row.subject
				}
			});
		},
		formatItem: function(item) {
			return format(item);
		}
	}).result(function(e, item) {
		$("#content").append("<p>selected " + format(item) + "</p>");
	});


});






