VM_SOURCE_DIR=./src

all: mach

clean:
	rm mach
	cd $(VM_SOURCE_DIR) && $(MAKE) clean

debug:
	cd $(VM_SOURCE_DIR) && $(MAKE) debug
	cp $(VM_SOURCE_DIR)/mach ./mach

mach:
	cd $(VM_SOURCE_DIR) && $(MAKE)
	cp $(VM_SOURCE_DIR)/mach ./mach
