# simple_nfc_reader
Using libnfc and PN532 in UART mode
# Installation
1. Install libnfc library from your distro package manager (and hope it has pn532_uart enabled. Also install -dev version that contain headers). <br />
On ubuntu: `sudo apt install libnfc-bin libnfc-dev libnfc-examples`  <br />
Alternatively (the hard way) you can complie it from source and use this configure command:
`./configure --with-drivers=pn532_uart --prefix=/usr --sysconfdir=/etc` <br />
`make`<br />
then `checkinstall` on debian based distros (or `make install`)
2. Clone repo (or download and unzip)
3. Open terminal in cloned folder 
4. Execute `make installconfig`
5. Edit `/etc/nfc/devices.d/pn532_via_uart2usb.conf` to reflect your UART port (`ls /dev/ttyUSB*` with and without reader plugged in)
6. (Optional) Add udev rules for your reader
7. Type `make run` (or `sudo make run` if You didn't any add udev rules)

One more note:
`gcc -o reader.o reader.c -lnfc`
is not the same as
`gcc -o reader.o -lnfc reader.c`
because linker cares about the order!
