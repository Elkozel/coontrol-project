var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var net = require('net');
var client = new net.Socket();
var servUp = false;

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
  socket.on('start', function(msg){
    if(!servUp){
      client.connect({port:0000, host:'hello.com'},function(){
        io.emit('msg', "Connected!");
        console.log("Connected!");
        servUp = true;
      });
    }
    else{
      client.write(msg);
    }
  });
  socket.on('dir', function(msg){
    if(servUp){
      client.write(msg);
    }
  });
  socket.on('ping', function(msg){
    console.log(msg);
  });
  socket.on('disconnect', function(){
    client.write("CED");
    client.destroy();
    servUp = false;
  });
});

client.on('data', function(data){
  data = String(data);
  io.emit('message', data);
});
client.on('disconnect', function(){
  io.emit('message', "CED");
  servUp = false;
})

http.listen(80, function(){
  console.log('listening on *:80');
});
