#!/bin/bash

case $1 in
    Example)
        ./FastBB -f "./dataset/Example.g" -k 1 -d 5 -l 3 -r 3 -v "spenode" -s 2;;
    divorce)
        v;;
    writer)
        ./FastBB -f "./dataset/writer.g" -k 1 -d 200 -l 3 -r 3 -v "node";;
    cfat)
        ./FastBB -f "./dataset/cfat.g" -k 1 -d 20 -l 6 -r 6 -v "node";;
    cities)
        ./FastBB -f "./dataset/cities.g" -k 1 -d 50 -l 5 -r 5 -v "node";;
    IMDB)
        ./FastBB -f "./dataset/IMDB.g" -k 1 -d 200 -l 5 -r 5 -v "node";;
    youtube)
        ./FastBB -f "./dataset/youtube.g" -k 1 -d 200 -l 6 -r 6;;
    actorMovie)
        ./FastBB -f "./dataset/actorMovie.g" -k 1 -d 200 -l 3 -r 3 -v "node";;
    DBLP)
        ./FastBB -f "./dataset/DBLP.g" -k 1 -d 200 -l 3 -r 3 -v "node";;
    flickr)
        ./FastBB -f "./dataset/flickr2.g" -k 1 -d 300 -l 3 -r 3;;
    patent)
        ./FastBB -f "./dataset/patent2.g" -k 1 -d 200 -l 3 -r 3 -v "node";;
esac
