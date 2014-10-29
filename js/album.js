$(function () {
    $('#pistes li').mouseenter(function () {
        $(this).find('.numeroPiste span').addClass('hide');
        $(this).find('a.btn-play').removeClass('hide');
    });
    
    $('#pistes li').mouseleave(function () {
        $(this).find('.numeroPiste span').removeClass('hide');
        $(this).find('a.btn-play').addClass('hide');
    });
});