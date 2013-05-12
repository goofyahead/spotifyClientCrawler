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

        var url = 'http://immense-stream-2556.herokuapp.com/spotify/'
        + item +'?key=arkguinscanfly2';

        api.loadUrl(url, {}, _x(callback, true, function(error, body, res) {
          api.storeData(item, body, _x(callback, true, callback));
        }));
    };
};