function def() {
	var elementsG = document.getElementsByClassName("group"); // trouve toutes les images
	
	elementsG[0].style.display = "block"; // affiche la nouvelle image
}

function link() {
	var link = document.getElementById("iTunesLink");
	link[0].style.display = "block";
}


function changerImage(fleche) {
    var elementsG = document.getElementsByClassName("group"); // trouve toutes les images

    
	// trouve l'ID de l'image et du bouton présentement affichés
    var groupID = imageVisible(elementsG);
    
	// cache l'image présentement affichée et change le background du bouton de position actuel (neutre)
	elementsG[groupID].style.display = "none"; 
	
    if(!fleche) {
        var nouvelleImage = prev(groupID, elementsG.length); // trouve l'image précédente

    } else {
        var nouvelleImage = next(groupID, elementsG.length); // trouve l'image suivante
	
    }
    elementsG[nouvelleImage].style.display = "block"; // affiche la nouvelle image
}


window.paused = false;

// fonction play / pause
function arret() {
    var elementsG = document.getElementsByClassName("group");
	var groupID = imageVisible(elementsG);
	
	if(!window.paused) {
        clearInterval(switching);
		elementsG[groupID].style.opacity = "0.9";
    } else {
		elementsG[groupID].style.opacity = "1";
        switching = setInterval("changerImage(true)", temps);
		
    }
    window.paused = !(window.paused);
}


// renvoit l'ID de l'image actuelle
function imageVisible(elementsG) {
    var groupID = -1;
    for(var i = 0; i < elementsG.length; i++) {
        if(elementsG[i].style.display == "block") {
            groupID = i;
        }
    }
    return groupID;
}


// trouve l'image précédente
function prev(num, elementsGLength) {
    if(num == 0) return elementsGLength-1;
    else return num-1;
}


// trouve l'image suivante
function next(num, elementsGLength) {
    if(num == elementsGLength-1) return 0;
    else return num+1;
}


var temps = 5000; // l'image change automatiquement chaque 5 sec.
var switching = setInterval("changerImage(true)", temps);

window.onload = function() {
  def();
};