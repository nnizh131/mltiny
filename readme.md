# Installing Source Files and Dependencies
* Install [Zephyr and Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)

# Verify Installation
* Build sample program by running the following commands from the zephyr directory
* `cd zephyrproject/zephyr`
* `west build -b nrf52840dk_nrf52840 -d peripheral samples/bluetooth/peripheral_hr`

# Merge with this git repository
* `git init`
* `git remote add origin git@github.com:nnizh131/TinyML.git`
* `git fetch --all`
* `git reset --hard origin/master`

# Build the Binaries
Run the following commands from the zephyrproject/zephyr directory
* `west build -b nrf52840dk_nrf52840 -d peripheral samples/bluetooth/peripheral_hr`
* `cp peripheral/zephyr/zephyr.elf ./zephyr-ble-peripheral_hr.elf`

* `west build -b nrf52840dk_nrf52840 -d central samples/bluetooth/central_hr`
* `cp central/zephyr/zephyr.elf ./zephyr-ble-central_hr.elf`


# Install Renode
* Download and Install Renode [Renode](https://github.com/renode/renode)


# Run Simulation

ble.resc file describes the CPU platform that is used in the simulation as well as runs the Zephyr OS and binary .elf files generated from the code above.

* Open Renode application and run the folloowing script with the correct path to ble.resc file.
* `include @path/to/ble.resc` 
 
