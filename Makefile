# Makefile for multiboot

CFLAGS       :=	-O2 -Wall
LDFLAGS      :=	-s


BIN_TARGETS  :=	evkey/evkey

SBIN_TARGETS :=	scripts/multiboot 		\
		scripts/multiboot_read_item 	\
		scripts/multiboot_flash_kernel	\
		scripts/powerconfig 		\
		scripts/preinit_backup 		\
		scripts/preinit_real

ETC_TARGETS :=	config/modules.boot.example	\
		config/power.conf.example

CONF_TARGETS := README 				\
		$(wildcard config/multiboot.d/*)



all: build

build: evkey/evkey

evkey/evkey: evkey/evkey.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

install:
	install -o root -g root -m 0755 -d $(DESTDIR)/etc
	install -o root -g root -m 0755 -d $(DESTDIR)/etc/multiboot.d
	install -o root -g root -m 0755 -d $(DESTDIR)/etc/multiboot.d/examples
	install -o root -g root -m 0755 -d $(DESTDIR)/bin
	install -o root -g root -m 0755 -d $(DESTDIR)/sbin
	install -o root -g root -m 0755 -d $(DESTDIR)/boot
	install -o root -g root -m 0755 -d $(DESTDIR)/boot/multiboot
	install -o root -g root -m 0755 $(BIN_TARGETS) $(DESTDIR)/bin/
	install -o root -g root -m 0755 $(SBIN_TARGETS) $(DESTDIR)/sbin/
	install -o root -g root -m 0644 $(ETC_TARGETS) $(DESTDIR)/etc/
	install -o root -g root -m 0644 $(CONF_TARGETS) $(DESTDIR)/etc/multiboot.d/examples/

clean:
	rm -f evkey/evkey

