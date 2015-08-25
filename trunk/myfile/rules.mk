PRECIOUS: $(MYFILE_BASEDIR)/obj/%.cpp $(MYFILE_BASEDIR)/obj/%.h

#####################################################################################
# Dependency computation
#############################
$(MYFILE_BASEDIR)/dep/%.d: $(MYFILE_BASEDIR)/obj/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(BASEDIR)/scripts/depend.sh `dirname $@`/debug $(MYFILE_INCFLAGS) $< >$@
	$(BASEDIR)/scripts/depend.sh `dirname $@`/release $(MYFILE_INCFLAGS) $< >>$@

$(MYFILE_BASEDIR)/dep/%.d: $(MYFILE_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(BASEDIR)/scripts/depend.sh `dirname $@`/debug $(MYFILE_INCFLAGS) $< >$@
	$(BASEDIR)/scripts/depend.sh `dirname $@`/release $(MYFILE_INCFLAGS) $< >>$@


#####################################################################################
# Object compilation
#############################
$(MYFILE_BASEDIR)/obj/%.cpp $(MYFILE_BASEDIR)/obj/%.h: $(MYFILE_BASEDIR)/src/%.y $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# YACC generation of $@ from $<"
	@echo "#########"
	bison -d $<
	mv *.tab.c $(MYFILE_BASEDIR)/obj/$*.cpp
	mv *.tab.h $(MYFILE_BASEDIR)/obj/$*.h


$(MYFILE_BASEDIR)/obj/debug/%.o: $(MYFILE_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_DEBUG_CFLAGS) $< -o $@

$(MYFILE_BASEDIR)/obj/release/%.o: $(MYFILE_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $<"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_RELEASE_CFLAGS) $< -o $@

$(MYFILE_BASEDIR)/obj/scrambled/%.o: $(MYFILE_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $<"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_SCRAMBLED_CFLAGS) $< -o $@

$(MYFILE_BASEDIR)/obj/debug/%.o: $(MYFILE_BASEDIR)/obj/%.cpp
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_DEBUG_CFLAGS) $< -o $@

$(MYFILE_BASEDIR)/obj/release/%.o: $(MYFILE_BASEDIR)/obj/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_RELEASE_CFLAGS) $< -o $@

$(MYFILE_BASEDIR)/obj/scrambled/%.o: $(MYFILE_BASEDIR)/obj/%.cpp $(BASEDIR)/Makefile $(MYFILE_BASEDIR)/module.mk $(MYFILE_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYFILE_CC) -c $(MYFILE_SCRAMBLED_CFLAGS) $< -o $@

$(DEBUG_MYFILE_ARCHIVE): $(MYFILE_DEBUG_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(RELEASE_MYFILE_ARCHIVE): $(MYFILE_RELEASE_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(SCRAMBLED_MYFILE_ARCHIVE): $(MYFILE_SCRAMBLED_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 
