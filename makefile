
all:
	make -C XMLConf
	make -C Example

clean: 
	make -C XMLConf clean
	make -C Example clean
	
.phony: clean all 