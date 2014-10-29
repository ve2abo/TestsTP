$(document).ready(function() {
	(function() {
		ModeleUtilisateur = Backbone.Model.extend({
			defaults: {
				id: '',
			    name: 'Invité',
			    email: '',
			    password: '',
			    following: []			
			},
			parse: function(response) {
				this.id = response.id;
				return response;
			}			
		});
	})();
});