const net = require('net');
const buf = Buffer.from([0xaa, 0xc3, 0x02, 0x00, 0x01, 0x00, 0xcc]);


function myParser(buf)
{
  buf = Buffer(buf);
  if(! buf instanceof Buffer) {
    return null;
  }
  if(buf.length !== 7) {
    return null;
  }
  json = {
    stx: buf.readUInt8(0),
    type: buf.readUInt8(1),
    len: buf.readUInt16LE(2),
    mute: buf.readUInt8(4),
    duration: buf.readUInt8(5),
    etc: buf.readUInt8(6),
  };
  return json;
};
console.log(buf);
console.log(buf.toJSON())
console.log(myParser(buf));

const client = net.createConnection({port: 8090, host:'127.0.0.1'}, function() {
    console.log('connected to server!');
    client.write(buf);
    client.end();
});

client.on('error', function(e) {
    console.log(e);
});