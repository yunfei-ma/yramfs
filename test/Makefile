
objs = test.o

test : $(objs)
	@echo compiling test...
	@gcc -lpthread $(objs) -lpthread -o test -lpthread

$(objs) : %.o: %.c
	@echo compiling $@...
	@gcc -c -lpthread $< -o $@

.PHONY: clean
clean:
	@echo cleaning...
	-rm -f *.o *.out test
