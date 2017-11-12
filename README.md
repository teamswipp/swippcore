<p align="center">
	<img src="http://www.swippcoin.com/images/logo-s.png" />
</p>

<p align="center">
	<h1>Swipp - a modern cryptocurrency</h1>
	SWIPP is a modern cryptocurrency based on a secure hashing algorithm - offering blazingly fast anonymous transactions. 
	The code base is actively developed by a skilled software engineering team with decades of experience.
</p>
<p>
	The currency is a long-term project with a long-term development plan. The currency features sane returns and 
	specifications intended to create a healthy cryptocurrency with longevity.
</p>

# Building the daemon
First, install the needed dependencies;
```
sudo apt-get install build-essential libssl-dev libboost-all-dev git
sudo apt-get libdb5.1++-dev libminiupnpc-dev
```
Then we also need to set executable permissions on the build_detect_platform script;
```
cd swipp/src/leveldb
chmod +x build_detect_platform
```
Then it's time to build. The example below uses the make file for linux/unix - but there are also make files available for other operating systems in the same directory.
```
cd ..
make -f makefile.unix
```
