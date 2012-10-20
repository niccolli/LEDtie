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

var serialport = require('serialport');
var SerialPort = serialport.SerialPort;
var sp = new SerialPort('/dev/tty.usbserial-AE01CQBA', {
	parser: serialport.parsers.readline('\r\n')
});

io.sockets.on('connection', function(socket){
	socket.on('value', function(data) {
		var xbeewrite = [data.state.charCodeAt(0), Math.pow(2,data.value)-1];
		sp.write(xbeewrite);
	});
});

