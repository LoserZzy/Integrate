

objects=universal.o utils.o sysutils.o hwutils.o drutils.o modutils.o collect.o FWCConnect.o get_additional_hwinfo.o

hardware_driver:$(objects)
	gcc -o hardware_driver $(objects)
	rm $(objects)
	#./hardware_driver

collect.o:collect.c HDMstruct.h FWCConnect.h SysInfo.h get_additional_hwinfo.h settings.h
	gcc -c collect.c
utils.o:utils.c SysInfo.h settings.h
	gcc -c utils.c
sysutils.o:sysutils.c SysInfo.h settings.h
	gcc -c sysutils.c
FWCConnect.o:FWCConnect.c FWCConnect.h
	gcc -c FWCConnect.c
universal.o:universal.c HDMstruct.h
	gcc -c universal.c 
hwutils.o:hwutils.c HDMstruct.h
	gcc -c hwutils.c 
drutils.o:drutils.c HDMstruct.h
	gcc -c drutils.c 
modutils.o:modutils.c HDMstruct.h
	gcc -c modutils.c 
get_additional_hwinfo.o:get_additional_hwinfo.c get_additional_hwinfo.h settings.h
	gcc -c get_additional_hwinfo.c

.PHONY:clean
clean:
	rm hardware_driver
