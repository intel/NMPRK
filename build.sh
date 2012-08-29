#!/bin/bash

chmod +x ipmi/make.sh
chmod +x ipmi/test/make.sh
chmod +x translation/make.sh
chmod +x translation/test/make.sh
chmod +x install.sh

cd ipmi/test
./make.sh
cd ../../translation/test
./make.sh
cd ../..

