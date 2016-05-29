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
	${OBJECTDIR}/_ext/771811176/CrclInterface.o \
	${OBJECTDIR}/_ext/771811176/LinuxGlobals.o \
	${OBJECTDIR}/_ext/771811176/XmlSocketClient.o \
	${OBJECTDIR}/_ext/1881829329/CRCLCommandInstance.o \
	${OBJECTDIR}/_ext/1881829329/CRCLCommands.o \
	${OBJECTDIR}/_ext/1881829329/CRCLProgramInstance.o \
	${OBJECTDIR}/_ext/1881829329/CRCLStatus.o \
	${OBJECTDIR}/_ext/1881829329/DataPrimitives.o \
	${OBJECTDIR}/adapter-master/src/adapter.o \
	${OBJECTDIR}/adapter-master/src/client.o \
	${OBJECTDIR}/adapter-master/src/condition.o \
	${OBJECTDIR}/adapter-master/src/configuration.o \
	${OBJECTDIR}/adapter-master/src/cutting_tool.o \
	${OBJECTDIR}/adapter-master/src/device_datum.o \
	${OBJECTDIR}/adapter-master/src/logger.o \
	${OBJECTDIR}/adapter-master/src/server.o \
	${OBJECTDIR}/adapter-master/src/service.o \
	${OBJECTDIR}/adapter-master/src/string_array.o \
	${OBJECTDIR}/adapter-master/src/string_buffer.o \
	${OBJECTDIR}/adapter-master/src/time_series.o \
	${OBJECTDIR}/adapter-master/yaml/src/aliascontent.o \
	${OBJECTDIR}/adapter-master/yaml/src/conversion.o \
	${OBJECTDIR}/adapter-master/yaml/src/emitter.o \
	${OBJECTDIR}/adapter-master/yaml/src/emitterstate.o \
	${OBJECTDIR}/adapter-master/yaml/src/emitterutils.o \
	${OBJECTDIR}/adapter-master/yaml/src/exp.o \
	${OBJECTDIR}/adapter-master/yaml/src/iterator.o \
	${OBJECTDIR}/adapter-master/yaml/src/map.o \
	${OBJECTDIR}/adapter-master/yaml/src/node.o \
	${OBJECTDIR}/adapter-master/yaml/src/null.o \
	${OBJECTDIR}/adapter-master/yaml/src/ostream.o \
	${OBJECTDIR}/adapter-master/yaml/src/parser.o \
	${OBJECTDIR}/adapter-master/yaml/src/parserstate.o \
	${OBJECTDIR}/adapter-master/yaml/src/regex.o \
	${OBJECTDIR}/adapter-master/yaml/src/scalar.o \
	${OBJECTDIR}/adapter-master/yaml/src/scanner.o \
	${OBJECTDIR}/adapter-master/yaml/src/scanscalar.o \
	${OBJECTDIR}/adapter-master/yaml/src/scantag.o \
	${OBJECTDIR}/adapter-master/yaml/src/scantoken.o \
	${OBJECTDIR}/adapter-master/yaml/src/sequence.o \
	${OBJECTDIR}/adapter-master/yaml/src/simplekey.o \
	${OBJECTDIR}/adapter-master/yaml/src/stream.o \
	${OBJECTDIR}/adapter-master/yaml/src/tag.o \
	${OBJECTDIR}/fake_adapter.o \
	${OBJECTDIR}/main.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crlcadapter

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crlcadapter: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crlcadapter ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/771811176/CrclInterface.o: /home/michalos/NetBeansProjects/CrlcAdapter/CrclInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/771811176
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/771811176/CrclInterface.o /home/michalos/NetBeansProjects/CrlcAdapter/CrclInterface.cpp

${OBJECTDIR}/_ext/771811176/LinuxGlobals.o: /home/michalos/NetBeansProjects/CrlcAdapter/LinuxGlobals.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/771811176
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/771811176/LinuxGlobals.o /home/michalos/NetBeansProjects/CrlcAdapter/LinuxGlobals.cpp

${OBJECTDIR}/_ext/771811176/XmlSocketClient.o: /home/michalos/NetBeansProjects/CrlcAdapter/XmlSocketClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/771811176
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/771811176/XmlSocketClient.o /home/michalos/NetBeansProjects/CrlcAdapter/XmlSocketClient.cpp

${OBJECTDIR}/_ext/1881829329/CRCLCommandInstance.o: /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLCommandInstance.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881829329
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1881829329/CRCLCommandInstance.o /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLCommandInstance.cxx

${OBJECTDIR}/_ext/1881829329/CRCLCommands.o: /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLCommands.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881829329
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1881829329/CRCLCommands.o /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLCommands.cxx

${OBJECTDIR}/_ext/1881829329/CRCLProgramInstance.o: /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLProgramInstance.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881829329
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1881829329/CRCLProgramInstance.o /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLProgramInstance.cxx

${OBJECTDIR}/_ext/1881829329/CRCLStatus.o: /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLStatus.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881829329
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1881829329/CRCLStatus.o /home/michalos/NetBeansProjects/CrlcAdapter/crcl/CRCLStatus.cxx

${OBJECTDIR}/_ext/1881829329/DataPrimitives.o: /home/michalos/NetBeansProjects/CrlcAdapter/crcl/DataPrimitives.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1881829329
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1881829329/DataPrimitives.o /home/michalos/NetBeansProjects/CrlcAdapter/crcl/DataPrimitives.cxx

${OBJECTDIR}/adapter-master/src/adapter.o: adapter-master/src/adapter.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/adapter.o adapter-master/src/adapter.cpp

${OBJECTDIR}/adapter-master/src/client.o: adapter-master/src/client.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/client.o adapter-master/src/client.cpp

${OBJECTDIR}/adapter-master/src/condition.o: adapter-master/src/condition.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/condition.o adapter-master/src/condition.cpp

${OBJECTDIR}/adapter-master/src/configuration.o: adapter-master/src/configuration.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/configuration.o adapter-master/src/configuration.cpp

${OBJECTDIR}/adapter-master/src/cutting_tool.o: adapter-master/src/cutting_tool.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/cutting_tool.o adapter-master/src/cutting_tool.cpp

${OBJECTDIR}/adapter-master/src/device_datum.o: adapter-master/src/device_datum.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/device_datum.o adapter-master/src/device_datum.cpp

${OBJECTDIR}/adapter-master/src/logger.o: adapter-master/src/logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/logger.o adapter-master/src/logger.cpp

${OBJECTDIR}/adapter-master/src/server.o: adapter-master/src/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/server.o adapter-master/src/server.cpp

${OBJECTDIR}/adapter-master/src/service.o: adapter-master/src/service.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/service.o adapter-master/src/service.cpp

${OBJECTDIR}/adapter-master/src/string_array.o: adapter-master/src/string_array.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/string_array.o adapter-master/src/string_array.cpp

${OBJECTDIR}/adapter-master/src/string_buffer.o: adapter-master/src/string_buffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/string_buffer.o adapter-master/src/string_buffer.cpp

${OBJECTDIR}/adapter-master/src/time_series.o: adapter-master/src/time_series.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/src/time_series.o adapter-master/src/time_series.cpp

${OBJECTDIR}/adapter-master/yaml/src/aliascontent.o: adapter-master/yaml/src/aliascontent.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/aliascontent.o adapter-master/yaml/src/aliascontent.cpp

${OBJECTDIR}/adapter-master/yaml/src/conversion.o: adapter-master/yaml/src/conversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/conversion.o adapter-master/yaml/src/conversion.cpp

${OBJECTDIR}/adapter-master/yaml/src/emitter.o: adapter-master/yaml/src/emitter.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/emitter.o adapter-master/yaml/src/emitter.cpp

${OBJECTDIR}/adapter-master/yaml/src/emitterstate.o: adapter-master/yaml/src/emitterstate.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/emitterstate.o adapter-master/yaml/src/emitterstate.cpp

${OBJECTDIR}/adapter-master/yaml/src/emitterutils.o: adapter-master/yaml/src/emitterutils.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/emitterutils.o adapter-master/yaml/src/emitterutils.cpp

${OBJECTDIR}/adapter-master/yaml/src/exp.o: adapter-master/yaml/src/exp.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/exp.o adapter-master/yaml/src/exp.cpp

${OBJECTDIR}/adapter-master/yaml/src/iterator.o: adapter-master/yaml/src/iterator.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/iterator.o adapter-master/yaml/src/iterator.cpp

${OBJECTDIR}/adapter-master/yaml/src/map.o: adapter-master/yaml/src/map.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/map.o adapter-master/yaml/src/map.cpp

${OBJECTDIR}/adapter-master/yaml/src/node.o: adapter-master/yaml/src/node.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/node.o adapter-master/yaml/src/node.cpp

${OBJECTDIR}/adapter-master/yaml/src/null.o: adapter-master/yaml/src/null.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/null.o adapter-master/yaml/src/null.cpp

${OBJECTDIR}/adapter-master/yaml/src/ostream.o: adapter-master/yaml/src/ostream.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/ostream.o adapter-master/yaml/src/ostream.cpp

${OBJECTDIR}/adapter-master/yaml/src/parser.o: adapter-master/yaml/src/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/parser.o adapter-master/yaml/src/parser.cpp

${OBJECTDIR}/adapter-master/yaml/src/parserstate.o: adapter-master/yaml/src/parserstate.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/parserstate.o adapter-master/yaml/src/parserstate.cpp

${OBJECTDIR}/adapter-master/yaml/src/regex.o: adapter-master/yaml/src/regex.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/regex.o adapter-master/yaml/src/regex.cpp

${OBJECTDIR}/adapter-master/yaml/src/scalar.o: adapter-master/yaml/src/scalar.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/scalar.o adapter-master/yaml/src/scalar.cpp

${OBJECTDIR}/adapter-master/yaml/src/scanner.o: adapter-master/yaml/src/scanner.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/scanner.o adapter-master/yaml/src/scanner.cpp

${OBJECTDIR}/adapter-master/yaml/src/scanscalar.o: adapter-master/yaml/src/scanscalar.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/scanscalar.o adapter-master/yaml/src/scanscalar.cpp

${OBJECTDIR}/adapter-master/yaml/src/scantag.o: adapter-master/yaml/src/scantag.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/scantag.o adapter-master/yaml/src/scantag.cpp

${OBJECTDIR}/adapter-master/yaml/src/scantoken.o: adapter-master/yaml/src/scantoken.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/scantoken.o adapter-master/yaml/src/scantoken.cpp

${OBJECTDIR}/adapter-master/yaml/src/sequence.o: adapter-master/yaml/src/sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/sequence.o adapter-master/yaml/src/sequence.cpp

${OBJECTDIR}/adapter-master/yaml/src/simplekey.o: adapter-master/yaml/src/simplekey.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/simplekey.o adapter-master/yaml/src/simplekey.cpp

${OBJECTDIR}/adapter-master/yaml/src/stream.o: adapter-master/yaml/src/stream.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/stream.o adapter-master/yaml/src/stream.cpp

${OBJECTDIR}/adapter-master/yaml/src/tag.o: adapter-master/yaml/src/tag.cpp 
	${MKDIR} -p ${OBJECTDIR}/adapter-master/yaml/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/adapter-master/yaml/src/tag.o adapter-master/yaml/src/tag.cpp

${OBJECTDIR}/fake_adapter.o: fake_adapter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/fake_adapter.o fake_adapter.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crlcadapter

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
