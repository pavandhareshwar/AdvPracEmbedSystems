CONFIG_MODULE_SIG=n

obj-m := process_tree_lineage.o

all:
	make -C /home/pavan/APES/linux-4.14.15 M=`pwd` modules
	#make -C /home/pavan/APES/linux-`uname -r` M=`pwd` modules

clean:
	make -C /home/pavan/APES/linux-4.14.15 M=`pwd` clean
	#make -C /home/pavan/APES/linux-`uname -r` M=`pwd` clean
