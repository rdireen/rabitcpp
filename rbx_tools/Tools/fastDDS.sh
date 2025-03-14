#!/bin/bash
# install and uninstall fast DDS 
# 
# Randy Direen
# 7/15/2020
#
# This is used for developing fast DDS applications and for installing 
# the fastrtpsgen tool for generating C++ messages.
#
# For further details on installation see 
# https://fast-dds.docs.eprosima.com/en/latest/installation/binaries/binaries_linux.html

HMSG="Type ./fastDDS.sh  with no arguments for help"
gcc_dir=$HOME/eProsima_Fast-DDS-2.0.0-Linux

pushd () {
	command pushd "$@" > /dev/null
}

popd () {
	command popd "$@" > /dev/null
}

function find_fastrtpsgen()
{
	if [ -x "$(command -v fastrtpsgen)" ] 
   	then
    	    found_fast=1 
	else
	    found_fast=0
	fi
}

function download_fastrtpsgen()
{
	if [ -d "$gcc_dir" ]
	then
		echo " Fast-DDS already downlaoded."
	else
		echo "Downloading Fast-DDS to: $gcc_dir"
		mkdir -p $gcc_dir
		pushd .
		cd ~    
		wget -O eProsima_Fast-DDS-2.0.0-Linux.tgz https://www.dropbox.com/s/ic1oohhh0ubstt2/eProsima_Fast-DDS-2.0.0-Linux.tgz?dl=0 
		tar -xzf eProsima_Fast-DDS-2.0.0-Linux.tgz -C $gcc_dir
		popd
	fi
}

function install_f()
{

	if [ -d "$gcc_dir" ]
	then
		pushd .
		cd $gcc_dir
		if [[ -f "install.sh" ]]
		then
			chmod +x install.sh
			./install.sh
		else
			echo "install.sh not located see documetation "
		fi
		popd
	else
		echo "Looks like Fast-DDS installer hasn't been downloaded"
		exit 
	fi
	
}

function uninstall_f()
{

	if [ -d "$gcc_dir" ]
	then
		pushd .
		cd $gcc_dir
		if [[ -f "uninstall.sh" ]]
		then
			chmod +x uninstall.sh
			./uninstall.sh
		else
			echo "install.sh not located see documetation "
		fi
		popd
	else
		echo "Looks like Fast-DDS uninstaller isn't available"
		echo "You need to make sure you have dPromia_Fast-DDS downloaded and extracted"
		echo "in your home directory"
		exit 
	fi
	
}

function install_fastrtps()
{
	
	find_fastrtpsgen
	if [ $found_fast -eq 1 ]
	then
		echo "Seems to already be installed, try uninstalling and installing again"
	else
		#install script does the following
		#echo "Installing dependencies"
		#apt-get -y install libasio-dev libtinyxml2-dev
		echo "Downloading fast-DDS"
		download_fastrtpsgen
		echo "Installing fast-DDS"
		install_f
        fi	

}

function uninstall_fastrtps()
{
	
	find_fastrtpsgen
	if [ $found_fast -eq 1 ]
	then
		echo "Uninstalling fastrtpsgen"
		uninstall_f
		echo "Uninstall complete"
	else
		echo "Seems to already be uninstalled"
        fi	
}



user=`whoami`

if [ $user != "root" ]
then
	echo "Permission denied, use sudo ./fastDDS.sh " 
	exit
fi

if [ "$#" -lt 1 ] || [ "$1" = "help" ] 
then
	echo ""
	echo "Install and Uninstall fastDDS (must use sudo on Linux machines)"
	echo ""
	echo "To install"
	echo "    >> sudo ./fastDDS.sh install"
	echo ""
	echo "To uninstall"
	echo "    >> sudo ./fastDDS.sh uninstall"
	echo ""
elif [ "$#" -eq 1 ]
then
	if [ "$1" = "install" ]
        then
		echo "installing"
		install_fastrtps

	elif [ "$1" = "uninstall" ]
	then
		echo "uninstalling"
		uninstall_fastrtps
	else
		echo ""
		echo $HMSG 
		echo ""
	fi
	
else
	echo ""
	echo $HMSG 
	echo ""
fi





