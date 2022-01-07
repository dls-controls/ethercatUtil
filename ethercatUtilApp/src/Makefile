TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build a Diamond Support Module
# -------------------------------

# Let's get those wonderful C++ 11 features
USR_CXXFLAGS_Linux += -std=c++11

LIBRARY_IOC = ethercatUtil

# directoryMonitor.dbd will be installed into <top>/dbd
DBD += ethercatUtil.dbd

# directoryMonitor.dbd will be created from these files
ethercatUtil_DBD += base.dbd
ethercatUtil_DBD += asyn.dbd
ethercatUtil_DBD += ethercatUtilRegister.dbd

# Source code
ethercatUtil_SRCS += ELM3704.cpp

# Library dependencies
ethercatUtil_LIBS += asyn

# We need to link this IOC Application against the EPICS Base libraries
ethercatUtil_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
