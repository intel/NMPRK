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
# * Updates:							                                      *
# * 4/30: prep for initial external release                                *
# **************************************************************************/
 
cd ../ipmi/
./make.sh
cd -

mkdir -p bin
rm -fr bin/*

g++ -g -c -fPIC src/nmprk_translation.cpp -o bin/nmprk_translation.o -Iinclude/ -I../
g++ -g -c -fPIC src/modules/dcmi/dcmi.cpp -o bin/nmprk_translation_dcmi.o -Iinclude/ -I../
g++ -g -c -fPIC src/modules/nm_dnm/nm_dnm.cpp -o bin/nmprk_translation_nm_dnm.o -Iinclude/ -I../

ar rcs bin/libNmprkTranslation.a ../ipmi/bin/libNmprkIpmi.a bin/nmprk_translation.o bin/nmprk_translation_dcmi.o bin/nmprk_translation_nm_dnm.o


