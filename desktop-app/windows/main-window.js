console.log("buna dimineata!!!");

const Path = require("path");
const FS   = require("fs");
//let Files  = [];
//let Folders = [];

const { ipcRenderer } = require('electron')
console.log(ipcRenderer.sendSync('synchronous-message', 'pingus')) // prints "pong"

function ThroughDirectory(Directory, Files, Folders) {
    FS.readdirSync(Directory).forEach(File => {
        const Absolute = Path.join(Directory, File);
        //if (FS.statSync(Absolute).isDirectory()) return ThroughDirectory(Absolute);
        //else return Files.push(Absolute);
        if (FS.statSync(Absolute).isDirectory()) {
            Folders.push({path: File, absolute: Absolute, subfolder: []})
        } else {
            Files.push({path: File, absolute: Absolute, subfolder: []});
        }
    });
}

//ThroughDirectory("/");
//console.log(Files);

$(document).ready(function () {
    console.log("jquery loaded");
    let Files = [];
    let Folders = [];
    ThroughDirectory("/", Files, Folders);
    console.log(Folders);
    console.log(Files);

    folder_list = [];
    for (var i = 0; i < Folders.length; i++) {
        folder_list.push('<div class="folder item" data-path="' + Folders[i].path + '" data-absolute="' + Folders[i].absolute + '"><span class="glyphicon glyphicon-folder-close folder-color" aria-hidden="true"></span><span class="folder-name">' + Folders[i].path + '</span><div class="folder sub-folder"></div></div>');
    }

    file_list = [];
    for (var i = 0; i < Files.length; i++) {
        file_list.push('<div class="file item"><span class="glyphicon glyphicon-file file-color" aria-hidden="true"></span>' + Files[i].path + '</div>');
    }

    $("#file-explorer").html(folder_list.join('') + file_list.join(''));
});

/*$('.folder').dblclick(function () {
    console.log("dublu click");
});*/

$('#file-explorer').on("click", ".item", function (event) {
    //console.log("click simplu pe folder");
    $('#file-explorer .item').each(function (i) {
        //console.log(it);
        $(this).find(".folder-name").removeClass("highlighted");
    });
    $(this).find(".folder-name").first().toggleClass("highlighted");
    //$("[data-absolute='" + $(this).data('absolute') + "']").toggleClass("highlighted");
});

$('#file-explorer').on("dblclick", ".folder", function () {
    let Files = [];
    let Folders = [];
    ThroughDirectory($(this).data('absolute'), Files, Folders);
    folder_list = [];
    for (var i = 0; i < Folders.length; i++) {
        folder_list.push('<div class="folder item" data-path="' + Folders[i].path + '" data-absolute="' + Folders[i].absolute + '"><span class="glyphicon glyphicon-folder-close folder-color" aria-hidden="true"></span><span class="folder-name">' + Folders[i].path + '</span><div class="folder sub-folder"></div></div>');
    }

    file_list = [];
    for (var i = 0; i < Files.length; i++) {
        file_list.push('<div class="file item"><span class="glyphicon glyphicon-file file-color" aria-hidden="true"></span>' + Files[i].path + '</div>');
    }

    //$("#file-explorer .folder[data-absolute='" + $(this).data('absolute') + "' .sub-folder]").html(folder_list.join('') + file_list.join(''));
    $(this).find(".sub-folder").html(folder_list.join('') + file_list.join(''));
});

$('#file-explorer').on("dblclick", ".file", function () {
    console.log("click dublu pe file");
});
