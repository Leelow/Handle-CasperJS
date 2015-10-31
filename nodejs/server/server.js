var http = require('http').Server();
//var io = require('socket.io')(http);
//var daemonIO = require("./lib/daemon.io.js")(io);
var wrapper = require("./lib/wrapper.js")();


wrapper.status();


var port = 8080;

http.listen(port, function(){
    console.log('Server started on *:' + port);

});