
#Makefile for nuggets

L = support
.PHONY: all clean

############## default: make all libs and programs ##########
all:
	make -C support
	make -C client
	make -C grid
	make -C server

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C support clean
	make -C client clean
	make -C grid clean
	make -C server clean
