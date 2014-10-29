$(document).ready(function() {

    $(function() {
        var collectionListesLectures = new ColListesLectures({});
        collectionListesLectures.url = 'http://localhost:5000/tasks';

        var vueListesLectures = new VueListesLectures({
            collection: collectionListesLectures
        }); 

        collectionListesLectures.fetch();
    });

    _.mixin({templateFromUrl: function (url, data, settings) {
        var templateHtml = "";
        this.cache = this.cache || {};

        if (this.cache[url]) {
            templateHtml = this.cache[url];
        } else {
            $.ajax({
                url: url,
                method: "GET",
                async: false,
                success: function(data) {
                    templateHtml = data;
                }
            });

            this.cache[url] = templateHtml;
        }

        return _.template(templateHtml, data, settings);
    }});

/*$('#boutonAjouterListe').click(function(event){
    postListesLectures({ task: $('#txtNouvListesLectures').val()});
})

function getListesLectures() {
    //demander les tâches
    $.ajax({
        url:'http://localhost:5000/tasks',
        type: 'GET'
    })
    .done(function(data){
        //Une fois les données reçu...
        
        //vider la table des données actuelles
        $('#tableListesLectures').empty();

        //Boucler sur l'array 'tasks' pour remplir la table
        //Donner au tr (tablerow) le meme id que la tâche
        for (var i=0; i<data.tasks.length; i++) {
            $('#tableListesLectures').append('<tr class="tableRowItem" id="' + data.tasks[i].id + '"> \
                                        <td class="listname"> \
                                            ' + data.tasks[i].task + ' \
                                        </td> \
                                        <td>\
                                            <span class="glyphicon glyphicon-trash"></span>\
                                            <span class="glyphicon glyphicon-pencil"></span>\
                                        </td> \
                                    </tr>');
        }

        //Programmer le click pour l'icone "trash"
        $('.glyphicon-trash').click(function(){
            //aller chercher l'element qui contient l'id
            var gparent = $(this).parent().parent(); 
            
            //appeler la fonction avec comme param l'id 
            deleteListesLectures(extractId(gparent));
        })
      
        //Programmer le click pour l'icone "edit"
        $('.glyphicon-pencil').click(function(){
            //aller chercher l'element qui contient l'id
            var id = extractId($(this).parent().parent())
            var listname = $.trim($('#'+id+' .listname').text());
            
            //Modifier le contenu du tr pour avoir un champ d'édition avec un bouton save
            $('#'+id).html('<td colspan=2 class="input-group input-group-sm"> \
                                <input type="text" class="form-control"> \
                                    <span class="input-group-btn"> \
                                        <button class="btn btn-primary" type="button"> \
                                            <span class="glyphicon glyphicon-floppy-disk"></span> \
                                        </button> \
                                    </span> \
                            </td> \
                            <td> \
                                <span id="modifPoubelle" class="glyphicon glyphicon-trash"></span> \
                            </td> ');
            $('#'+id+' .form-control').val(listname);

            //Programmer le bouton save
            $('.glyphicon-floppy-disk').click(function() {
                var parentBtn = $(this).parent().parent().parent().parent();
                var parentTxt = $(this).parent().parent().parent().find(".form-control");
                putListesLectures(extractId(parentBtn), {task: $('#'+extractId(parentBtn)+' .form-control').val()});
            })
        })

        //Programmer le double-click pour les items de la liste
        $('.tableRowItem').click(function() {
            $('#table_listes_lectures').css('display','none');
            $('#table_contenu_liste').css('display','block');
        })

    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function postListesLectures(data) {
    $.ajax({
        url:'http://localhost:5000/tasks',
        type: 'POST',
        data: JSON.stringify(data),
        contentType: "application/json;charset=UTF-8"
    })
    .done(function(data){
        //Do something with returned data
        task: $('#txtNouvListesLectures').val('');
        getListesLectures();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function putListesLectures(id, data) {
    $.ajax({
        url:'http://localhost:5000/tasks/'+parseInt(id),
        type: 'PUT',
        data: JSON.stringify(data),
        contentType: 'application/json'
    })
    .done(function(data){
        //Do something with returned data
        getListesLectures();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function deleteListesLectures(id) {
    $.ajax({
        url:'http://localhost:5000/tasks/'+parseInt(id),
        type: 'DELETE',
    })
    .done(function(data){
        //Do something with returned data
        getListesLectures();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

getListesLectures();

function extractId(element) {
    return element.attr('id');
}*/

});