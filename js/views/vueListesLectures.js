$(function() {
	VueListesLectures = Backbone.View.extend({
		
		//template: _.templateFromUrl("http://localhost:63342/LabTests1/templates/templateListesLecturesRegulier.html"),
		template: _.template($('#templateListeLecture').html()),
		el: '#div_listes_lectures',
		events: {
			"click #boutonAjouterListe" : "gestEvenBtnAjouter",
			"click .glyphicon-pencil" : "gestEvenBtnModifier",
			"click .glyphicon-floppy-disk" : "gestEvenBtnSauvegarder",
			"click .glyphicon-trash" : "gestEvenBtnSupprimer"
		},
		initialize: function () {
			_.bindAll(this, 'render');

			var self = this;

			this.collection.bind('sync add remove', function() {
				self.render();
			});
		},
		render: function () {
			this.$el.html(this.template({
				tasks: this.collection.toJSON()
			}));
		},
		gestEvenBtnAjouter: function(event) {
			this.collection.create({
				id: Math.floor((Math.random() * 5000) + 1),
				task: $('#txtNouvListesLectures').val()
			}, {
				type: 'POST'
			})
		},
		gestEvenBtnModifier: function(event) {
			var idListe = $(event.target).data('id');
			var modele = this.collection.get(idListe);

		},
		gestEvenBtnSauvegarder: function (event) {
			var idListe = $(event.target).data('id');
			var modele = this.collection.get(idListe);

			modele.save({
				"task": $(event.target).parent().parent().parent().find('.form-control').val()
			})
		},
		gestEvenBtnSupprimer: function(event) {
			var idListe = $(event.target).data('id');
			var modele = this.collection.get(idListe);

			modele.destroy();
			this.collection.remove(idListe);
		}

	})
});