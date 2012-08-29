#!/bin/bash
echo installing drivers...
mkdir -p /usr/local/include/nmprk
mkdir -p /usr/local/include/nmprk/ipmi
mkdir -p /usr/local/include/nmprk/translation
cp -fr ipmi/include/* /usr/local/include/nmprk/ipmi/
cp -fr translation/include/* /usr/local/include/nmprk/translation/
sed 's/i.include/i/g' /usr/local/include/nmprk/translation/nmprk_translation.h > /usr/local/include/nmprk/translation/nmprk_translation.h2
rm -f /usr/local/include/nmprk/translation/nmprk_translation.h
mv /usr/local/include/nmprk/translation/nmprk_translation.h2 /usr/local/include/nmprk/translation/nmprk_translation.h
cp ipmi/bin/libNmprkIpmi.a /usr/local/lib/
cp translation/bin/libNmprkTranslation.a /usr/local/lib/
echo installed
