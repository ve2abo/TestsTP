/*
$(function() {
    VueListesLectures = Backbone.View.extend({
        template: _.template($('#templateListesLectures').html()),
        el: '#cadre_principal',
        events: {
        },
        initialize: function () {
            _.bindAll(this, 'render');

            var self = this;

            //Décommenter pour que cette vue soit celle par défaut du cadre.
            //Une seule vue utilisant '#cadre_principal' devrait décommenter cette ligne. 
            //self.render();
        },
        render: function () {
            this.$el.html(this.template({}));
        }
    })
});
*/
ubeatClient.VueLesListesLectures = Backbone.View.extend({
    template: _.template($('#templateLesListeLecture').html()),
    el: '#cadre_principal',
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
            listesLectures: this.collection.toJSON()
        }));
    },
    gestEvenBtnAjouter: function(event) {
        console.log("Test: " + $('#txtNouvListe').val());
        this.collection.create({
            name: $('#txtNouvListe').val(),
            owner: {
                id: "544eeb3633e94d02006b2ff1",
                name: "USER_TEAM8",
                email: "user@team8.org"
            },
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
});