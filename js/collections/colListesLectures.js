$(document).ready(function() {
	$(function() {
		ColListesLectures = Backbone.Collection.extend({
		    model: ModeleListeLectures,
		    parse: function(response) {
		        return response.tasks;
		    }
		});
	});

});