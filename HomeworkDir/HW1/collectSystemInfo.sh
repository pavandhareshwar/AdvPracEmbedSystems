#!/bin/bash

###############################################################################
# filename: 	collectSystemInfo.sh 
# Description: 	This is a simple script that can be used to collect some 
#				of the important parts of the operating system. The information 
#				collected is written to a file named systemInfo.txt. 

# The items included as part of the output file are:
#	1. User information
#	2. Operating system type
#	3. OS distribution
#	4. OS version
#	5. Kernel version
#	6. Kernel GCC version build
#	7. Kernel build time
#	8. System Architecture information
#	9. Information on file system memory
 
###############################################################################

OUTPUT_FILE=systemInfo.txt

if [ -f $OUTPUT_FILE ]
then
	#echo "$OUTPUT_FILE already exists. Deleting it." 
	rm -rf $OUTPUT_FILE
fi

# User Information
echo "User Information: `uname -a | awk '{print $2}'`" >> $OUTPUT_FILE 

# OS type
echo "OS type: `uname -a | awk '{print $1}'`" >> $OUTPUT_FILE
#echo "OS type: `uname -o`" >> $OUTPUT_FILE

# OS distribution 
echo "OS distribution: `cat /etc/*-release | grep "\<DISTRIB_DESCRIPTION\>" | awk -F'=' '{print $2}' | awk -F'"' '{print $2}'`" >> $OUTPUT_FILE 

# OS version
echo "OS version: `cat /etc/*-release | grep "\<VERSION\>" | awk -F'=' '{print $2}' | awk -F'"' '{print $2}'`">> $OUTPUT_FILE 

# Kernel Version
echo "Kernel version: `uname -a | awk '{print $3}'`" >> $OUTPUT_FILE
#echo "Kernel version: `cat /proc/version`" >> $OUTPUT_FILE

# Kernel GCC version build
echo "Kernel GCC version: `cat /proc/version | awk 'BEGIN {FS="("}{print $3}' | cut -d' ' -f 3`" >> $OUTPUT_FILE

# Kernel build time
#echo "Kernel build time: `cat /proc/version | awk 'BEGIN {FS="#"}{print $2}'`" >> $OUTPUT_FILE
kern_ver_str=`cat /proc/version`
if [ ! -z `cat /proc/version | grep -b -o "SMP"` ]
then
	start_pos=`cat /proc/version | grep -b -o "SMP" | awk -F ":" '{print $1}'`
	offset=$((`echo "SMP " | wc -m` -1))
	start_pos=$((start_pos+offset))
	echo "Kernel build time: ${kern_ver_str:start_pos}" >> $OUTPUT_FILE
elif [! -z `cat /proc/version | grep -b -o "AMP"` ]
then
	start_pos=`cat /proc/version | grep -b -o "AMP" | awk -F ":" '{print $1}'`
	offset=$((`echo "AMP " | wc -m` -1))
	start_pos=$((start_pos+offset))
	echo "Kernel build time: ${kern_ver_str:start_pos}" >> $OUTPUT_FILE
fi

# System architecture information
echo -e "\nSystem Architecture Info: \n" >> $OUTPUT_FILE
echo "`lscpu`" >> $OUTPUT_FILE

# File system memory information
echo -e "\nFile System memory information:\n" >> $OUTPUT_FILE
echo "`df -h`" >> $OUTPUT_FILE
echo -e "\n" >> $OUTPUT_FILE
echo "`cat /proc/meminfo`" >> $OUTPUT_FILE

# Displaying the content of the file
echo "########################################################################"
cat $OUTPUT_FILE
echo "########################################################################"
