'use strict';

var DenHubDevice = require('denhub-device'),
	CommandsHandler = require(__dirname + '/handler');

// Start the daemon
var daemon = new DenHubDevice(CommandsHandler);
daemon.start();

