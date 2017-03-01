MD380tools VM for VirtualBox (aka KD4Z Toolkit)
Warren Merkel, KD4Z

Current Version: 3.11 (after glv is run)
Image file updated:	 01/04/2017  Version 3.0
Installation PDF updated: 01/28/2017

From github project
https://github.com/KD4Z/md380tools-vm

Follow the installation directions in the PDF found here:

https://github.com/KD4Z/md380tools-vm/raw/master/MD380tools_VM_installation_3.08.pdf

You only need to download a couple of files from this github site. 
1. The Installlation instructions pdf 
2. The Image File for the Virtual Machine.

These instructions describe how to install software on your Windows or Mac, that runs linux in 
a Virtual Machine environment.  You don't need to know anything about linux to use it!

If you are linux savy, and would rather just use the scripting framework on your existing Linux box, see
the linux install pdf document for brief installation instructions that allow you to use your existing linux
environment such as Debian, Ubuntu or Raspian on a Raspberry Pi.  This guide will discribe how to install exactly
the same scripting framework used in the VM.   Once installed, it will self update in the same way the VM does. 

See MD380tools_Menu_installation_on_native_linux_KD4Z.pdf here:  
https://github.com/KD4Z/md380tools-vm/raw/master/MD380tools_Menu_installation_on_native_linux_KD4Z.pdf
If you run into something I missed in this brief document, please forward that information on to me
so I can update the document.  

Facebook Support group, specializing in Dutch and English, courtesy of Rob van Rheenen, PD0DIB.
https://www.facebook.com/groups/KD4ZToolkit/
 

What does this thing do?

The VM allows you to create the latest firmware for the MD-380 / MD-390
series of radios from Tytera (TYT) and Retevis RT3/RT8.  It leverages the MD380tools project by
Travis Goodspeed found elsewhere on github.  It also downloads the whole dmr-marc contact
database into the radio.  

The VM provided provides simple single-word menu that is easy to
use, and does all of the heavy lifting of installing the MD380tools project files.
 
Other interesting tweaks in this VM make it stand out as producing better results than just
using other scripting or VM solutions.

* Automatically updates itself to the latest scripting and source code.  You will always, ALWAYS have the absolute latest features.
* Single, one word commands to do the primary functions.
* Builds the "firmware" for radios having GPS and not having GPS.  No need to download firmware from websites.
* Builds the contact list on-the-fly, directly from the dmr-marc database.  Currently over 55K records worldwide.
* After running one command, the computer with the VM can be offline from Internet, and still write
firmware and contact data to the radio.
* Larger font on contact name with redline data separator.
* More detail in EU contact data.
* Support for private DMR IDs pulled in with the full dmr-marc contact database.
* Scripting framework functions can be extended for custom actions before firmware compilation,
after compilation, and after contact data is downloaded.
* Commands to save and restore firmware and contact data in snapshots.  Restores to a stable firmware if the latest gets sideways.
* Screen language defaults to English.  Five alternate languages available: Dutch, Portuguese, Spanish, Italian and German.
* Lite weight version for Linux!  The scripting is all done using Bash scripts.  You can install just the scripting on Linux or Raspberry Pi.

Be sure to read the installation completely before emailing questions!  99% of all questions are
answered in the installation PDF.  If you are stuck, join the Facebook group mentioned above for help.

You have to make a choice.  You can use Oracle Virtual Box or VMware player.  Choose one, and select the correct image download below. 

Get to the downloads:

* For the VirtualBox Appliance image file, use these links

Single file download:
https://drive.google.com/file/d/0Bwoi2MrlPb3vVEt3Ym4tVHF3SDQ/view?usp=sharing

or 

Mirror 1:
https://www.dropbox.com/sh/ap0g1o9g48wyide/AAA1dXOHPWOMD3vbzXO-iBaIa/tyt_kd4z_3.0.ova
	
Use one of the above links, or download all 4 links below.  Don't do both!
	
Parts - Use these links if you have trouble downloading the large file above.
	
Part 1: https://drive.google.com/file/d/0Bwoi2MrlPb3vY2hkb0lVZ3Z4R3c
Part 2: https://drive.google.com/file/d/0Bwoi2MrlPb3vMDlCMmY2V0FWY28
Part 3:	https://drive.google.com/file/d/0Bwoi2MrlPb3vTFZfRGZ4X3NJakU
Part 4: https://drive.google.com/file/d/0Bwoi2MrlPb3vSHkwdHV1dGVaZjA

For the part files, download each one.  You will need to use the 7-zip program to extract them
back into the actual OVA Appliance file.  You can get that from here -- http://www.7-zip.org/download.html 
 
Install 7-Zip, and then Right-Click on the first part file, find the 7-Zip menu, and
choose extract.

* For VMware, use these links:

Single file download:

https://drive.google.com/open?id=0BwED_-BHnZjSWklsNjY3VHBqMVE

-OR Download the VMWare image in parts - don't download these if you download from the single link above.

Parts - Use these links if you have trouble downloading the large file above.

Part 1:	https://drive.google.com/file/d/0BwED_-BHnZjSdERwY1BHc1Y3ekk
Part 2: https://drive.google.com/open?id=0BwED_-BHnZjSYnZEeXJHcjh5R3c
Part 3: https://drive.google.com/open?id=0BwED_-BHnZjSb3NzSE41azhzN3M
Part 4: https://drive.google.com/open?id=0BwED_-BHnZjSNmlOMTJvT3l4VWM


* Multiple language support.   Currently 5 alternate languages are supported.
  
  Dutch			(enable with:  touch lang.nl )  Courtesy of PD0DIB
  
  Portuguese	(enable with:  touch lang.pt )  Courtesy of CS7AGH

  Italian		(enable with:  touch lang.it )  Courtesy of IW0QPP

  Spanish       (enable with : touch lang.es )  Courtesy of EA8EE
   
  German        (enable with : touch lang.de )  Courtesy of OE7BSH
  
  If you can help translate into other languages, create a branch, and modify the
  "strings" script (found here), and request a pull.  Or send me the new language section in email.  

  Able to display user's first name in the nickname space, which makes the name easy to see.
  
  User customization ability using your own bash scripting.  Callouts to user scripts are available
  prior to glv, during filtering, and after glv is completed.  Knock yourself out!
  
  KD4Z