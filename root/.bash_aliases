alias glv="cd ~/ &&  rm -rf ~/md380tools-vm && git clone https://github.com/KD4Z/md380tools-vm.git && rm -rf ~/md380tools && git clone https://github.com/travisgoodspeed/md380tools.git && chmod +x ~/md380tools-vm/init && ~/md380tools-vm/init &&  ~/md380tools-vm/exec.pre && cd ~/md380tools && make dist &&
 cp dist/md380tools-`date "+%Y-%m-%d"`/firmware-`date "+%Y-%m-%d-NoGPS"`.bin ~/firmware-noGPS.bin &&
 cp dist/md380tools-`date "+%Y-%m-%d"`/firmware-`date "+%Y-%m-%d-GPS"`.bin ~/firmware-GPS.bin &&
 make updatedb && wc -c < db/stripped.csv > user.bin && cat db/stripped.csv >> user.bin &&
 cp user.bin ~/user.bin && cd ~/md380tools-vm && ~/md380tools-vm/exec.post && cd ~"
alias flash="cd ~/md380tools && ./md380-dfu upgrade ~/firmware-noGPS.bin && echo -e '\033[1;32mFirmware flash completed\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
alias flashgps="cd ~/md380tools && ./md380-dfu upgrade ~/firmware-GPS.bin && echo -e '\033[1;32mFirmware flash completed\033[0m' && sleep 5 && cd ~ && ~/md380tools-vm/menuopts"
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

