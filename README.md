MD380tools VM for VirtualBox
Warren Merkel, KD4Z
Current Version: 2.22

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
https://github.com/KD4Z/md380tools-vm

Be sure to read it completely before emailing questions.  99% of all questions are
answered in the installation PDF.

You will need to download the VirtualBox Appliance image file from Dropbox.

Link:   https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.ova  

The file is also available split into four parts.
Here are the links for each piece:

https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part1.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part2.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part3.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part4.rar

Download each one.  You will need to use the RAR program to extract them
back into the original OVA Appliance file.  (http://www.win-rar.com)

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







