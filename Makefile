all: debug release
.PHONY: all

debug:
	node-gyp -d rebuild
	cp build/Debug/xmlgoo.node lib
	touch spec/buster.js

release:
	node-gyp rebuild
	cp build/Release/xmlgoo.node lib