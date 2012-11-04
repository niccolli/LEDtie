var express = require('express')
  , app = express()
  , server = require('http').createServer(app)
  , io = require('socket.io').listen(server);

server.listen(3000);

app.configure(function(){
	app.use(express.static('static'));
	app.use(express.logger());
	app.use(express.bodyParser());
});

app.get('/', function(req, res){
	res.sendfile(__dirname + '/index.html');
});

// Use "serialport" and "xbee" to communicate Arduino
var rsp = require('serialport');
var xbee = require('xbee');
var SerialPort = rsp.SerialPort;
var serial_xbee = new SerialPort('/dev/tty.usbserial-AE01CQBA', {
	parser: xbee.packetParser()
});

// Address of remote XBee board.
// 16bit address is assigned DEFAULT
var remote64 = [0x00,0x13,0xa2,0x00,0x40,0x9c,0x0f,0x10];
var remote16 = [0xff,0xfe];

function sendRemoteRFData(cmd, val, remote64, remote16){
	var rf = new xbee.TransmitRFData();
	/*
	 * If you write directory data to XBee
	 * rf.RFData = String.fromCharCode(66, 212);
	 */
	rf.RFData = cmd + String.fromCharCode(val);
	rf.destination64 = remote64;
	rf.destination16 = remote16;
	b = rf.getBytes();
	serial_xbee.write(b);
}

// Socket.IO is connected
io.sockets.on('connection', function(socket){
	// Remote XBee sent the AD value
	serial_xbee.on('data', function(data){
		// If packet is 'Data Sample'
		if( (typeof data.bytes !== "undefined") && (data.bytes[0] == 0x92)){
			socket.emit('voltage', { vel: data.samples.A0 * 5.88 / 1023.0,
									vard: data.samples.A1 * 5.88 / 1023.0,}
			);
		}
	});
	socket.on('value', function(data) {
		// Write command to remote XBee from iPhone
		sendRemoteRFData(data.state, Math.pow(2,data.value)-1, remote64, remote16);
	});
});

