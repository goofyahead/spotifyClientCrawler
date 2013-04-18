#!/usr/bin/env node
var fs = require ('fs');
var myScraper = require('./SpotifyIdScrapper');
var parser = require('../../../crawler_engine/lib/boot');
var engine = require('../../../crawler_engine/lib/engines');
var engines = require('../../../crawler_engine');
var jquery = fs.readFileSync('./jquery-1.9.1.min.js').toString();
var es = require('../../../crawler_engine/lib/es');

parser.options.proxy = new engines.common.FileStringProvider({ 
    file: __dirname + '/../../../crawler_engine/proxy-lists/proxies.json',
    random: true
});

parser.options.jquery = jquery;

parser.options.quitOnError = false;

parser.options.noScraperLogging = true;

parser.options.input = new engine.inputs.RangeInput({
    start: 1,
    stop: 999000000,
    step: 1
});

parser.run({
    scraper: myScraper.SpotifyIdCrawler,
    useOutputIds: true
});