/** @file
 *  @brief HRS Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/zephyr.h>
#include <zephyr/init.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/sys/printk.h>

#define LOG_LEVEL CONFIG_BT_HRS_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(hrs);


#define GATT_PERM_READ_MASK     (BT_GATT_PERM_READ | \
				 BT_GATT_PERM_READ_ENCRYPT | \
				 BT_GATT_PERM_READ_AUTHEN)
#define GATT_PERM_WRITE_MASK    (BT_GATT_PERM_WRITE | \
				 BT_GATT_PERM_WRITE_ENCRYPT | \
				 BT_GATT_PERM_WRITE_AUTHEN)

#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN
#define CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN 0
#endif
#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT
#define CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT 0
#endif
#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW
#define CONFIG_BT_HRS_DEFAULT_PERM_RW 0
#endif

#define HRS_GATT_PERM_DEFAULT (						\
	CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN ?				\
	(BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN) :	\
	CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT ?				\
	(BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT) :	\
	(BT_GATT_PERM_READ | BT_GATT_PERM_WRITE))			\

static uint8_t hrs_blsc;

static void hrmc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("HRS notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &hrs_blsc,
				 sizeof(hrs_blsc));
}

/* Heart Rate Service Declaration */
BT_GATT_SERVICE_DEFINE(hrs_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_HRS),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
	BT_GATT_CCC(hrmc_ccc_cfg_changed,
		    HRS_GATT_PERM_DEFAULT),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_BODY_SENSOR, BT_GATT_CHRC_READ,
			       HRS_GATT_PERM_DEFAULT & GATT_PERM_READ_MASK,
			       read_blsc, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_CONTROL_POINT, BT_GATT_CHRC_WRITE,
			       HRS_GATT_PERM_DEFAULT & GATT_PERM_WRITE_MASK,
			       NULL, NULL, NULL),
);

static int hrs_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	hrs_blsc = 0x01;

	return 0;
}

int bt_hrs_notify(int sample_id)
{
	printk("%d", sample_id);
	int rc=0;
	static float samples[20][10] = {
	   {-0.01090561,  0.07280737,  0.08699954, -0.00734589,  0.09090522,
        -0.02118376,  0.02404321, -0.07267798,  0.01810839, -0.0679553 },
       {-0.0418057 ,  0.06051836,  0.04975531, -0.01097248,  0.03168421,
        -0.01171548,  0.00695419, -0.060789  , -0.06000866, -0.0138873 },
       { 0.02982634,  0.10337642,  0.08364723, -0.03570775,  0.06068315,
        -0.08249636,  0.07270218,  0.0893643 ,  0.10049078,  0.09984395},
       {-0.0839422 ,  0.03178036,  0.0213625 ,  0.01121671,  0.0011124 ,
         0.03807082,  0.05532652,  0.00055359,  0.03360505, -0.01618103},
       { 0.17168586,  0.38154139, -0.0976538 , -0.45064527, -0.32852964,
        -0.22653772,  0.33366448, -0.01972932,  0.10795526,  0.09821179},
       { 0.03123088,  0.13035825,  0.11910057, -0.0112244 ,  0.10110337,
        -0.02294015,  0.11531545, -0.07009374, -0.01285277, -0.1131253 },
       {-0.12467414,  0.02612701,  0.03185468, -0.00298492,  0.03005768,
        -0.00474076,  0.0143772 , -0.03055945, -0.02165639, -0.02830346},
       { 0.06213098,  0.18852808,  0.16289099, -0.05862501,  0.12915777,
        -0.10648259,  0.12280703,  0.03620361, -0.04736189,  0.101826  },
       { 0.08179468,  0.08822283,  0.21113963, -0.02236503,  0.22487111,
         0.03046851,  0.08080833, -0.07536104, -0.01322553, -0.08449135},
       { 0.05510823,  0.129597  ,  0.13187551, -0.03644765,  0.12621473,
        -0.08562354, -0.04710176,  0.17121658, -0.15404284,  0.06494224},
       {-0.09798769,  0.01404765, -0.0056021 ,  0.01573472, -0.02522795,
         0.03769311, -0.00127867, -0.04812321, -0.06122367,  0.01702515},
       {-0.0530421 ,  0.0520597 ,  0.05225997, -0.00039382,  0.06937358,
        -0.0239262 ,  0.01641575, -0.00481751,  0.02373038,  0.05257386},
       {-0.00950106,  0.07039248,  0.06862365,  0.0354652 ,  0.03334978,
         0.04877984,  0.04838616, -0.07503758, -0.03589142, -0.07417994},
       {-0.13731509,  0.02721209,  0.03122195, -0.00195312,  0.03387026,
        -0.00306532, -0.00280424, -0.01568065, -0.01952944,  0.01219217},
       { 0.06494008,  0.18348953,  0.13428167, -0.07567907,  0.07022127,
        -0.16789851,  0.11075099, -0.04365782,  0.12286001, -0.08945598},
       { 0.00173534,  0.07736467,  0.08623712,  0.02556002,  0.08682819,
         0.03932437,  0.03606694, -0.06953012, -0.04600795, -0.06537221},
       { 0.07477193,  0.11028414,  0.14175315, -0.01247142, -0.02939842,
         0.000374  ,  0.15518786,  0.2000251 ,  0.17919532,  0.18445614},
       { 0.09022198,  0.22171347,  0.1847893 , -0.10049253,  0.14437417,
        -0.16747435,  0.14508701,  0.06270191, -0.00979695,  0.04988106},
       { 0.13376302,  0.24736297,  0.15913228, -0.04028518, -0.05773931,
        -0.11193732,  0.11332017, -0.26476297, -0.12837306, -0.29564342},
       { 0.02420814,  0.0905442 ,  0.09485654, -0.02417496,  0.10999682,
        -0.05766527,  0.08185907, -0.04633042,  0.03809121, -0.06789833}};
	static float sample1[5];
	static float sample2[5];
	for(int i=0;i<5; i++){
		sample1[i] = samples[sample_id][i];
	}
	for(int i=5;i<10; i++){
		sample2[i-5] = samples[sample_id][i];
	}
	// = samples[sample_id];
	if(bt_gatt_notify(NULL, &hrs_svc.attrs[1], &sample1, sizeof(sample1)) == 0){
		rc = bt_gatt_notify(NULL, &hrs_svc.attrs[1], &sample2, sizeof(sample2));
	}
	return rc == -ENOTCONN ? 0 : rc;
}

SYS_INIT(hrs_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
