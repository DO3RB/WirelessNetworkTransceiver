ifeq ($(PROJECT),)
$(error Missing PROJECT path)
endif

MAKEFLAGS := --jobs=$(shell nproc)

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
TARGET_ARCH := -mcpu=cortex-m0plus -mthumb#-mfloat-abi=soft -mtune=cortex-m0plus.small-multiply
CFLAGS := -pipe -Wall -Wextra -O2 -flto -ffunction-sections -fdata-sections -fwrapv
#CFLAGS += -ffreestanding -fno-tree-loop-distribute-patterns -fno-builtin -fno-builtin-memcpy -nostdlib
CPPFLAGS = -I $(firstword $(subst /, , $<)) -I. -MMD -MP -I tinyusb -DCFG_TUSB_CONFIG_FILE=\"$(PROJECT)/tinyusb.h\"
CXXFLAGS := -fno-use-cxa-atexit -fno-exceptions -fno-unwind-tables -fno-rtti
LDLIBS := -lm
LDFLAGS := -nostartfiles -specs=nano.specs -T $(LDFILE) -Wl,--gc-sections
#LDLIBS = -L../newlib -lc_nano -lg_nano -lstdc++_nano -lsupc++_nano
#LDFLAGS = -fuse-linker-plugin -Wl,--exclude-libs,libgcc.a -Wl,--print-memory-usage -u _printf_float -Map=main.map,--cref
#arm-none-eabi-gcc -I. -I.. -I ../tinyusb -DCFG_TUSB_CONFIG_FILE=\"tinyusb.h\" -E -MMD -MF- -o /dev/null main.c

BUILD := $(PROJECT)/.build

NAMES = $(addprefix tinyusb/, tusb common/tusb_fifo device/usbd device/usbd_control portable/microchip/samd/dcd_samd)\
	$(basename $(wildcard $(MCU)/*.c)) $(subst $(PROJECT)/main,, $(basename $(wildcard $(PROJECT)/*.c)))

OBJS = $(addprefix $(BUILD)/, $(addsuffix .o, $(NAMES)))

all: version $(BUILD)/main.uf2 size

$(BUILD)/main.uf2: $(BUILD)/main.hex
	loader/uf2conv.py -f 0x68ed2b88 -c -o $@ $<

$(BUILD)/main.hex: $(BUILD)/main.elf
	$(OCP) -O ihex $< $@

$(BUILD)/main.debug.elf: $(PROJECT)/main.c $(OBJS) $(LDFILE)
	$(LINK.c) -g3 $(OBJS) $(LDLIBS) $< $(OUTPUT_OPTION)
	
$(BUILD)/main.elf: $(PROJECT)/main.c $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) $< $(OUTPUT_OPTION)

$(BUILD)/%.o : %.c Makefile | $(BUILD)
	@mkdir -p $(dir $@)
	$(COMPILE.c) $< $(OUTPUT_OPTION)

$(BUILD):
	if [ -h "$(BUILD)" -a ! -e "$(BUILD)" ]; then rm $(BUILD); fi
	ln -s $(shell mktemp -d) -T $(BUILD)

DEPFILES := $(OBJS:%.o=%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

version:
	$(eval GIT := $(shell git describe --always --long --dirty --abbrev=40))
	$(eval STR := 'const char VERSION[] = "$(PROJECT:code_%=%)-$(GIT)\r\n";')
	$(eval NOW := '$(file < $(MCU)/version.c)')
	if [ "$(STR)" != "$(NOW)" ]; then echo $(STR) > $(MCU)/version.c ; fi

clean: $(BUILD)
	rm -rf $(shell readlink -f $<) $<

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
	$(DMP) -SCw --visualize-jumps=color --disassembler-color=extended --no-show-raw-insn $< | less -RS
lst: $(BUILD)/main.elf
	$(DMP) -dCw --visualize-jumps=color --disassembler-color=extended --no-show-raw-insn $< | less -RS
nms: $(BUILD)/main.elf
	$(NMS) -nC $< | less

diff:
	git difftool -d

usb:
	$(eval BUS := $(shell lsusb -d 0403:7ae7 | awk 'patsplit($$0, ID, /[[:digit:]]+/) {printf "%d", ID[1]}'))
	$(eval DEV := $(shell lsusb -d 0403:7ae7 | awk 'patsplit($$0, ID, /[[:digit:]]+/) {printf "%d", ID[2]}'))
	sudo modprobe usbmon
	sudo tshark -i usbmon0 -Y 'usb.addr matches "$(BUS).$(DEV).{1}"' \
	-T fields -e usb.addr -e usb.endpoint_address.direction -e usb.urb_type -e usb.data_len -e usb.capdata

tinyusb-upstream:
	$(eval TNY := $(shell mktemp -d))
	git clone --depth 1 https://github.com/hathach/tinyusb.git $(TNY)
	rm -rf ./tinyusb/*
	mkdir -p ./tinyusb/portable/microchip/
	cp -r $(TNY)/src/portable/microchip/samd ./tinyusb/portable/microchip/
	cp -r $(TNY)/src/class ./tinyusb/class
	cp -r $(TNY)/src/common ./tinyusb/common
	cp -r $(TNY)/src/device ./tinyusb/device
	cp -r $(TNY)/src/osal ./tinyusb/osal
	cp $(TNY)/src/tusb_option.h $(TNY)/src/tusb.h $(TNY)/src/tusb.c ./tinyusb/
	rm -rf $(TNY)
	awk -i inplace '/[\s]*#include/{gsub(/sam.h/,"$(MCU)/sam.h")}{print}' tinyusb/portable/microchip/samd/dcd_samd.c

tinyusb-latesttag:
	$(eval TNY := $(shell mktemp -d))
	$(eval TAG := $(shell git ls-remote --tags --sort='v:refname' https://github.com/hathach/tinyusb.git | tail --lines=1 | cut --delimiter='/' --fields=3))
	git clone --branch $(TAG) --single-branch --depth 1 https://github.com/hathach/tinyusb.git $(TNY)
	rm -rf ./tinyusb/*
	mkdir -p ./tinyusb/portable/microchip/
	cp -r $(TNY)/src/portable/microchip/samd ./tinyusb/portable/microchip/
	cp -r $(TNY)/src/class ./tinyusb/class
	cp -r $(TNY)/src/common ./tinyusb/common
	cp -r $(TNY)/src/device ./tinyusb/device
	cp -r $(TNY)/src/osal ./tinyusb/osal
	cp $(TNY)/src/tusb_option.h $(TNY)/src/tusb.h $(TNY)/src/tusb.c ./tinyusb/
	rm -rf $(TNY)
	awk -i inplace '/[\s]*#include/{gsub(/sam.h/,"$(MCU)/sam.h")}{print}' tinyusb/portable/microchip/samd/dcd_samd.c

cppcheck:
	cppcheck --check-level=exhaustive --force -Itinyusb .

cppcheck-excessive:
	echo | arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -dM -E -c - | sort > $(BUILD)/cppcheck.h
	cppcheck --enable=all --check-level=exhaustive --force -I/usr/arm-none-eabi/include/ -I/usr/lib/gcc/arm-none-eabi/14.1.0/include/ --include=$(BUILD)/cppcheck.h --platform=unix32 --quiet -I. -I tinyusb -DCFG_TUSB_CONFIG_FILE=\"$(PROJECT)/tinyusb.h\" .

analyzer: CFLAGS += -fanalyzer -fdump-analyzer-supergraph -g3
analyzer: $(PROJECT)/main.c $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) $< -o $(BUILD)/main.analyzer.elf
#	dot -Tsvg -Gsize=1920,1200 .build/main.analyzer.elf.wpa.supergraph.dot -o .build/main.svg
#	inkscape .build/main.svg

stack: LDFLAGS += -fstack-usage
stack: $(PROJECT)/main.c $(OBJS) $(LDFILE)
	$(LINK.c) $(OBJS) $(LDLIBS) $< -o $(BUILD)/main.elf
	@cat $(BUILD)/main.elf.ltrans0.ltrans.su | sort > $(BUILD)/main.su
	@rm $(BUILD)/main.elf.ltrans0.ltrans.su
	@less $(BUILD)/main.su

.PHONY:  remote flash size clean diff dbg lst nms version global usb tinyusb analyzer
.SILENT: remote flash size clean diff dbg lst nms version $(BUILD) $(BUILD)/main.hex $(BUILD)/main.uf2
