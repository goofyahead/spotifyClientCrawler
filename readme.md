//CREATE IDS INDEX
export ES=http://v4:doyjod134u@elasticsearch-a-1184289202.us-east-1.elb.amazonaws.com:2013

//LOAD PRE
node lib/bulkfile2es.js -d ./../../ark/spotifyApp/idCrawler/initialData/ -e $ES/sp_pre_ids/ids/ -t ids -c ../../indexes/_ids.js --checkfile ./../../ark/spotifyApp/idCrawler/initialData/checkfile.json

//CHECK FB LINKS

//LAUNCH MUSIC CRAWLER
