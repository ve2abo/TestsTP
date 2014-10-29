ubeatClient.VueAccueil = Backbone.View.extend({
    template: _.template($('#templateAccueil').html()),
    el: '#cadre_principal',
    events: {
    },
    initialize: function () {
        _.bindAll(this, 'render');

        var self = this;

        //Décommenter pour que cette vue soit celle par défaut du cadre.
        //Une seule vue utilisant '#cadre_principal' devrait décommenter cette ligne. 
        if (utilisateurCourant !== undefined) {
            self.render();
        }
    },
    render: function () {
        this.$el.html(this.template({}));
    }
});