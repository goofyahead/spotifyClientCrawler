//CREATE IDS INDEX

//LOAD PRE
pushd ../elasticsearch/scripts && \
  node lib/bulkfile2es.js -d $DATA_DIR -e $ES/${CRAWLER_PREFIX}_ids/ids/ \
    -b $BULK -t ids -c ../../indexes/_ids.js --checkfile $checkfile && \

//CHECK FB LINKS

//LAUNCH MUSIC CRAWLER