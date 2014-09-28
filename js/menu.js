function click_connexion() {
    document.getElementById('cadre_principal').src = "views/authentification.html";
}

function click_deconnexion() {
    document.getElementById('cadre_principal').src = "views/authentification.html";
    document.getElementById('menu_item_btn_conn').style.display = "block";
    document.getElementById('menu_item_btn_deconnexion').style.display = "none";
    /**document.getElementById('menu_item_util_courant').innerHTML = '<img src="img/guest.png" id="menu_icone_utilisateur" />&nbsp;Invité';**/
    document.getElementById('menu_item_util_courant').innerHTML = '<div id="menu_user_name">Invité</div> <img src="img/guest.png" id="menu_icone_utilisateur" />';
    /**document.getElementById('menu_item_util_courant').innerHTML = '<i class="fa fa-question fa-2x"></i>&nbsp;Invité';**/
}

function click_accueil() {
    document.getElementById('cadre_principal').src = "views/accueil.html";
}

function click_listes_lectures() {
    document.getElementById('cadre_principal').src = "views/liste_lectures.html";
}

function click_utilisateurs() {
    document.getElementById('cadre_principal').src = "views/utilisateurs.html";
}

function click_parametres() {
    document.getElementById('cadre_principal').src = "views/parametres.html";
}

//Gestion du champ de recherche
document.getElementById("menu_formulaire_rech").action = "views/resultat_rech.html";
document.getElementById("menu_formulaire_rech").target = "cadre_principal";