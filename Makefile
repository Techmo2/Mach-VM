VM_SOURCE_DIR=./src
MASM_SOURCE_DIR=./asm

all: mach masm

clean:
	rm mach
	rm masm
	cd $(VM_SOURCE_DIR) && $(MAKE) clean
	cd $(MASM_SOURCE_DIR) && $(MAKE) clean

debug:
	cd $(VM_SOURCE_DIR) && $(MAKE) debug
	cd $(MASM_SOURCE_DIR) && $(MAKE) debug
	cp $(VM_SOURCE_DIR)/mach ./mach
	cp $(MASM_SOURCE_DIR)/masm ./masm
    
mach:
	cd $(VM_SOURCE_DIR) && $(MAKE)
	cp $(VM_SOURCE_DIR)/mach ./mach

masm:
	cd $(MASM_SOURCE_DIR) && $(MAKE)
	cp $(MASM_SOURCE_DIR)/masm ./masm
