all:
	@cd latex && $(MAKE)
.PHONY: all

clean:
	@cd latex && $(MAKE) clean
.PHONY: clean
