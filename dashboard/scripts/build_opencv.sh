#!/bin/bash

# check if we have the correct number of parameters
if [ $# -ne 1 ]; then
	if git rev-parse --git-dir > /dev/null 2>&1; then :
		OPENCV_ROOT=$(git rev-parse --show-toplevel)
	else :
		echo "Error: you must execute this script in the stereomatch demo repository repository or pass the path"
		exit -1
	fi
else :
	OPENCV_ROOT=$1
fi

# check if the directory exists
if [ ! -d "$OPENCV_ROOT" ]; then
	echo "$0: Unable to build OpenCV in a non-existing directory"
	exit -2
fi

OPENCV_HOME=$OPENCV_ROOT/opencv
OPENCV_BUILD_PATH=$OPENCV_HOME/build
OPENCV_INSTALL_PATH=$OPENCV_HOME/install

# check if we need to download it
if [ ! -d "$OPENCV_HOME" ]; then

	# get the source code
	git clone --branch 3.2.0 https://github.com/opencv/opencv.git $OPENCV_HOME
fi

# check if we need to compile it
if [ ! -f "$OPENCV_INSTALL_PATH/FindOpenCV.cmake" ]; then
	mkdir -p $OPENCV_BUILD_PATH
	CURRENT_CWD=$PWD
	bash -c "cd $OPENCV_BUILD_PATH && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_PATH $OPENCV_HOME" || exit 1
	make -C $OPENCV_BUILD_PATH install
	ln -s $OPENCV_INSTALL_PATH/share/OpenCV/OpenCVConfig.cmake $OPENCV_INSTALL_PATH/share/OpenCV/FindOpenCV.cmake
fi
