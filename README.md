MD380tools VM for VirtualBox
Warren Merkel, KD4Z

This project is a fork of https://github.com/kd0kkv/md380tools-vm

You could freshly install any Linux distribution you like in VirtualBox, 
however I recently ran across the image made by KD0KKV, and thought it was useful.

I created this fork to enhance the image to support MD-390 radios with GPS and
provide more detailed instructions in its use.

Follow directions in the installation PDF.

You will need to download the VirtualBox Appliance image file from Dropbox.

Link:   https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.ova  

The file is also available split into four parts.
Here are the links for each piece:

https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part1.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part2.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part3.rar
https://dl.dropboxusercontent.com/u/24789865/VM/tyt_kd4z.part4.rar

Download each one.  You will need to use the RAR program to extract them
back into the original OVA Appliance file.

Image file updated:	 12/16/2016 22:30 EST
Current Version: 2.1

NOTE:  12/16/2016 22:30 EST

This build includes an extra command to workaround a current issue with the Makefile as retrieved from 
the github repository for md380tools.  I have opened a ticket in hope the error will be resolved.  In
the mean time, if you get an error when running the glv command, use glvfixup instead.  That command
does a fixup by copying back a prior version of the Makefile, that is known good.  At some point the
glv command should start working again when the developers working on the md380tools project fix the 
issue.   KD4Z




