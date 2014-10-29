$(document).ready(function() {

/*$('#boutonAjouter').click(function(event){
    postTask({ task: $('#txtNouvTache').val()});
})

function getTasks() {
    //demander les tâches
    $.ajax({
        url:'http://localhost:5000/tasks',
        type: 'GET'
    })
    .done(function(data){
        //Une fois les données reçu...
        
        //vider la table des données actuelles
        $('#tasktable').empty();

        //Boucler sur l'array 'tasks' pour remplir la table
        //Donner au tr (tablerow) le meme id que la tâche
        for (var i=0; i<data.tasks.length; i++) {
            $('#tasktable').append('<tr id="' + data.tasks[i].id + '"> \
                                        <td class="input-group"> \
                                            <input type="text" class="form-control"> \
                                            <span class="input-group-btn"> \
                                                <button class="btn btn-primary" type="button"> \
                                                    <span class="glyphicon glyphicon-pencil"></span> \
                                                </button> \
                                            </span> \
                                        </td> \
                                        <td id="actionTD"> \
                                            <span class="glyphicon glyphicon-trash"></span> \
                                        </td> \
                                    </tr>');
            $('#'+data.tasks[i].id+' .form-control').val(data.tasks[i].task);
        }

//glyphicon glyphicon-pencil
//glyphicon glyphicon-floppy-disk

        $('.glyphicon-pencil').click(function() {
            var parentBtn = $(this).parent().parent().parent().parent();
            var parentTxt = $(this).parent().parent().parent().find(".form-control");
            console.log($('#'+extractId(parentBtn)+' .form-control').val());
            putTask(extractId(parentBtn), {task: $('#'+extractId(parentBtn)+' .form-control').val()});
        })

        $('.glyphicon-floppy-disk').click(function() {
            var parentBtn = $(this).parent().parent().parent().parent();
            var parentTxt = $(this).parent().parent().parent().find(".form-control");
            console.log($('#'+extractId(parentBtn)+' .form-control').val());
            putTask(extractId(parentBtn), {task: $('#'+extractId(parentBtn)+' .form-control').val()});
        })


        //Programmer le click pour l'icone "trash"
        $('.glyphicon-trash').click(function(){
            //aller chercher l'element qui contient l'id
            var gparent = $(this).parent().parent(); 
            
            //appeler la fonction avec comme param l'id 
            deleteTask(extractId(gparent));
        })
      
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function postTask(data) {
    $.ajax({
        url:'http://localhost:5000/tasks',
        type: 'POST',
        data: JSON.stringify(data),
        contentType: "application/json;charset=UTF-8"
    })
    .done(function(data){
        //Do something with returned data
        task: $('#txtNouvTache').val('');
        getTasks();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function putTask(id, data) {
    $.ajax({
        url:'http://localhost:5000/tasks/'+parseInt(id),
        type: 'PUT',
        data: JSON.stringify(data),
        contentType: 'application/json'
    })
    .done(function(data){
        //Do something with returned data
        getTasks();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

function deleteTask(id) {
    $.ajax({
        url:'http://localhost:5000/tasks/'+parseInt(id),
        type: 'DELETE',
    })
    .done(function(data){
        //Do something with returned data
        getTasks();
    })
    .fail(function(jqXHR, textStatus){
        //Do something about the error
        console.log(textStatus);
    });
}

getTasks();

function extractId(element) {
    return element.attr('id');
}*/

});