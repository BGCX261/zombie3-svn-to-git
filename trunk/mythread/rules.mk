#####################################################################################
# Dependency computation
#############################

$(MYTHREAD_BASEDIR)/dep/%.d: $(MYTHREAD_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYTHREAD_BASEDIR)/module.mk $(MYTHREAD_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(BASEDIR)/scripts/depend.sh `dirname $@`/debug $(MYTHREAD_INCFLAGS) $< >$@
	$(BASEDIR)/scripts/depend.sh `dirname $@`/release $(MYTHREAD_INCFLAGS) $< >>$@

#####################################################################################
# Object compilation
#############################
$(MYTHREAD_BASEDIR)/obj/debug/%.o: $(MYTHREAD_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYTHREAD_BASEDIR)/module.mk $(MYTHREAD_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYTHREAD_CC) -c $(MYTHREAD_DEBUG_CFLAGS) $< -o $@

$(MYTHREAD_BASEDIR)/obj/release/%.o: $(MYTHREAD_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYTHREAD_BASEDIR)/module.mk $(MYTHREAD_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYTHREAD_CC) -c $(MYTHREAD_RELEASE_CFLAGS) $< -o $@

$(DEBUG_MYTHREAD_ARCHIVE): $(MYTHREAD_DEBUG_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(RELEASE_MYTHREAD_ARCHIVE): $(MYTHREAD_RELEASE_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(DEBUG_MYTHREAD_LIB): $(MYTHREAD_DEBUG_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating dynamic library $@"
	@echo "#########"
	$(MYTHREAD_LD) -shared -o $@ $^ 

$(RELEASE_MYTHREAD_LIB): $(MYTHREAD_RELEASE_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating dynamic library $@"
	@echo "#########"
	$(MYTHREAD_LD) -shared -o $@ $^ 
