#Makefile

rm=/bin/rm -f
CC= cc
DEFS=  
PROGNAME= rsi_wc_app
LINKS= -lpthread -L../../../build/ -lrps


RSI_DRIVER_PATH = ../../../include
RSI_CERTIFICATES_PATH = ../../utilities/certificates

CFLAGS+=  -I$(RSI_DRIVER_PATH)/ -I$(RSI_CERTIFICATES_PATH)
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D RSI_BT_ENABLE

WLAN_BT_APP = rsi_bt_app.o \
							rsi_wlan_app.o \
							main.o


all: $(WLAN_BT_APP) 

	make wlan_bt_classic CC=$(CC) -C ../../../build/ 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(WLAN_BT_APP) core *.o *~ rsi_wc_app
