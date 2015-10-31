var http = require('http');
var express = require('express');
var exec = require('child_process').exec;

var app = express();

var server = http.createServer(function(req, res) {
    res.setHeader('Content-Type', 'text/plain');
    res.end('Vous etes a l\'accueil');
});

var io = require('socket.io').listen(server);

exec('node -v', function(error, stdout, stderr) {
    console.log('stdout: ' + stdout);
    console.log('stderr: ' + stderr);
    if (error !== null) {
        console.log('exec error: ' + error);
    }
});

app.listen(8080);