const net = require('net');
const client = net.createConnection({ port: 8090, host: '127.0.0.1' }, function () { 
    console.log("connected to server!");
});

client.on('data', function(data) {
    console.log(data);
    const dataArray = [];
    for (let i = 0; i < data.length; i+=4) {
        dataArray.push(data.readInt32LE(i));
    }
    console.log(dataArray);
 });
 
 client.on('end', function() { 
    console.log('disconnected from server');
 });