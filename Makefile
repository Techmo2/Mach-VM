VM_SOURCE_DIR=./src

all: mach

clean:
	rm mach
	cd $(VM_SOURCE_DIR) && $(MAKE) clean
    
mach:
	cd $(VM_SOURCE_DIR) && $(MAKE)
	cp $(VM_SOURCE_DIR)/mach ./mach
