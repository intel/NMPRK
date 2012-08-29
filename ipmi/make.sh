#!/bin/bash
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
# * Updates:							                                   *
# * 4/30: prep for initial external release                                *
# **************************************************************************/

mkdir -p bin
rm -fr bin/*

g++ -g -c -fPIC src/nmprk_ipmi.cpp -o bin/nmprk_ipmi.o -Iinclude/
g++ -g -c -fPIC src/nmprk_ipmi_sel.cpp -o bin/nmprk_ipmi_sel.o -Iinclude/
g++ -g -c -fPIC src/nmprk_ipmi_sdr.cpp -o bin/nmprk_ipmi_sdr.o -Iinclude/
g++ -g -c -fPIC src/nmprk_ipmi_fru.cpp -o bin/nmprk_ipmi_fru.o -Iinclude/
g++ -g -c -fPIC src/nmprk_ipmi_global.cpp -o bin/nmprk_ipmi_global.o -Iinclude/
#g++ -g -c -fPIC src/nmprk_ipmi_alert.cpp  -o bin/nmprk_ipmi_alert.o -Iinclude/
gcc -g -c -fPIC src/modules/nm_dcmi/linux/auth.c -o bin/nmprk_ipmi_auth.o -Iinclude/
gcc -g -c -fPIC src/modules/nm_dcmi/linux/md5.c  -o bin/nmprk_ipmi_md5.o -Iinclude/
gcc -g -c -fPIC src/modules/nm_dcmi/linux/ipmi_strings.c -o bin/nmprk_ipmi_strings.o -Iinclude/
#gcc -g -c -fPIC src/modules/nm_dcmi/linux/helper.c       -o bin/nmprk_ipmi_helper.o
g++ -g -c -fPIC src/modules/nm_dcmi/nm_dcmi.cpp -o bin/nmprk_nm_dcmi.o -Iinclude/
#g++ -g -c -fPIC src/modules/nm_dcmi/open.cpp  -o bin/nmprk_nm_dcmi_open.o -Iinclude/
g++ -g -c -fPIC src/modules/dnm/dnm.cpp   -o bin/nmprk_dnm.o -Iinclude/
g++ -g -c -fPIC src/nmprk_helper.cpp      -o bin/nmprk_helper.o -Iinclude/
ar rcs bin/libNmprkIpmi.a bin/nmprk_ipmi_auth.o bin/nmprk_ipmi.o  bin/nmprk_ipmi_sel.o bin/nmprk_ipmi_sdr.o bin/nmprk_ipmi_fru.o bin/nmprk_ipmi_global.o bin/nmprk_helper.o bin/nmprk_nm_dcmi.o bin/nmprk_dnm.o bin/nmprk_nm_dcmi.o bin/nmprk_ipmi_md5.o bin/nmprk_ipmi_strings.o 
#bin/nmprk_alert.o

