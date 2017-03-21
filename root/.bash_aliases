alias glv="cd ~/ && echo -e '\033[1;32mUpdating KD4Z Toolkit' &&  rm -rf ~/md380tools-vm ~/md380tools && git clone https://github.com/KD4Z/md380tools-vm.git && echo -e '\033[1;32mUpdating md380tools' && git clone https://github.com/travisgoodspeed/md380tools.git && chmod +x ~/md380tools-vm/init && ~/md380tools-vm/init && ~/build"
alias flash="echo -e '\033[1;31mRadio should be connected in DFU mode now...\033[0m' && sleep 1 && cd ~/md380tools && ./md380-dfu upgrade ~/firmware-noGPS.bin && echo -e '\033[1;32mFirmware write completed.  Turn radio off then on now.\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
alias flashgps="echo -e '\033[1;31mRadio should be connected in DFU mode now...\033[0m' && sleep 1 && cd ~/md380tools && ./md380-dfu upgrade ~/firmware-GPS.bin && echo -e '\033[1;32mFirmware write completed.  Turn radio off then on now.\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
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
alias fl="flash"
alias fdb="flashdb"
alias dir="ls -lH"
alias reboot="/sbin/reboot"
lookup() { ~/lookup "$1"; }
save() { ~/save "$1"; }
restore() { ~/restore "$1"; }
rdtwrite() { ~/md380tools/md380-dfu write "$1"; }
rdtread() { ~/md380tools/md380-dfu read "$1"; }
rdtpassreset() { echo -e 'Resetting codeplug password' ;echo -e -n 'Reading codeplug...' ; ~/md380tools/md380-dfu read _my.img; echo -e -n 'Patching codeplug...' ; printf '\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF' | dd conv=notrunc of=_my.img bs=1 seek=$((0x20a0)); echo -e -n 'Writing codeplug to radio...' ; ~/md380tools/md380-dfu write _my.img; rm _my.img; echo -e ' Done. Rebooting radio now.' ; ~/md380tools/md380-dfu reboot; }
export RED='\033[1;31m'
export GREEN='\033[1;32m'
export BLUE='\033[1;34m'
export YELLOW='\033[1;33m'
export LGRAY='\033[1;37m'
export WHITE='\033[1;37m'
export BROWN='\033[1;33m'
export CYAN='\033[1;36m'
export NC='\033[0m'
