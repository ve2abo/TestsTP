var ubeatClient = {
    views: {},

    loadTemplates: function(views, callback) {

        var deferreds = [];

        $.each(views, function(index, view) {
            if (ubeatClient['Vue' + view]) {
                deferreds.push($.get('tpl/' + view + '.html', function(data) {
                    ubeatClient['Vue' + view].prototype.template = _.template(data);
                }, 'html'));
            } else {
                alert('Vue' + view + " not found");
            }
        });

        $.when.apply(null, deferreds).done(callback);
    }
}

ubeatClient.Router = Backbone.Router.extend({
    routes: {
        "":                 "accueil" ,
        "auth":             "connexion",
        //"employees/:id":    "employeeDetails"
    },

    initialize: function () {
        ubeatClient.vueMenu = new ubeatClient.VueMenu();
        $('#menu_barre').html(ubeatClient.vueMenu.render().el);
    },

    accueil: function () {
        // Since the home view never changes, we instantiate it and render it only once
        if (!ubeatClient.vueAccueil) {
            ubeatClient.vueAccueil = new ubeatClient.VueAccueil();
            ubeatClient.vueAccueil.render();
        } else {
            console.log('reusing home view');
            ubeatClient.vueAccueil.delegateEvents(); // delegate events when the view is recycled
        }
        this.$content.html(ubeatClient.vueAccueil.el);
        //ubeatClient.vueMenu.selectMenuItem('home-menu');
    } ,

    connexion: function () {
        if (!ubeatClient.vueConnexion) {
            ubeatClient.vueConnexion = new ubeatClient.VueConnexion();
            ubeatClient.vueConnexion.render();
        }
        this.$content.html(ubeatClient.vueConnexion.el);
        //ubeatClient.vueMenu.selectMenuItem('contact-menu');
    }/*,

    employeeDetails: function (id) {
        var employee = new directory.Employee({id: id});
        var self = this;
        employee.fetch({
            success: function (data) {
                console.log(data);
                // Note that we could also 'recycle' the same instance of EmployeeFullView
                // instead of creating new instances
                self.$content.html(new directory.EmployeeView({model: data}).render().el);
            }
        });
        directory.shellView.selectMenuItem();
    }*/

});

$(document).ready(function() {
    ubeatClient.loadTemplates(["Accueil", "Connexion", "Menu", "LesListesLectures", "UneListeLectures", "Utilisateurs", "Parametres"],
        function () {
            ubeatClient.router = new ubeatClient.Router();
            Backbone.history.start();
        }
    );


    /*Informations user test harcodées :
    {
        "email":"user@team8.org",
        "name":"USER_TEAM8",
        "following":[],
        "id":"544eeb3633e94d02006b2ff1"
    }

    {
        "token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NDRlZWIzNjMzZTk0ZDAyMDA2YjJmZjEiLCJleHAiOjE0MTQ1NDkwMzQyOTR9.Uy7e7W2UrcYTxKqP_LHPpXIaalHrC8H5DPZxAwqvZUo",
        "email": "user@team8.org",
        "name": "USER_TEAM8",
        "following": [],
        "id": "544eeb3633e94d02006b2ff1"
    }
    */

/*
    var collectionListesLectures = new ColListesLectures({});
    collectionListesLectures.url = 'http://localhost:5000/tasks';

    vueListesLectures = new VueListesLectures({
        collection: collectionListesLectures
    }); 
    collectionListesLectures.fetch();
*/

    //Gestion du champ de recherche
    //document.getElementById("menu_formulaire_rech").action = "views/resultat_rech.html";
    //document.getElementById("menu_formulaire_rech").target = "cadre_principal";

});

