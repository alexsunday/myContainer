all:
	python3 setup.py build
	cp build/lib*/clone*.so clone.so
