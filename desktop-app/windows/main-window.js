console.log("buna dimineata!!!");

const Path = require("path");
const FS   = require("fs");

const { ipcRenderer } = require('electron')
//console.log(ipcRenderer.sendSync('synchronous-message', 'pingus')); // prints "pong"

const app_os = ipcRenderer.sendSync('synchronous-message', 'getPlatform');
//console.log(app_os);

let folder_structure = [];
let reference_structure = {};
if (app_os == 'macos' || app_os == 'linux') {
    folder_structure.push ({
        'absolute_path': '/',
        'state': 'closed',
        'children': [],
        'type': 'folder'
    });
}
if (app_os == 'win') {
    folder_structure.push ({
        'absolute_path': 'C://',
        'state': 'closed',
        'children': [],
        'type': 'folder'
    });
}

function openFolder(element) {
    let absolute_path = element['absolute_path'];
    let files = [];
    element['children'] = [];
    FS.readdirSync(absolute_path).forEach(file => {
        let path = absolute_path + '/' + file;
        let type = 'file';
        if (absolute_path.endsWith('/')) {
            path = absolute_path + file;
        }
        if (FS.lstatSync(path).isDirectory()) {
            type = 'folder';
        }
        element['children'].push({
            'absolute_path': path,
            'state': 'closed',
            'children': [],
            'type': type
        });
    });
    element['state'] = 'open';
}

function closeFolder(element) {
    element['state'] = 'closed';
    element['children'] = [];
}

function getElementByAbsolutePath(absolute_path) {
    let tokens = absolute_path.split('/');
    if (tokens[0] == '') {
        tokens[0] = '/';
    }
    //console.log(tokens);
    var i, j, walking_path = '', current_children = folder_structure, current = folder_structure[0];
    for (i = 0; i < tokens.length; i++) {
        if (walking_path.endsWith('/') || walking_path == '') {
            walking_path += tokens[i];
        } else {
            walking_path += '/' + tokens[i];
        }
        //console.log(walking_path);
        for (j = 0; j < current_children.length; j++) {
            if (current_children[j].absolute_path == walking_path) {
                current = current_children[j];
                current_children = current_children[j].children;
                break;
            }
        }
    }
    //current.status = 'bla bla bla'; // this is a reference, and it actually works
    return current;
}

function getElementParent() {
    //... strip last token after the last '/', that's the parent, it can be reference from reference_structure
}

function test_logic() {
    // this works, this can be extended
    openFolder(folder_structure[0]);
    reference_structure[folder_structure[0].absolute_path] = folder_structure[0];
    openFolder(folder_structure[0].children[13]);
    reference_structure[folder_structure[0].children[13].absolute_path] = folder_structure[0].children[13];
    openFolder(folder_structure[0].children[13].children[3]);
    reference_structure[folder_structure[0].children[13].children[3].absolute_path] = folder_structure[0].children[13].children[3];

    openFolder(reference_structure['/Users/adrianpoplesanu'].children[5]);
    reference_structure[reference_structure['/Users/adrianpoplesanu'].children[5].absolute_path] = reference_structure['/Users/adrianpoplesanu'].children[5];
}

function display_folder_structure() {
    //... parse folder_structure and create divs
}

$(document).ready(function () {
    test_logic();
});
