ubeatClient.VueUneListeLectures = Backbone.View.extend({
    template: _.template($('#templateUneListeLecture').html()),
    el: '#cadre_principal',
    events: {
        "click .glyphicon-play" : "gestEventBtnPlay",
        "click .glyphicon-trash" : "gestEventBtnSupprimer",
        "click .glyphicon-pause" : "gestEventBtnPause"
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
            listeLecture: this.collection.toJSON()
        }));
    },
    gestEventBtnPlay: function(event) {
        var nomPiste = $(event.target).data('trackName');
        var nomArtiste = $(event.target).data('artistName');
        var nomAlbum = $(event.target).data('collectionName');
        var duree = $(event.target).data('trackTimeMillis');
        var urlPreview = $(event.target).data('previewUrl');
        
        console.log("Play: " + nomPiste + " " + nomArtiste + " " + nomAlbum + " " + duree + "urlPreview");
        
        //Fonction pour jouer l'URL preview...

    },
    gestEventBtnSupprimer: function(event) {
        var idListe = $(event.target).data('trackId');
        var modele = this.collection.get(idListe);

        modele.destroy();
        this.collection.remove(idListe);
    },
    gestEventBtnPause: function(event) {
        var nomPiste = $(event.target).data('trackName');
        var nomArtiste = $(event.target).data('artistName');
        var nomAlbum = $(event.target).data('collectionName');
        var duree = $(event.target).data('trackTimeMillis');
        var urlPreview = $(event.target).data('previewUrl');
        
        console.log("Pause: " + nomPiste + " " + nomArtiste + " " + nomAlbum + " " + duree + "urlPreview");
        
        //Fonction pour arrêter l'URL preview...
    }
});