#!/usr/bin/env node
var fs = require ('fs');
var myScraper = require('./SpotifyIdScrapper');
var parser = require('../../../crawler_engine/lib/boot');
var engine = require('../../../crawler_engine/lib/engines');
var engines = require('../../../crawler_engine');
var es = require('../../../crawler_engine/lib/es');

parser.options.quitOnError = false;

parser.options.extraOutputs = [
	{	outputName : 'outIds',
		idx : 'sp_ids/ids'
	}
];

parser.run({
    scraper: myScraper.SpotifyIdCrawler
});