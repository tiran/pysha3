PYTHON=python
SETUPFLAGS=
COMPILEFLAGS=
INSTALLFLAGS=

.PHONY: inplace all rebuild test_inplace test fulltests clean distclean
.PHONY: sdist install

inplace:
	$(PYTHON) setup.py $(SETUPFLAGS) build_ext -i $(COMPILEFLAGS)

all: inplace

rebuild: clean all

test_inplace: inplace
	$(PYTHON) -m tests

test: test_inplace

fulltest:
	$(MAKE) PYTHON=python2.7 clean test
	$(MAKE) PYTHON=python3.2 clean test
	$(MAKE) PYTHON=python3.3 clean test

clean:
	$(PYTHON) setup.py clean --all
	find . \( -name '*.o' -or -name '*.so' -or -name '*.py[cod]' \) -delete

distclean: clean
	rm -rf build
	rm -rf dist

sdist:
	$(PYTHON) setup.py sdist --formats gztar,zip

install:
	$(PYTHON) setup.py $(SETUPFLAGS) build $(COMPILEFLAGS)
	$(PYTHON) setup.py install $(INSTALLFLAGS)
