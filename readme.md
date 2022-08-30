# code to genereate peripheral_hr elf file
west build -b nrf52840dk_nrf52840 -d peripheral samples/bluetooth/peripheral_hr
cp peripheral_hr/zephyr/zephyr.elf ./zephyr-ble-peripheral_hr.elf

# code to genereate central node / anchor elf file
west build -b nrf52840dk_nrf52840 -d central_hr samples/bluetooth/central_hr

cp central_hr/zephyr/zephyr.elf ./zephyr-ble-central_hr.elf


include @C:\Users\nini\Desktop\Renode\m4.resc
