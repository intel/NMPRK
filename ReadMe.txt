Intel Node Manager Programmers Reference Kit (NMPKR)

Version 1.00							Date: August 1, 2012

Intel(r) Node Manager is a server management technology that allows management software to accurately monitor and control the platform's power and thermal behaviors through an industry defined standard Intelligent Platform Management Interface (IPMI) and Datacenter Manageability Interface (DCMI).  

Intel(r) Node Manager Technology allows the datacenter IT and Facilities managers to monitor actual server power and thermal behavior. Using this technology datacenter managers can set upper limits on the server power to maximize the rack density with confidence that rack power budget will not be exceeded. During a power or thermal emergency, Intel(r) Node Manager can automatically limit server power consumption and extend service uptime from standby power sources. These server's power and thermal information can also be used to improve overall datacenter efficiency and maximize overall datacenter utilization.

Intel(r) Node Manager Programmer's Reference Kit contains the source code (C and C++), test application, and documents to manage a server that support Intel Node Manager. 

The Kit simplifies the encoding/decoding of complex IPMI commands and responses, hides the protocol details and provides simple Application Programming Interfaces (APIs) to monitor and control power and thermal capabilities of Intel(r) Node Manager capable platforms. 

DOWNLOAD NOW: Intel(r) Node Manager Programmer's Reference Kit

Intel(r) has three version of server management technology - Intel(r) Node Manager, Intel(r) Node Manager Basic Edition and Intel(r) Node Manager Extended Edition. Intel(r) Node Manager provides platform's power and thermal capabilities to baseboard management controller (BMC). Intel(r) Node Manager Basic Edition provides simple manageability capabilities and does not need platform's BMC. Intel(r) Node Manager Extended Edition provides simple platforms' manageability capabilities along with power and thermal capabilities and does not need platform's BMC. Intel(r) Node Manager Programmer's Reference Kit supports all the three version of server management technology. 

Please follow the links below for more information on Intel(r) Node Manager Technology:

Intel(r) Node Manager: 
http://www.intel.com/content/www/us/en/data-center/data-center-management/techrefresh-info-nodemanagerfull.html?wapkw=node+manager

Data Center Power Management with Intel(r) Node Manager:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

Intel(r) Node Manager Specification:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

Known Issues and Limitations to the NMPRK:

* Currently the DNM interface has not been implemented.

* There currently is no support for the DCMI HECI/MEI drivers, which means the DCMI interface will not work in-band on a DCMI machine unless the IMB drivers have been installed and are functional.

* Currently there is no support for partial SDR reads.


NMPRK Testing:

The NMPRK has been tested on both Linux and Windows server platforms.  The two OS's that were used during test were Microsoft(r) Windows Server 2008 R2 and RedHat(r) Enterprise Linux 6.0.  The test programs used are included in the package.  The TestIpmi program tests the IPMI interface to the library and the TestTranslation program tests the translation layer interface to the library.

