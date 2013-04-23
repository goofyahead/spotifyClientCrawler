var laeh2 = require('laeh2');
var _x = laeh2._x;
var _e = laeh2._e;

var _ = require('underscore');
var colors = require('colors');

exports.SpotifyMusicListCrawler = function SpotifyMusicListCrawler() {

    this.process = function( item, data, api, callback) {
        api.logger.info(item);
        var that = this;
        var data = {};

        console.log(item.toString().green + ' item processed'.green);
        console.log(__dirname);

        var childProcess = require('child_process'),spot;

        spot = childProcess.exec(__dirname + '/../spot ' + item , 
            { encoding: 'utf8',
              timeout: 0,
              maxBuffer: 1000*1024,
              killSignal: 'SIGTERM',
              cwd: null,
              env: null },
              function (error, stdout, stderr) {
                if (error) {
                 console.log(error.stack);
                 console.log('Error code: ' + error.code);
                 console.log('Signal received: ' + error.signal);
                }
                console.log('Child Process STDOUT: ' + stdout);
                console.log('Child Process STDERR: ' + stderr);
        });

        spot.on('exit', function (code) {
           console.log('Child process exited with exit code '+ code);
        });
    };
};