function displayPlayBtn(element) {
    var btn = element.getElementsByClassName("btn-play");
    var numeroPiste = element.getElementsByTagName("span");
    btn[0].style.display = "block";
    numeroPiste[0].style.display = "none";
}

function hidePlayBtn(element) {
    var btn = element.getElementsByClassName("btn-play");
    var numeroPiste = element.getElementsByTagName("span");
    btn[0].style.display = "none";
    numeroPiste[0].style.display = "block";
}


