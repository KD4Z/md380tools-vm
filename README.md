MD380tools VM for VirtualBox
Warren Merkel, KD4Z
Current Version: 3.0

*** NEW Version 3.0 ***
10:00am EST 4 January, 2017

From github project
https://github.com/KD4Z/md380tools-vm

Follow the installation directions in the PDF found here:

https://github.com/KD4Z/md380tools-vm/raw/master/MD380tools_VM_installation_3.0.pdf

This project allows you to create the latest firmware for the MD-380 / MD-390
series of radios from Tytera (TYT).  It leverages the MD380tools project by
Travis Goodspeed found elsewhere on github.  It also downloads the dmr-marc contact
database into the radio.

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

Be sure to read it completely before emailing questions!  99% of all questions are
answered in the installation PDF.

You will need to download the VirtualBox Appliance image file from these links:

Single Image (Zip):
https://drive.google.com/file/d/0Bwoi2MrlPb3vVEt3Ym4tVHF3SDQ/view?usp=sharing
	
Use the above link, or the 4 links below.  Don't do both!
	
Parts - Must use 7-zip to extract  http://www.7-zip.org/download.html	
	
Part 1: https://drive.google.com/file/d/0Bwoi2MrlPb3vY2hkb0lVZ3Z4R3c/view?usp=sharing
Part 2: https://drive.google.com/file/d/0Bwoi2MrlPb3vMDlCMmY2V0FWY28/view?usp=sharing
Part 3:	https://drive.google.com/file/d/0Bwoi2MrlPb3vTFZfRGZ4X3NJakU/view?usp=sharing
Part 4: https://drive.google.com/file/d/0Bwoi2MrlPb3vSHkwdHV1dGVaZjA/view?usp=sharing

Download each one.  You will need to use the 7-zip program to extract them
back into the original OVA Appliance file. 
Install 7-Zip, and then Right-Click on the first part file and find the 7-Zip menu, and
follow it out to extract.

Current Version Numbers:
Image file updated:	 01/04/2017 10:00 EST to Version 3.0
Installation PDF updated: 12/29/2016 12:26 EST to Version 2.22

Noteable features:

  Runs under Windows or OS-X hosts.  Runs under VMware as well, reported by others.

  Able to update radios when not connected to the internet, great for laptops.

  Able to add private DMR IDs to the UsersCSV (Version 2.21)

  Automatically adds back missing userCSV data from the EU using seed file (Version 3.0)

  Automatically stays updated with changes going forward. You will not need to download
  the VM Image again, after getting to Version 3.0
  
  Simple, one word commands

  Able to update firmware on MD-380 and MD-390 radios with GPS

  





