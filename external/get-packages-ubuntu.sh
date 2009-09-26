#/bin/bash

# This script needs to be run with sudo

# To install the bakefile package, we need to add it's repo
# (details: http://bakefile.org/wiki/Debian)
if [ "$1" = "bakefile" ] ; then
	curl http://apt.tt-solutions.com/key.asc | apt-key add -
	echo "\n#Needed for bakefile\ndeb http://apt.tt-solutions.com/ubuntu hardy main" >> /etc/apt/sources.list
	apt-get update
fi

apt-get install libgtk2.0-dev build-essential libwxgtk2.8-dev libglib2.0-dev libatk1.0-dev libcurl4-openssl-dev libxml2-dev libxslt1-dev libsqlite3-dev libicu-dev libjpeg62-dev flex bison gperf bakefile 
