#!/bin/sh

url=`git config --get remote.origin.url`

cd /tmp && rm -rf ulapi && git clone $url && cd ulapi && ./autoconf.sh && make dist && tar xzvf *.tar.gz && cd ulapi-* && ./configure && make && make dist || exit 1

exit 0

