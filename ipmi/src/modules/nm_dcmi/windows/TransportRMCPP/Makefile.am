#/***************************************************************************
# * Copyright 2012 Intel Corporation                                        *
# *Licensed under the Apache License, Version 2.0 (the "License");          *
# * you may not use this file except in compliance with the License.        *
# * You may obtain a copy of the License at                                 *
# * http://www.apache.org/licenses/LICENSE-2.0                              *
# * Unless required by applicable law or agreed to in writing, software     *
# * distributed under the License is distributed on an "AS IS" BASIS,       *
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
# * See the License for the specific language governing permissions and     *
# * limitations under the License.                                          *
# ***************************************************************************/
#/**************************************************************************
# * Author: Stewart Dale <IASI NM TEAM>                                    *
# * Updates:							                                      *
# * 4/30: prep for initial external release                                *
# **************************************************************************/

COMMONDIR = $(srcdir)/../Common

BUILDVERSION_MAIN = $(word 3, $(shell cat ${srcdir}/version.h | grep "^\#define[[:space:]]\+MAJOR_VERSION"))
BUILDVERSION_MINOR = $(word 3, $(shell cat ${srcdir}/version.h | grep "^\#define[[:space:]]\+MINOR_VERSION"))
BUILDVERSION_FIX = $(word 3, $(shell cat ${srcdir}/version.h | grep "^\#define[[:space:]]\+QUICK_FIX_NUMBER"))
BUILDVERSION_BUILD = $(word 3, $(shell cat ${srcdir}/../../Build.h | grep "^\#define[[:space:]]\+VER_BUILD"))

lib_LTLIBRARIES = libTransportRMCPP.la
libTransportRMCPP_la_LIBADD = $(OPENSSLLIBS)
libTransportRMCPP_la_SOURCES =	IpmiRequest.h \
				IpmiRequest.cpp \
				RmcppTransportPlugin.cpp \
				ByteVector.h \
				ByteVector.cpp \
				CryptoProxy.h \
				CryptoProxyLinux.cpp \
				IpmiError.h \
				IpmiError.cpp \
				IpmiHeader.h \
				IpmiHeader.cpp \
				IpmiPayload.h \
				IpmiPayload.cpp \
				IpmiSession.h \
				IpmiSession.cpp \
				RmcppTransportPluginImpl.h \
				RmcppTransportPluginImpl.cpp \
				IpmiResponse.h \
				IpmiResponse.cpp \
				RmcppTypes.cpp \
				RmcppTypes.h \
				version.h \
				../../VersionCommon.h \
				../../Build.h \
				$(COMMONDIR)/TransportCommon.h \
				$(COMMONDIR)/TransportCommon.cpp \
				$(COMMONDIR)/LogLevel.h \
				$(COMMONDIR)/RemoteLogger.h \
				$(COMMONDIR)/RemoteLogger.cpp \
				$(COMMONDIR)/OsSpecificSockets.h \
				$(COMMONDIR)/OsSpecificCritSect.h \
				$(COMMONDIR)/OsSpecificCritSectLinux.h \
				$(COMMONDIR)/OsSpecificCritSectLinux.cpp \
				$(COMMONDIR)/OsSpecificFunctions.h \
				$(COMMONDIR)/OsSpecificFunctionsLinux.cpp
libTransportRMCPP_la_LDFLAGS = -version-info $(BUILDVERSION_MAIN):$(BUILDVERSION_BUILD):0
AM_CPPFLAGS = -I $(srcdir)/.. -I $(COMMONDIR) -I $(srcdir) $(OPENSSLINCS)

