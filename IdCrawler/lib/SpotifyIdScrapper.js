var laeh2 = require('laeh2');
var _x = laeh2._x;
var _e = laeh2._e;

var _ = require('underscore');
var jsdom = require('jsdom');
var colors = require('colors');

exports.SpotifyIdCrawler = function SpotifyIdCrawler() {

    this.process = function( item, data, api, callback) {
        api.logger.info(item);
        var that = this;
        var data = {};

        var url = 'http://open.spotify.com/user/' + item;
        //console.log(url.toString().green);

        api.loadUrl(url, {}, _x(callback, true, function(error, body, res) {

            jsdom.env({
                html: body,
                src: [api.engine.options.jquery],

                done: function (errors, window) {
                    var $ = window.jQuery;

                    if ($('meta[property="fb:profile_id"]').length > 0) {
                        var fb_id = $('meta[property="fb:profile_id"]').attr('content');
                        console.log(fb_id.toString().yellow + ' found'.yellow
                            + ' to spotify id: '.yellow + item.toString().yellow);

                        api.engine.options.outputIds.store(item.toString(), {'fb_id' : fb_id}, _x(callback, true, function(err) {
                            callback(null);
                        }));
                    }
                    window.close();
                }
            });
        }));
    };
};