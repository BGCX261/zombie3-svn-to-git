#####################################################################################
# Dependency computation
#############################

$(MYNET_BASEDIR)/dep/%.d: $(MYNET_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYNET_BASEDIR)/module.mk $(MYNET_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(BASEDIR)/scripts/depend.sh `dirname $@`/debug $(MYNET_INCFLAGS) $< >$@
	$(BASEDIR)/scripts/depend.sh `dirname $@`/release $(MYNET_INCFLAGS) $< >>$@

#####################################################################################
# Object compilation
#############################
$(MYNET_BASEDIR)/obj/debug/%.o: $(MYNET_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYNET_BASEDIR)/module.mk $(MYNET_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYNET_CC) -c $(MYNET_DEBUG_CFLAGS) $< -o $@

$(MYNET_BASEDIR)/obj/release/%.o: $(MYNET_BASEDIR)/src/%.cpp $(BASEDIR)/Makefile $(MYNET_BASEDIR)/module.mk $(MYNET_BASEDIR)/rules.mk
	@echo 
	@echo "################################################################"
	@echo "# Generating $@"
	@echo "#########"
	$(MYNET_CC) -c $(MYNET_RELEASE_CFLAGS) $< -o $@

$(DEBUG_MYNET_ARCHIVE): $(MYNET_DEBUG_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(RELEASE_MYNET_ARCHIVE): $(MYNET_RELEASE_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating archive $@"
	@echo "#########"
	ar -rcv $@ $^ 

$(DEBUG_MYNET_LIB): $(MYNET_DEBUG_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating dynamic library $@"
	@echo "#########"
	$(MYNET_LD) -shared -o $@ $^ 

$(RELEASE_MYNET_LIB): $(MYNET_RELEASE_OBJS)
	@echo 
	@echo "################################################################"
	@echo "# Creating dynamic library $@"
	@echo "#########"
	$(MYNET_LD) -shared -o $@ $^ 
