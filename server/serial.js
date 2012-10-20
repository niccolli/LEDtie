var serialport = require('serialport');
var SerialPort = serialport.SerialPort;
var sp = new SerialPort('/dev/tty.usbserial-AE01CQBA', {
	parser: serialport.parsers.readline('\r\n')
});

count = 0;

setInterval(function() {
	var data = [68, parseInt(Math.random()*255)];
	sp.write(data);
//	count++;
//	if(count > 255){ count = 0; }
}, 50);
