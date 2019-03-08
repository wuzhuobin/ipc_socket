const net = require('net');
const client = net.connect({ port: 8080, host: '127.0.0.1' }, function () { 
    console.log("connected to server!");
});

client.on('data', function(data) {
    console.log(data.toString());
 });
 
 client.on('end', function() { 
    console.log('disconnected from server');
 });