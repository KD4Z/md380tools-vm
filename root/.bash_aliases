alias glv="cd ~/ &&  rm -rf ~/md380tools-vm ~/md380tools && git clone https://github.com/KD4Z/md380tools-vm.git && git clone https://github.com/travisgoodspeed/md380tools.git && chmod +x ~/md380tools-vm/init && ~/md380tools-vm/init && ~/build"
alias flash="cd ~/md380tools && ./md380-dfu upgrade ~/firmware-noGPS.bin && echo -e '\033[1;32mFirmware write completed\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
alias flashgps="cd ~/md380tools && ./md380-dfu upgrade ~/firmware-GPS.bin && echo -e '\033[1;32mFirmware write completed\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
alias flashdb="~/flashdb"
alias glvusers="~/glvusers"
alias tweaks="~/tweaks"
alias menu="cd ~ && ~/md380tools-vm/menuopts"
alias recipes="cd ~ && ~/md380tools-vm/recipes"
alias recipe="recipes"
alias help="recipes"
alias g1v="glv"
alias GLV="glv"
alias FLASH="flash"
alias FLASHGPS="flashgps"
alias FLASHDB="flashdb"
alias dir="ls -lH"
lookup() { ~/lookup "$1"; }
save() { ~/save "$1"; }
restore() { ~/restore "$1"; }
rdtwrite() { ~/md380tools/md380-dfu write "$1"; }
rdtread() { ~/md380tools/md380-dfu read "$1"; }
