#!/bin/sh

# Install
# apt install massif-visualizer

# Literature
# - https://valgrind.org/docs/manual/ms-manual.html
# - https://github.com/KDE/massif-visualizer

cd build-native && ninja && valgrind \
	--tool=massif \
	--massif-out-file=massif.out \
	./app

massif-visualizer massif.out &

