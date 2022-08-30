# Installing Source Files and Dependencies
* Install [Zephyr and Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)

# Verify Installation
* Build sample program by running the following commands from the zephyr directory
* `cd zephyrproject/zephyr`
* `west build -b nrf52840dk_nrf52840 -d peripheral samples/bluetooth/peripheral_hr`


# Merge with this git repository
* `git init`
* `git add remote origin git@github.com:nnizh131/TinyML.git'
* `git fetch --all`
* `git reset --hard origin/master`

# Build the Binaries
* `west build -b nrf52840dk_nrf52840 -d peripheral samples/bluetooth/peripheral_hr`
* 'cp peripheral/zephyr/zephyr.elf ./zephyr-ble-peripheral_hr.elf'

* `west build -b nrf52840dk_nrf52840 -d central samples/bluetooth/central_hr`
* `cp central_hr/zephyr/zephyr.elf ./zephyr-ble-central_hr.elf`




include @C:\Users\nini\Desktop\Renode\m4.resc
cd zephyrproject/zephyr
