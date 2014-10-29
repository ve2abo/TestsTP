ubeatClient.VueParametres = Backbone.View.extend({
    template: _.template($('#templateParametres').html()),
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
});