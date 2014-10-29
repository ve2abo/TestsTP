 /*   var collectionListesLectures = new ColListesLectures({});
    //collectionListesLectures.url = 'http://ubeat.herokuapp.com/unsecure/playlists';
    collectionListesLectures.url = 'http://ve2abo.ddns.net:3000/unsecure/playlists';


    vueListesLectures = new VueListesLectures({
        collection: collectionListesLectures
    }); 
    collectionListesLectures.fetch();

    //Déclaration de la collection d'une liste de lecture.
    var collectionUneListeLecture = new ColUneListeLecture({});
    collectionUneListeLecture.url = 'http://ve2abo.ddns.net:3000/unsecure/playlists/';

    var vueAuthentification = new VueAuthentification({});
    var vueParametres = new VueParametres({});
    var vueUtilisateurs = new VueUtilisateurs({});
//    var vueListesLectures = new VueListesLectures({});
    var vueAccueil = new VueAccueil({});
*/
ubeatClient.VueMenu = Backbone.View.extend({
    template: _.template($('#templateMenuBarre').html()),
    el: '#menu_barre',
    events: {
        "click #menu_item_logo" : "gestEvenMenuAccueil",
        "click #menu_item_btn_conn" : "gestEvenMenuConnexion",
        "click #menu_item_btn_deconnexion" : "gestEvenMenuDeconnexion",
        "click #menu_item_accueil" : "gestEvenMenuAccueil",
        "click #menu_item_listes_lectures" : "gestEvenMenuListesLestures",
        "click #menu_item_utilisateurs" : "gestEvenMenuUtilisateurs",
        "click #menu_item_parametres" : "gestEvenMenuParametres"
    },
    initialize: function () {
        _.bindAll(this, 'render');

        var self = this;

        self.render();
    },
    render: function () {
        this.$el.html(this.template({}));
    },
    gestEvenMenuConnexion: function(event) {
        vueAuthentification.render();
    },
    gestEvenMenuDeconnexion:function(event) {
        vueAuthentification.render();
        $('#menu_item_btn_conn').css('display','block');
        $('#menu_item_btn_deconnexion').css('display','none');
        $('#menu_item_util_courant').html('<div id="menu_user_name">Invité</div> <img src="img/guest.png" id="menu_icone_utilisateur" />');
        utilisateurCourant = '';
        /*
        //**document.getElementById('menu_item_util_courant').innerHTML = '<img src="img/guest.png" id="menu_icone_utilisateur" />&nbsp;Invité';
        //**document.getElementById('menu_item_util_courant').innerHTML = '<i class="fa fa-question fa-2x"></i>&nbsp;Invité';
        */
    },
    gestEvenMenuAccueil:function(event) {
        vueAccueil.render();
    },
    gestEvenMenuListesLestures:function(event) {
        vueListesLectures.render();
    },
    gestEvenMenuUtilisateurs:function(event) {
        vueUtilisateurs.render();
    },
    gestEvenMenuParametres:function(event) {
        vueParametres.render();
    }
});