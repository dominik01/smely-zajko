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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/Jazda.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/ImuThread.o \
	${OBJECTDIR}/oneLine.o \
	${OBJECTDIR}/SbotThread.o


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
LDLIBSOPTIONS=-L/usr/lib/i386-linux-gnu -L/usr/local/include/opencv2 -L/usr/local/include/opencv -lopencv_core -lopencv_highgui -lopencv_ml -lpthread -lopencv_imgproc -lopencv_legacy -lopencv_objdetect

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/guide-test_utorok

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/guide-test_utorok: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/guide-test_utorok ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/Jazda.o: Jazda.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Jazda.o Jazda.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/oneLine.o: oneLine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/oneLine.o oneLine.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I/usr/local/include/opencv2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/guide-test_utorok

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
