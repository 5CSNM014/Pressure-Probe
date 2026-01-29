# SPDX-FileCopyrightText: 2003 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = PProbe
# PProbe.dbd will be created and installed
DBD += PProbe.dbd

# PProbe.dbd will be made up from these files:
PProbe_DBD += base.dbd
PProbe_DBD += asyn.dbd # <--------- 追加(1)
PProbe_DBD += stream.dbd # <--------- 追加(2)
PProbe_DBD += drvAsynSerialPort.dbd # <--------- 追加(3)

# Include dbd files from all support applications:
#PProbe_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#PProbe_LIBS += xxx

# PProbe_registerRecordDeviceDriver.cpp derives from PProbe.dbd
PProbe_SRCS += PProbe_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
PProbe_SRCS_DEFAULT += PProbeMain.cpp
PProbe_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#PProbe_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
PProbe_LIBS += $(EPICS_BASE_IOC_LIBS)
PProbe_LIBS += asyn  # <--------- 追加(4)
PProbe_LIBS += stream  # <--------- 追加(5)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

