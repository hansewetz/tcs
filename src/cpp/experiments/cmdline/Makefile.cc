# To be built
LOCAL_TARGET    =  test1 
LOCAL_SOTARGET  = 
LOCAL_OBJS      = test1.o \
                  CmdBase.o NopCmd.o DummyCmd.o \
	          CmdHelper.o

# Control.
LOCAL_LIBS      = -lutils -lboost_program_options
LOCAL_LIBPATH   = -L$(TCS_LIBDIR)
LOCAL_INCPATH   = 
LOCAL_DEFINES   = 
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  =
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules


test1-completion.bash: test1
	test1 twmnbm-bash  > $@

