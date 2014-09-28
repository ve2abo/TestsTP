function click_bouton_connexion() {
    parent.document.getElementById('cadre_principal').src = "views/accueil.html";
    parent.document.getElementById('menu_item_btn_conn').style.display = "none";
    parent.document.getElementById('menu_item_btn_deconnexion').style.display = "block";
    /*parent.document.getElementById('menu_item_util_courant').innerHTML= '<i class="fa fa-headphones fa-2x"></i>&nbsp;' + document.getElementById('zone_connexion_textbox_user').value;
    parent.document.getElementById('menu_item_util_courant').innerHTML= '<img src="img/photo.png" id="menu_icone_utilisateur" />&nbsp;' + document.getElementById('zone_connexion_textbox_user').value;*/
    parent.document.getElementById('menu_item_util_courant').innerHTML = '<div id="menu_user_name">&nbsp;' + document.getElementById('zone_connexion_textbox_user').value +'</div> <img src="img/photo.png" id="menu_icone_utilisateur" />';
}