var laeh2 = require('laeh2');
var _x = laeh2._x;
var _e = laeh2._e;

var _ = require('underscore');
var colors = require('colors');
var cheerio = require('cheerio');

exports.SpotifyMusicListCrawler = function SpotifyMusicListCrawler() {

    this.process = function( item, data, api, callback) {
      var data = {};

      var urlSpoty = 'http://open.spotify.com/user/' + item;
        console.log(urlSpoty.toString().green);

        api.loadUrl(urlSpoty, {}, function(error, body, res) {

            $ = cheerio.load(body);

            if ($('meta[property="fb:profile_id"]').length > 0) {
                var fb_id = $('meta[property="fb:profile_id"]').attr('content');
                console.log(fb_id.toString().yellow + ' found'.yellow + ' to spotify id: '.yellow + item.toString().yellow);

                data['fb_id'] = fb_id;
                api.logger.info(item);
                var that = this;

                var url = 'http://immense-stream-2556.herokuapp.com/spotify/'
                + item +'?key=arkguinscanfly2';
                //console.log('call for ' + item);
                api.loadUrl(url, {}, function(error, body, res) {
                  
                  data['music'] = JSON.parse(body);
                  console.log(data);
                  api.storeData(item, data, _x(callback, true, callback));
                });
            }
        });      
    };
};