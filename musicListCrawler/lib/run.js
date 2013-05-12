#!/usr/bin/env node
var fs = require ('fs');
var myScraper = require('./SpotifyMusicListCrawler');
var parser = require('../../../crawler_engine/lib/boot');
var engine = require('../../../crawler_engine/lib/engines');
var engines = require('../../../crawler_engine');
var es = require('../../../crawler_engine/lib/es');

parser.options.quitOnError = false;

parser.options.noScraperLogging = true;

parser.run({
    scraper: myScraper.SpotifyMusicListCrawler
});