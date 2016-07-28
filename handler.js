/**
 * Device Commands Handler for rccar
 */

'use strict';

var serialport = require('serialport');


/**
 * Constructor method
 * @param {Object} config         Configuration which read from config.json (Read only)
 * @param {Function} logger       Logger compatible with console object - Usage: logger.log(text)
 * @param {HandlerHelper} helper  Instance of HandlerHelper
 */
var CommandsHandler = function (config, logger, helper) {

	// Device configuration
	this.config = config;
	this.config.arduinoSerialPort = config.arduinoSerialPort || '/dev/ttyACM0';

	// Logger
	this.logger = logger;

	// Current sensor value
	this.sonicSensorValue = null;

	// Connection for serial port
	this.serialPort = null;
	this._connectToSerialPort();

};


/**
* Set the motor power
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.setMotorPower = function (args, cb_runner) {

	this._sendToSerialPort('setMotorPower', args.left, args.right);

	cb_runner.send(null, 'OKAY');

};


/**
* Set the brightness of head light
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.setHeadLight = function (args, cb_runner) {

	this._sendToSerialPort('setHeadLight', args.brightness);

	cb_runner.send(null, 'OKAY');

};


/**
* Set the lighting of blinker light
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.setBlinker = function (args, cb_runner) {

	this._sendToSerialPort('setBlinker', args.left, args.right);

	cb_runner.send(null, 'OKAY');

};


/**
* Set the lighting of rear light
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.setRearLight = function (args, cb_runner) {

	this._sendToSerialPort('setRearLight', args.red, args.green, args.blue);

	cb_runner.send(null, 'OKAY');

};


/**
* Set the string of display
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.setLCD = function (args, cb_runner) {

	this._sendToSerialPort('setLCD', args.str);

	cb_runner.send(null, 'OKAY');

};


/**
* Get the sonic sensor value
* @param  {Object} args         Arguments of the received command
* @param  {Function} cb_runner  Callback runner for response
* @return {Boolean} if returns true, handler indicates won't use the callback
*/
CommandsHandler.prototype.getSonic = function (args, cb_runner) {

	var self = this;

	self.sonicSensorValue = null;

	var wait_timer = setInterval(function () {

		if (self.sonicSensorValue == null) return;

		clearInterval(wait_timer);
		cb_runner.send(null, self.sonicSensorValue);

	}, 10);

	self._sendToSerialPort('getSonic');

};


// ----


/**
 * Open the serial port connection
 */
CommandsHandler.prototype._connectToSerialPort = function () {

	var self = this;

	console.log(self.config);

	self.serialPort = null;

	self.serialPort = new serialport('/dev/ttyACM0', {
		baudRate: 9600,
		dataBits: 8,
		parity: 'none',
		stopBits: 1,
		flowControl: false,
		parser: serialport.parsers.readline('\n'),
		autoOpen: false
	});

	self.serialPort.on('data', function (data) {

		if (data.match(/getSonic:(\d+);/)) {
			self.sonicSensorValue = RegExp.$1;
			self.logger.log('Received sensor value - getSonic = ' + self.sonicSensorValue);
		}

	});

	// Connect
	self.serialPort.open(function (error) {

		if (!error) return self.logger.log('Connected to serial port');

		// Reconnect
		self.logger.log('Could not connect to serial port; Reconnecting...');
		setTimeout(self._connectToSerialPort, 3000);

	});

};


/**
 * Send the command to Arduino via the serial port connection
 * @param  {Arguments} Variable-length argument. first item is command name. later item is value.
 */
CommandsHandler.prototype._sendToSerialPort = function () {

	var self = this;

	if (arguments.length <= 0) return;

	var cmd_str = new String();
	for (var i = 0; i < arguments.length; i++) {
		if (0 < cmd_str.length) {
			cmd_str += ':';
		}
		cmd_str += arguments[i] + '';
	}
	cmd_str += ';\n';

	self.serialPort.write(cmd_str, function(err, results) {

		if (err) {
			self.logger.log('_sendToSerialPort - Could not send a command - ' + cmd_str);
		} else {
			self.logger.log('_sendToSerialPort - Sent a command - ' + cmd_str);
		}

	});

}


// ----

module.exports = CommandsHandler;
