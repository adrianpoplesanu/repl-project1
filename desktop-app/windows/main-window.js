console.log("buna dimineata!!!");

const Path = require("path");
const FS   = require("fs");
let Files  = [];

function ThroughDirectory(Directory) {
    FS.readdirSync(Directory).forEach(File => {
        const Absolute = Path.join(Directory, File);
        //if (FS.statSync(Absolute).isDirectory()) return ThroughDirectory(Absolute);
        //else return Files.push(Absolute);
        Files.push(Absolute);
    });
}

ThroughDirectory("/");
console.log(Files);