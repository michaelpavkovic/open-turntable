platform=LPC1768#NRF52840_DK # micro:bit
project_name=open-turntable
lpc_mount=/Volumes/MBED
microbit_mount=/Volumes/MICROBIT

mount_location=$(lpc_mount)
extension=.bin

compile:
	mbed compile -t GCC_ARM -m $(platform)

deploy:
	cp BUILD/$(platform)/GCC_ARM/$(project_name)$(extension) $(mount_location)

clean:
	rm -rf BUILD
