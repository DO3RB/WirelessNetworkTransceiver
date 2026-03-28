ifeq ($(PROJECT),)
$(error Missing PROJECT path)
endif

MAKEFLAGS := -O -r --jobs=$(shell nproc)#-R
SHELL := $(shell which bash)
#.SHELLFLAGS := -eu -o pipefail -c

MCU := samd21g18a
LDFILE := $(MCU)/sam.ld

# make -p print implicits
CHAIN := arm-none-eabi-
# variables custom
NMS := $(CHAIN)nm
OCP := $(CHAIN)objcopy
DMP := $(CHAIN)objdump
SZE := $(CHAIN)size
# variables implicit
AR  := $(CHAIN)ar
AS  := $(CHAIN)as
CC  := $(CHAIN)gcc
CPP := $(CHAIN)gcc -E
CXX := $(CHAIN)g++
ARFLAGS :=
ASFLAGS :=
TARGET_ARCH := -mcpu=cortex-m0plus -mthumb#-mfloat-abi=soft
CFLAGS := -Wall -Wextra -O2 -flto -ffunction-sections -fdata-sections -fwrapv
CPPFLAGS = -I $(firstword $(subst /, , $<)) -I. -MMD -MP -I tinyusb -DCFG_TUSB_CONFIG_FILE=\"$(PROJECT)/tinyusb.h\"
CXXFLAGS := -fno-use-cxa-atexit -fno-exceptions -fno-unwind-tables -fno-rtti
LDFLAGS := -nostdlib -T $(LDFILE) -Wl,--gc-sections
LDLIBS := -Wl,--start-group -lm -lc_nano -lgcc -Wl,--end-group#-lg_nano -lstdc++_nano -lsupc++_nano

BUILD := $(PROJECT)/.build

.SILENT: clean diff version tinyusb analyzer stack flash remote size dbg lst nms global
.PHONY:  clean diff version tinyusb analyzer stack flash remote size dbg lst nms global usb
NOBLD := clean diff version tinyusb usb

ifeq ($(filter $(NOBLD),$(MAKECMDGOALS)),)
include  $(BUILD)/main.mk
ROOTS := $(PROJECT)/main samd21g18a/syscalls samd21g18a/newlib
endif

all: version size $(BUILD)/main.uf2

$(BUILD)/main.mk: $(BUILD)
	@if [ ! -f '$@' ]; then set -- $(addprefix $(BUILD)/, $(ROOTS)); \
	printf "OBJS := $$1.o\ninclude $$1.d\n"  > $@; shift; for f in "$$@"; do \
	printf "OBJS += $$f.o\ninclude $$f.d\n" >> $@;  done; fi

$(BUILD)/main.uf2: $(BUILD)/main.hex
	@loader/uf2conv.py -f 0x68ed2b88 -c -o $@ $<

$(BUILD)/main.hex: $(BUILD)/main.elf
	@$(OCP) -O ihex $< $@

$(BUILD)/main.debug.elf: LDFLAGS += -g3
$(BUILD)/main.debug.elf $(BUILD)/main.elf: $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) $(OUTPUT_OPTION)

$(BUILD)/%.o $(BUILD)/%.d: %.c Makefile
	@mkdir -p $(@D)
	$(COMPILE.c) $< -o $(BUILD)/$*.o
	$(call gather_objs,$(BUILD)/$*)

$(BUILD):
	@if [ -h "$(BUILD)" -a ! -e "$(BUILD)" ]; then rm $(BUILD); fi
	@ln -s $(shell mktemp -d) -T $(BUILD)

define gather_objs
@flock -w 5 $(BUILD)/main.mk awk 'BEGIN { RS="[^\\\\]\\n"; FS=":" } \
	ARGIND==1 { n=$$0; if (gsub("^include $(BUILD)/|\\.$$","",n)>1) { have[n]; } } \
	ARGIND==2 && FNR>1 { n=$$1; if (sub(/\.h$$/,"",n) && (getline _ <(n".c"))>0) { need[n]; close(n".c"); } } \
	END { for (name in need) if (!(name in have)) { \
	print "OBJS += $(BUILD)/"name".o"; \
	print "include $(BUILD)/"name".d"; \
	}}' $(BUILD)/main.mk "$(1).d" >> $(BUILD)/main.mk
endef

version:
	$(eval GIT := $(shell git describe --always --long --dirty --abbrev=40))
	$(eval STR := 'const char VERSION[] = "$(PROJECT:code_%=%)-$(GIT)\r\n";')
	$(eval NOW := '$(file < $(MCU)/version.c)')
	if [ "$(STR)" != "$(NOW)" ]; then echo $(STR) > $(MCU)/version.c ; fi

clean:
	rm -rf $(shell readlink -f \"$(BUILD)\") "$(BUILD)"

flash: $(BUILD)/main.uf2
	echo -n 'Flashing... '
	if [ ! -e /dev/disk/by-label/Arduino ] && [ -e /dev/ttyACM0 ]; then printf 'load\r' > /dev/ttyACM0; fi
	timeout 10 bash -c 'while [ ! -e /dev/disk/by-label/Arduino ]; do sleep 0.1; done' || true
	udisksctl mount -b /dev/disk/by-label/Arduino > /dev/null && \
	cp $< /run/media/$(shell whoami)/Arduino/ && echo done || true
remote: $(BUILD)/main.uf2
	echo -n 'Flashing remotely... '
	scp -q $< 'pi@raspberrypi.local:/run/user/1000/'
	ssh -n pi@raspberrypi.local 'cd /run/user/1000/; \
		if [ -a /dev/ttyACM0 ]; then printf "load\r" > /dev/ttyACM0; sleep 2; fi; \
		sudo udisksctl mount -b /dev/disk/by-label/Arduino > /dev/null && \
		sudo cp main.uf2 /media/root/Arduino/'
	echo done
global: flash remote

size: $(BUILD)/main.elf
	echo -e "\033[1m$(SZE)\033[0m"
	$(SZE) -A "$<" | awk '/^section*/{n=1} { if(n>1) $$3=sprintf("0x%08X",$$3) } \
	{ if(n) {printf("%-10s %5s %s\n",$$1,$$2,$$3); n++ }} /^.stack*/{n=0}'

dbg: $(BUILD)/main.debug.elf
	$(DMP) -SCw --visualize-jumps=color --disassembler-color=extended --no-show-raw-insn $< | less -RS > $(MAKE_TERMOUT)
lst: $(BUILD)/main.elf
	$(DMP) -dCwz --visualize-jumps=color --disassembler-color=extended --no-show-raw-insn $< | less -RS > $(MAKE_TERMOUT)
nms: $(BUILD)/main.elf
	$(NMS) -nC $< | less > $(MAKE_TERMOUT)

diff:
	git difftool -d

usb:
	$(eval BUS := $(shell lsusb -d 0403:7ae7 | awk 'patsplit($$0, ID, /[[:digit:]]+/) {printf "%d", ID[1]}'))
	$(eval DEV := $(shell lsusb -d 0403:7ae7 | awk 'patsplit($$0, ID, /[[:digit:]]+/) {printf "%d", ID[2]}'))
	sudo modprobe usbmon
	sudo tshark -i usbmon0 -Y 'usb.addr matches "$(BUS).$(DEV).{1}"' \
	-T fields -e usb.addr -e usb.endpoint_address.direction -e usb.urb_type -e usb.data_len -e usb.capdata

tinyusb:
	$(eval URL := https://github.com/hathach/tinyusb.git)
	$(eval TAG := $(shell git ls-remote --tags --sort='v:refname' $(URL) | tail -n1 | cut -d'/' --fields=3))
	$(eval TNY := $(shell mktemp -d))
	git clone -q --depth 1 --branch $(TAG) --single-branch -c advice.detachedHead=false $(URL) $(TNY)
	git -C $(TNY) --no-pager log -n1 --pretty=format:"%C(auto)Commit: %H %D%nAuthor: %an <%ae>%nDate:   %ad%n"
	rm -rf ./tinyusb/*
	cp     $(TNY)/src/tusb.{h,c} $(TNY)/src/tusb_option.h ./tinyusb/
	cp -r  $(TNY)/src/{class,common,device,osal} ./tinyusb/
	cp     $(TNY)/src/portable/microchip/samd/dcd_samd.c ./tinyusb/device/dcd.c
	rm -rf $(TNY)
	mv ./tinyusb/device/usbd_control.c ./tinyusb/device/usbd_pvt.c
	awk -i inplace '/[\s]*#include/{gsub(/sam.h/,"$(MCU)/sam.h")}{print}' ./tinyusb/device/dcd.c

cppcheck:
	cppcheck --check-level=exhaustive --force -Itinyusb .

cppcheck-excessive:
	echo | arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -dM -E -c - | sort > $(BUILD)/cppcheck.h
	cppcheck --enable=all --check-level=exhaustive --force -I/usr/arm-none-eabi/include/ -I/usr/lib/gcc/arm-none-eabi/14.1.0/include/ --include=$(BUILD)/cppcheck.h --platform=unix32 --quiet -I. -I tinyusb -DCFG_TUSB_CONFIG_FILE=\"$(PROJECT)/tinyusb.h\" .

analyzer: CFLAGS += -g3 -fanalyzer -fdump-analyzer-supergraph#-fdump-analyzer-callgraph
analyzer: $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) -o $(BUILD)/main.analyzer.elf
#	dot -T svg -Gsize=1920,1200 -o $(BUILD)/graphcall.svg  $(BUILD)/main.analyzer.elf.wpa.callgraph.dot
	dot -T svg -Gsize=1920,1200 -o $(BUILD)/graphsuper.svg $(BUILD)/main.analyzer.elf.wpa.supergraph.dot
	inkscape $(BUILD)/graphsuper.svg -o $(BUILD)/graphsuper.png -w 19200
#	inkscape $(BUILD)/graphsuper.svg > /dev/null 2>&1 &
	xdg-open $(BUILD)/graphsuper.png

stack: LDFLAGS += -fstack-usage
stack: $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) -o $(BUILD)/main.elf
	cat $(BUILD)/main.elf.ltrans0.ltrans.su | sort > $(BUILD)/main.su
	rm $(BUILD)/main.elf.ltrans0.ltrans.su
	less -RS $(BUILD)/main.su > $(MAKE_TERMOUT)

callgraph:  CFLAGS := -fcallgraph-info=su,da $(filter-out -flto,$(CFLAGS))
callgraph: LDFLAGS += -fcallgraph-info=su,da
callgraph: $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) -o /dev/null

ctrlflow:  CFLAGS := -fdump-tree-all-graph -fdump-rtl-outof_cfglayout $(filter-out -flto,$(CFLAGS))
ctrlflow: LDFLAGS += -fdump-tree-all-graph -fdump-rtl-outof_cfglayout
ctrlflow: $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) -o /dev/null
