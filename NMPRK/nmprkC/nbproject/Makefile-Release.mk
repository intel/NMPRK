#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/RmcppTypes.o \
	${OBJECTDIR}/LastError.o \
	${OBJECTDIR}/CryptoProxyLinux.o \
	${OBJECTDIR}/IpmiProxy.o \
	${OBJECTDIR}/RmcppIpmiProxy.o \
	${OBJECTDIR}/nmprkUtilities.o \
	${OBJECTDIR}/ByteVector.o \
	${OBJECTDIR}/RmcppIpmiError.o \
	${OBJECTDIR}/RmcppIpmiPayload.o \
	${OBJECTDIR}/OpenIpmiProxy.o \
	${OBJECTDIR}/transportInterfaceSlim.o \
	${OBJECTDIR}/transportInterfaceOriginal.o \
	${OBJECTDIR}/RmcppIpmiSession.o \
	${OBJECTDIR}/RmcppIpmiHeader.o \
	${OBJECTDIR}/transportInterface.o \
	${OBJECTDIR}/RmcppIpmiRequest.o \
	${OBJECTDIR}/RmcppIpmiResponse.o \
	${OBJECTDIR}/nmprkC.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a

${OBJECTDIR}/RmcppTypes.o: RmcppTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppTypes.o RmcppTypes.cpp

${OBJECTDIR}/LastError.o: LastError.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/LastError.o LastError.cpp

${OBJECTDIR}/CryptoProxyLinux.o: CryptoProxyLinux.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/CryptoProxyLinux.o CryptoProxyLinux.cpp

${OBJECTDIR}/IpmiProxy.o: IpmiProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/IpmiProxy.o IpmiProxy.cpp

${OBJECTDIR}/RmcppIpmiProxy.o: RmcppIpmiProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiProxy.o RmcppIpmiProxy.cpp

${OBJECTDIR}/nmprkUtilities.o: nmprkUtilities.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/nmprkUtilities.o nmprkUtilities.cpp

${OBJECTDIR}/ByteVector.o: ByteVector.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ByteVector.o ByteVector.cpp

${OBJECTDIR}/RmcppIpmiError.o: RmcppIpmiError.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiError.o RmcppIpmiError.cpp

${OBJECTDIR}/RmcppIpmiPayload.o: RmcppIpmiPayload.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiPayload.o RmcppIpmiPayload.cpp

${OBJECTDIR}/OpenIpmiProxy.o: OpenIpmiProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/OpenIpmiProxy.o OpenIpmiProxy.cpp

${OBJECTDIR}/transportInterfaceSlim.o: transportInterfaceSlim.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/transportInterfaceSlim.o transportInterfaceSlim.cpp

${OBJECTDIR}/transportInterfaceOriginal.o: transportInterfaceOriginal.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/transportInterfaceOriginal.o transportInterfaceOriginal.cpp

${OBJECTDIR}/RmcppIpmiSession.o: RmcppIpmiSession.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiSession.o RmcppIpmiSession.cpp

${OBJECTDIR}/RmcppIpmiHeader.o: RmcppIpmiHeader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiHeader.o RmcppIpmiHeader.cpp

${OBJECTDIR}/transportInterface.o: transportInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/transportInterface.o transportInterface.cpp

${OBJECTDIR}/RmcppIpmiRequest.o: RmcppIpmiRequest.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiRequest.o RmcppIpmiRequest.cpp

${OBJECTDIR}/RmcppIpmiResponse.o: RmcppIpmiResponse.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RmcppIpmiResponse.o RmcppIpmiResponse.cpp

${OBJECTDIR}/nmprkC.o: nmprkC.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/nmprkC.o nmprkC.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnmprkc.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
