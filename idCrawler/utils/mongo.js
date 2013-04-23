// Retrieve
var Db = require('mongodb').Db;
var Server = require('mongodb').Server;
var BSON = require('mongodb').BSONPure;
var MongoClient = require('mongodb').MongoClient;
var crypto = require('crypto');

var db = new Db('ark', new Server("127.0.0.1", 27017,
   {auto_reconnect: false, poolSize: 4}), {w:0, native_parser: false});

// Establish connection to db
db.open(function(err, db) {
  if(err) { 
    return console.dir(err);
} else {
    //opens the database and the menus collection
    db.createCollection('servers', {safe:true}, function(err, collection) {
        if (err) {
            console.log("The 'servers' collection doesn't exist. Creating it with sample data...");
        } else {
            console.log("Collection 'servers' exists.");
        }
    });
}
});

exports.addServer = function (serverObject) {
    db.collection('servers', function(err, collection) {
        collection.insert(serverObject, {safe:true}, function(err, result) {
            if (err) {
                return({'error':'An error has occurred'});
            } else {
                return(result[0]);
            }
        });
    });
}


exports.findByServerName = function(serverName, success, error) {
    
    db.collection('servers', function(err, collection) {
        if (err){
            return -1;
        } else {
            collection.findOne({'server': serverName}, function(err, item) {
                if(err) {
                    return -1;
                } else {
                    if (item) {
                        success();
                    } else {
                        error();
                    }                    
                }
            });
        }
    });
}

exports.incrementServerCount = function(serverName) {

    db.collection('servers', function(err, collection) {
        collection.update({'server': serverName}, {$inc: {'count' : 1}}, {safe:true}, function(err, result) {
            if (err) {
                res.send({'error':'An error has occurred'});
            } else {
                return(result);
            }
        });
    });
}