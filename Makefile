PYTHON=python
SETUPFLAGS=
COMPILEFLAGS=
INSTALLFLAGS=

.PHONY: inplace all rebuild test_inplace test fulltests clean distclean
.PHONY: sdist install

all: inplace README.html

README.html: README.txt CHANGES.txt
	@echo | cat README.txt - CHANGES.txt | \
	    rst2html --verbose --exit-status=1 > README.html

inplace:
	$(PYTHON) setup.py $(SETUPFLAGS) build_ext -i $(COMPILEFLAGS)

rebuild: clean all

test_inplace: inplace
	$(PYTHON) -m tests

test: test_inplace

fulltest:
	python2.6 setup.py $(SETUPFLAGS) test
	python2.7 setup.py $(SETUPFLAGS) test
	python3.2 setup.py $(SETUPFLAGS) test
	python3.3 setup.py $(SETUPFLAGS) test
	python3.4 setup.py $(SETUPFLAGS) test

clean:
	$(PYTHON) setup.py clean --all
	find . \( -name '*.o' -or -name '*.so' -or -name '*.py[cod]' \) -delete
	rm -f README.html

distclean: clean
	rm -rf build
	rm -rf dist
	find . \( -name '~*' -or -name '*.orig' -or -name '*.bak' -or -name 'core*' \) -delete

sdist: README.html
	$(PYTHON) setup.py sdist --formats gztar,zip

install:
	$(PYTHON) setup.py $(SETUPFLAGS) build $(COMPILEFLAGS)
	$(PYTHON) setup.py install $(INSTALLFLAGS)
