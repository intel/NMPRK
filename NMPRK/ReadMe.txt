Intel Node Manager Programmers Reference Kit (NMPRK) Version 2.0

NMPRK has been modified to provide a simple C API that allows for easy access from C, C++, and C# applications. NMPRK also has add support for NM 3.0 commands.

Intel(r) Node Manager is a server management technology that allows management software to accurately monitor and control the platform's power and thermal behaviors through an industry defined standard Intelligent Platform Management Interface (IPMI).  

Intel(r) Node Manager Technology allows the datacenter IT and Facilities managers to monitor actual server power and thermal behavior. Using this technology datacenter managers can set upper limits on the server power to maximize the rack density with confidence that rack power budget will not be exceeded. During a power or thermal emergency, Intel(r) Node Manager can automatically limit server power consumption and extend service uptime from standby power sources. These server's power and thermal information can also be used to improve overall datacenter efficiency and maximize overall datacenter utilization.

Intel(r) Node Manager Programmer's Reference Kit contains the source code and a test application to manage a server that support Intel Node Manager. 

The Kit simplifies the encoding/decoding of complex IPMI commands and responses, hides the protocol details and provides simple Application Programming Interfaces (APIs) to monitor and control power and thermal capabilities of Intel(r) Node Manager capable platforms. 

DOWNLOAD NOW: Intel(r) Node Manager Programmer's Reference Kit at:

http://01.org/nodemanager or https://github.com/01org/NMPRK

Please follow the links below for more information on Intel(r) Node Manager Technology:

Intel(r) Node Manager: 
http://www.intel.com/content/www/us/en/data-center/data-center-management/techrefresh-info-nodemanagerfull.html?wapkw=node+manager

Data Center Power Management with Intel(r) Node Manager:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

Intel(r) Node Manager Specification:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

Defects may now be entered in the NMPRK repository located on GitHub:
https://github.com/01org/NMPRK/issues

Source Code:
nmprkC - Contains the source code for the NMPRK library
testC - Contains a test application for NMPRK
docs - Contains the NMPRK API documentation
NMPRK.cs - C# interface for accessing NMPRK functions