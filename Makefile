compile:
	gcc -o reader.o reader.c -lnfc
run: compile
	./reader.o
clean:
	rm ./reader.o
installconfig:
	sudo mkdir -p /etc/nfc/devices.d
	sudo cp ./libnfc_ex_conf/libnfc.conf.sample /etc/nfc/libnfc.conf
	sudo cp ./libnfc_ex_conf/pn532_via_uart2usb.conf.sample /etc/nfc/devices.d/pn532_via_uart2usb.conf
	echo "*Available usbtty devices: `ls /dev/ttyUSB*`"
	echo "*Edit /etc/nfc/devices.d/pn532_via_uart2usb.conf if your device is other than ttyUSB0"
