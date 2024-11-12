PROJECT := $(notdir $(lastword $(CURDIR)))
all:
	@ $(MAKE) --no-print-directory -C .. PROJECT=$(PROJECT) $@
%:
	@ $(MAKE) --no-print-directory -C .. PROJECT=$(PROJECT) $@
