PYTHON=python
SETUPFLAGS=
COMPILEFLAGS=
INSTALLFLAGS=
PYTHONS=python2.7 python3.4 python3.5

.PHONY: inplace all rebuild test_inplace test fulltests clean distclean
.PHONY: sdist install

all: README.html README.md

README.html: README.txt CHANGES.txt
	@echo | cat README.txt - CHANGES.txt | \
	    rst2html --verbose --exit-status=1 > README.html

README.md: README.txt CHANGES.txt
	@echo | cat README.txt - CHANGES.txt | \
	    pandoc --from=rst --to=markdown > $@

inplace:
	$(PYTHON) setup.py $(SETUPFLAGS) build_ext -i $(COMPILEFLAGS)

rebuild: clean all

test_inplace: inplace
	$(PYTHON) -m tests

test: test_inplace

fulltest:
	$(MAKE) clean
	@set -e; \
	for python in $(PYTHONS); do \
		$$python $(SETUPFLAGS) setup.py -q test; \
	done
	$(MAKE) clean

clean:
	@find . \( -name '*.o' -or -name '*.so' -or -name '*.sl' -or \
	           -name '*.py[cod]' -or -name README.html \) \
	    -and -type f -delete

distclean: clean
	@rm -rf build
	@rm -rf dist
	@find . \( -name '~*' -or -name '*.orig' -or -name '*.bak' -or \
	          -name 'core*' \) -and -type f  -delete


install:
	$(PYTHON) setup.py $(SETUPFLAGS) build $(COMPILEFLAGS)
	$(PYTHON) setup.py install $(INSTALLFLAGS)

dist: clean sdist manylinux2014

sdist: README.html README.md
	$(PYTHON) setup.py sdist --formats gztar,zip

manylinux2014: clean
	docker run --rm -v $(CURDIR):/io:Z \
	    -e BUILD_UID=$(shell id -u) -e BUILD_GID=$(shell id -g) \
	    quay.io/pypa/manylinux2014_x86_64:latest \
	    /io/extras/build-wheels.sh
	docker run --rm -v $(CURDIR):/io:Z \
	    -e BUILD_UID=$(shell id -u) -e BUILD_GID=$(shell id -g) \
	    quay.io/pypa/manylinux2014_i686:latest \
	    linux32 /io/extras/build-wheels.sh
	docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
	docker run --rm -v $(CURDIR):/io:Z \
            -e BUILD_UID=$(shell id -u) -e BUILD_GID=$(shell id -g) \
            quay.io/pypa/manylinux2014_aarch64:latest \
            /io/extras/build-wheels.sh
