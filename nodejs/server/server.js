var http = require('http').Server();
var io = require('socket.io')(http);
//var daemonIO = require("./lib/daemon.io.js")(io);
global.wrapper = require("./lib/wrapper.js");

var port = 8080;

//daemonIO.start();

http.listen(wrapper, port, function(){
    console.log('Server started on *:' + port);
    wrapper.status();
});