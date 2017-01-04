MD380tools VM for VirtualBox
Warren Merkel, KD4Z
Current Version: 2.22

*** UPDATE COMING OUT VERY SOON ***  Version 3.0
12:00am EST 4 January, 2017
You might want to hold off a day or two, as Version 3.0 is coming out within 48 hours.
I'll leave the links up for version 2.22 just in case you have to have it now.


From github project
https://github.com/KD4Z/md380tools-vm


This project allows you to create the latest firmware for the MD-380 / MD-390
series of radios from Tytera (TYT).  It leverages the MD380tools project by
Travis Goodspeed found elsewhere on github. 


This VM provided in the image provides simple single-word menu that is easy to
use, and does all of the heavy lifting of installing the MD380tools project files.
 
The most important benefit of using this VM instead of other solutions is you can 
update radio firmware and user contact data while offline from the Internet.  Run
one single command (glv) while online, and then you can disconnect your laptop, 
and take it where you dont' have a live internet connection.  

If you are interested in adding in private DMR IDs to the user contact data, support
for that has been recently added.  This feature may also be interesting to you if
use a hotspot device and are annoyed that certain parts of the contact database 
now show up with blank, redacted location data.

Follow the installation directions in the PDF found here:
https://github.com/KD4Z/md380tools-vm/raw/master/MD380tools_VM_installation_2.22.pdf


Be sure to read it completely before emailing questions!  99% of all questions are
answered in the installation PDF.

You will need to download the VirtualBox Appliance image file from these links:

Link:   https://drive.google.com/file/d/0Bwoi2MrlPb3vSzlxeXFrTEVuWEU/view?usp=sharing  803 MB

The file is also available split into four parts.  You dont' need these if you downloaded 
from the single link above.

Here are the links for each piece:

https://drive.google.com/file/d/0Bwoi2MrlPb3vMzN3bWxfZUI1MTQ/view?usp=sharing  205 MB
https://drive.google.com/file/d/0Bwoi2MrlPb3vOC15N18xcHF1WjQ/view?usp=sharing  205 MB
https://drive.google.com/file/d/0Bwoi2MrlPb3vbEJWTEtCSHh1b3c/view?usp=sharing  205 MB
https://drive.google.com/file/d/0Bwoi2MrlPb3vRnBORmNCQndQMUE/view?usp=sharing  200 MB


Download each one.  You will need to use the RAR program to extract them
back into the original OVA Appliance file.  (http://www.win-rar.com)
Install RAR, and then Right-Click on the first part and choose the RAR item on
the context menu to extract.

Current Version Numbers:
Image file updated:	 12/16/2016 22:30 EST to Version 2.2
Installation PDF updated: 12/29/2016 12:26 EST to Version 2.22

Noteable features:

  Runs under Windows or OS-X hosts.  Runs under VMware as well, reported by others.

  Simple, one word commands

  Able to update firmware on MD-380 and MD-390 radios with GPS

  Able to update radios when not connected to the internet

  Able to add private DMR IDs to the UsersCSV (Version 2.21)

  Able to add back missing userCSV data using seed file (Version 2.22)
   To install, see the section on Installing support for Static DMR IDs
   in the PDF document.   







