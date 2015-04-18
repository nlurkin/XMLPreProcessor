
all: XMLConf Example

XMLConf:
	make -C XMLConf
	
Example:
	make -C Example

clean: 
	make -C XMLConf clean
	make -C Example clean
	
.phony: clean all