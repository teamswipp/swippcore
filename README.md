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

# Build preparation
The first step is to clone the Swipp repository into a local directory on you computer. In order to be able to do this, we also need to install git:
```
sudo apt-get install git
git clone https://github.com/teamswipp/swippcore
```

Make sure you answer "Yes", to all the questions asked by apt-get.

When this is done, we need to install a few build tools in order for the build to succeed, such as GNU make and GCC:
```
sudo apt-get install build-essential make g++
```

After we have prepared the needed build environment, we also need to install a number of library dependencies used by the Swipp wallet:
```
sudo apt-get install libboost-all-dev libssl1.0-dev libdb5.3++-dev libminiupnpc-dev libz-dev
```

# Building the CLI/RPC version
If the build environment is properly prepared, we should be able to build the wallet by executing the following command:
```
cd swippcore/src
make -j8 -f makefile.unix
```

With a little luck, the build succeeds, leaving you with an executable file "swippd" in the current directory.

# Building the QT version
The QT wallet needs a number of additional dependencies. Execute the following command to complement the build environment with
the default libraries and build tools from QT:
```
sudo apt-get install qt5-default qttools5-dev-tools
```

Once completed, we should be able to generate the make file from the Swipp project file by running the following command:
```
cd swippcore
qmake swipp.pro
```

Now simply build the project bu executing the following command:
```
make -j8
```

If successful, this should generate an executable file "swipp-qt" in the current directory.

# IMPORTANT!
The master branch is  our main development branch. When you run or update to a new Swipp version, we recommend you that you only
use commits where a new version has been tagged. We tag new versions at regular intervals - even when we do not make an official
release. Any tagged release can be considered stable and safe to run. <b>Running any other version at any other commit in-between
is not supported and done at your own risk.</b>
