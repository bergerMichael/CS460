#!/bin/bash
#downloads all files from provided web directory address

echo Enter web directory address:

read webaddr

wget -r -np -nH --cut-dirs=3 -R index.html $webaddr
