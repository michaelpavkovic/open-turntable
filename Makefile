platform=LPC1768
project_name=open-turntable
microcontroller_mount=/Volumes/MBED

compile:
	mbed compile -t GCC_ARM -m $(platform)

deploy:
	cp BUILD/$(platform)/GCC_ARM/$(project_name).bin $(microcontroller_mount)

clean:
	rm -rf BUILD
