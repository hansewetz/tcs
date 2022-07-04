all: test1  test1-completion.bash

test1:
	$(MAKE) -f Makefile.cc test1

test1-completion.bash: test1
	test1 twmnbm-bash > $@

clean:
	$(MAKE) -f Makefile.cc clean
	rm -f test1-completion.bash

