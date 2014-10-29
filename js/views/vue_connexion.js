ubeatClient.VueConnexion = Backbone.View.extend({
    template: _.template($('#templateZoneConnexion').html()),
    el: '#cadre_principal',
    events: {
        "click #zone_connexion_bouton_connexion" : "gestEvenBtnConnexion"
    },
    initialize: function () {
        _.bindAll(this, 'render');

        var self = this;

        //Décommenter pour que cette vue soit celle par défaut du cadre.
        //Une seule vue utilisant '#cadre_principal' devrait décommenter cette ligne. 
        if (utilisateurCourant === undefined) {
            self.render();
        }
    },
    valider_user: function () {
    },
    render: function () {
        this.$el.html(this.template({}));
    },

    gestEvenBtnConnexion: function(event) {
        var nom_user = $('#zone_connexion_textbox_user').val();
        var password_user = $('#zone_connexion_textbox_password').val();

        if (nom_user != '' && password_user != '')
        {
            utilisateurCourant = nom_user;

            //vueAccueil.render();
            $('#menu_item_btn_conn').css('display','none');
            $('#menu_item_btn_deconnexion').css('display','block');
            $('#menu_item_util_courant').html('<div id="menu_user_name"><i class="fa fa-camera-retro" style="color:#000000"></i> ' + document.getElementById('zone_connexion_textbox_user').value +'</div>');

            //parent.document.getElementById('menu_item_util_courant').innerHTML= '<i class="fa fa-headphones fa-2x"></i>&nbsp;' + document.getElementById('zone_connexion_textbox_user').value;
            //parent.document.getElementById('menu_item_util_courant').innerHTML= '<img src="img/photo.png" id="menu_icone_utilisateur" />&nbsp;' + document.getElementById('zone_connexion_textbox_user').value;
        }
    }
});