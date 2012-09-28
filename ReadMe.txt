Intel Node Manager Programmers Reference Kit (NMPKR)

Version 1.2								Date: Sept 21, 2012

Intel(r) Node Manager Programmers Reference Kit has been updated so that many of the issues with the previous version have been fixed. In general the NM functions have been greatly enhanced and the IPMI functions now work correctly with the SEL, SDR and FRU areas. See the release notes at the end of this document.

Note: DCMI and DNM functionality has been removed from the NMPRK. 

Intel(r) Node Manager is a server management technology that allows management software to accurately monitor and control the platform's power and thermal behaviors through an industry defined standard Intelligent Platform Management Interface (IPMI).  

Intel(r) Node Manager Technology allows the datacenter IT and Facilities managers to monitor actual server power and thermal behavior. Using this technology datacenter managers can set upper limits on the server power to maximize the rack density with confidence that rack power budget will not be exceeded. During a power or thermal emergency, Intel(r) Node Manager can automatically limit server power consumption and extend service uptime from standby power sources. These server's power and thermal information can also be used to improve overall datacenter efficiency and maximize overall datacenter utilization.

Intel(r) Node Manager Programmer's Reference Kit contains the source code (C and C++), test application, and documents to manage a server that support Intel Node Manager. 

The Kit simplifies the encoding/decoding of complex IPMI commands and responses, hides the protocol details and provides simple Application Programming Interfaces (APIs) to monitor and control power and thermal capabilities of Intel(r) Node Manager capable platforms. 

DOWNLOAD NOW: Intel(r) Node Manager Programmer's Reference Kit at:

https://github.com/01org/NMPRK

Please follow the links below for more information on Intel(r) Node Manager Technology:

Intel(r) Node Manager: 
http://www.intel.com/content/www/us/en/data-center/data-center-management/techrefresh-info-nodemanagerfull.html?wapkw=node+manager

Data Center Power Management with Intel(r) Node Manager:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

Intel(r) Node Manager Specification:
https://www-ssl.intel.com/content/www/us/en/data-center/data-center-management/node-manager-general.html?wapkw=intel%20node%20manager

NMPRK Testing:

The NMPRK has been tested on both Linux and Windows server platforms.  The two OS's that were used during test were Microsoft(r) Windows Server 2008 R2 and RedHat(r) Enterprise Linux 6.0.  The test programs used are included in the package.  The TestIpmi.exe program tests the IPMI interface to the library and the TestTranslation.exe program tests the translation layer interface to the library.

Defects may now be entered in the NMPRK repository located on GitHub:

https://github.com/01org/NMPRK/issues


Release Notes:
----------------------------------------------------------------------

The following defects have been fixed in this release:

The software was modified to return error codes that were previously being ignored.  Additionally now the error codes returned from IPMI communications are now being passed to the calling routine and may be interrogated so that the calling application may know exactly why the call failed and take appropriate action.  These error codes are defined in the IPMI specification.

The following Node Manager function calls were fixed:

setPolicy(): 
1.  Previously the enable/disable flag was not working, this has been fixed.
2. The aggressive flag has been added to support aggressive policy behavior.
3. Thermal policy trigger support has been fixed in this release.
4. setPolicy() has been fixed to allow the modifying of the current 
   policy (note: the policy must first be disabled in order to modify 
   it).

delPolicy() has been fixed to correctly delete a policy.

setPolicyStatus() has been fixed to allow the enabling and disabling of 
   policies.

clearSEL() has been fixed to correctly clear the SEL repository.

delSelRecord() has been fixed to correctly delete a SEL event from the 
   repository.

addSel() has been removed as there is no purpose for addSEL(). You cannot 
   restore the SEL repository as the SEL will modify the event.  There is 
   another function (not implemented) that allows you to add events to 
   the SEL.  That function may be added in a future release.

getSdrRecord() function has been fixed to correctly get SDR data.  

clearSdr() has been fixed to correctly clear the SDR repository

addSdrRecord() has been fixed to correctly add SDR data.

delSdrRecord() has been fixed to delete SDR data.

getFruData() has been fixed to correctly get FRU data.

getFruRpry() has been added to return the entire FRU from the repository

setFruData() has been fixed to correctly write FRU data back to the FRU.

getNMVersion() has been added to return the current NM Version (and IPMI 
   Version) of the targeted server.

Test Program Changes:

The testIpmi.exe test program has been modified to:

- return all the data in the SEL and store it into a file called 
   testfile.sel.  
- return all the data in the SDR repository and store it into a file 
   called testfile.sdr.  
- program the SDR repository using the file testfile.sdr which it 
   previously created.  
- return all the data in the FRU data area and store it in a file called 
   testfile.fru.  
- program the FRU data area using the file testfile.fru which it 
   previously created.

The program testTranslation.exe has been modified to create a policy, 
modify the policy, enable and disable the policy, and create a policy 
using a thermal trip.

Documentation Changes:

ImplemetationGuide-NMPRK.doc has been update to reflect the changes to the different function calls and structures supported by the NMPRK.


