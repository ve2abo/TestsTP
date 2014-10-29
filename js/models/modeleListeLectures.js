$(document).ready(function() {
	(function() {
		ModeleListeLectures = Backbone.Model.extend({
			defaults: {
				id: '',
				task: ''
			},
			parse: function(response) {
				this.id = response.id;
				return response;
			}
		});
	})();
});