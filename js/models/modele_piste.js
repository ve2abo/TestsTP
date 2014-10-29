$(document).ready(function() {
	(function() {
		ModelePiste = Backbone.Model.extend({
			defaults: {
			    artistId: '',
			    collectionId: '',
			    trackId: '',
			    artistName: '',
			    collectionName: '',
			    trackName: '',
			    collectionCensoredName: '',
			    trackCensoredName: '',
			    artistViewUrl: '',
			    collectionViewUrl: '',
			    trackViewUrl: '',
			    previewUrl: '',
			    artworkUrl30: '',
			    artworkUrl60: '',
			    artworkUrl100: '',
			    collectionPrice: '',
			    trackPrice: '',
			    releaseDate: '',
			    discCount: '',
			    discNumber: '',
			    trackCount: '',
			    trackNumber: '',
			    trackTimeMillis: '',
			    country: '',
			    currency: '',
			    primaryGenreName: ''			},
			parse: function(response) {
				this.trackId = response.trackId;
				return response;
			}
		});
	})();
});