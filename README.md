# simple_nfc_reader
Using libnfc and PN532 in UART mode
# Preparations
1. Download relase from https://github.com/nfc-tools/libnfc/releases/
2. Extract and enter dir `cd libnfc...`
4. Install config
`sudo mkdir -p /etc/nfc/devices.d
sudo cp libnfc.conf.sample /etc/nfc/libnfc.conf
cp pn532_via_uart2usb.conf.sample /etc/nfc/devices.d/pn532_via_uart2usb.conf`  
- edit `/etc/nfc/devices.d/pn532_via_uart2usb.conf` to reflect your UART port (`ls /dev/ttyUSB*` with and without reader plugged int)
5. Install library from your distro package manager (and hope it has pn532_uart, also install -devel version that contain headers) or complie it from source:
`./configure --with-drivers=pn532_uart --prefix=/usr --sysconfdir=/etc 
make`
then
`checkinstall` on debian based distros or `make install`
6. Clone repo
7. `make run`


 
