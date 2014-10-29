$(document).ready(function() {
/*	(function() {
		ModeleListeLecture = Backbone.Model.extend({
			defaults: {
				name: '',
			    owner: '',
			    tracks: ''
			}
		});
	})();
	(function() {
		ModeleListeLecture = Backbone.Model.extend({
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
*/
	(function() {
		ModeleListeLecture = Backbone.Model.extend({
			defaults: {
		        "id": "",
		        "name":"",
		        "owner": {
		            "email": "",
		            "name": "",
		            "id": ""
		        },
		        "tracks": []
			}			
		});
	})();
});

