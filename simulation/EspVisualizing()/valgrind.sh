#!/bin/sh

# Leak kinds: definite indirect possible reachable

# Literature
# - https://valgrind.org/docs/manual/mc-manual.html
# - https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto

cd build-native && ninja && valgrind \
	-s \
	--leak-check=full \
	--show-leak-kinds=definite,indirect,possible  \
	--gen-suppressions=all \
	--suppressions=../val.sup \
	./app

