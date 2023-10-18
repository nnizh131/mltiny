/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>


// #include "genNN.h"
// #include "tinyengine_function.h"

#include "ml_models/singlechannel/RandomForestRegressor().h"
// #include "nnom.h"

#include "pdda/df_algorithm.h"


// #include "ml_models/DecisionTreeRegressor().h"
// #include "ml_models/multichannel/LinearRegression().h"




static void start_scan(void);

static struct bt_conn *default_conn;


static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;


static int count = 0;
static float inputs[10];
static float multi_inputs[30] = {-1.06498225, -1.02169505, -0.88210107,  0.36731684, -0.34239239,
        0.43721337, -0.24015097, -0.44909086,  0.0101671 , -0.67371197,
       -0.9594799 , -0.69148155, -0.46643819,  0.12041559,  0.08175228,
        0.09414331, -0.08858204, -0.2395787 ,  0.37089802, -0.30053167,
       -0.90672872, -0.53291384, -0.4952998 ,  0.15463726, -0.11381661,
        0.10600844, -0.53721834, -0.38098864, -0.16580797,  0.08724365};
// nnom_model_t* model;

static uint8_t notify_func(struct bt_conn *conn,
			   struct bt_gatt_subscribe_params *params,
			   const void *data, uint16_t length)
{
	if (!data) {
		printk("[UNSUBSCRIBED]\n");
		params->value_handle = 0U;
		return BT_GATT_ITER_STOP;
	}

	printk("[NOTIFICATION] data %p length %u\n", data, length);
	// float a = 0.234;
	// printk("%f works\n", a);
	// printk("%f data\n", ((flaot *)data)[0]);
	for(int j = 0; j < length/sizeof(float); j++){
  		// printk("%f\n", ((float *) data)[j]);
		if(count < 10){
			inputs[count] = ((float *) data)[j];
		}
		count++;
	}

	if (count==0){
		count = 0;
		for(int i=0; i<10;i++){
			printk("%f\n",inputs[i]);
		}

		// uint32_t predic_label;
		// float prob;
		// int8_t* input;
		// model_run(model);
		// float p = (float)(nnom_output_data[0]) / 127.f;
		// nnom_predict(model, &predic_label, &prob);
		// printk("predicted - %f, true label - 45\n", p);
		// printk("predicted - %d, true label - 45\n", nnom_output_data[0]);
		// printk("%f\n", data);
		// int8_t* input;
		// float result = score(multi_inputs);
		float result = score(inputs);
		printk("predicted - %f, true label - 45\n", result);
	}   

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_func(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	int err;

	if (!attr) {
		printk("Discover complete\n");
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	printk("[ATTRIBUTE] handle %u\n", attr->handle);

	if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_HRS)) {
		memcpy(&uuid, BT_UUID_HRS_MEASUREMENT, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 1;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printk("Discover failed (err %d)\n", err);
		}
	} else if (!bt_uuid_cmp(discover_params.uuid,
				BT_UUID_HRS_MEASUREMENT)) {
		memcpy(&uuid, BT_UUID_GATT_CCC, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 2;
		discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
		subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printk("Discover failed (err %d)\n", err);
		}
	} else {
		subscribe_params.notify = notify_func;
		subscribe_params.value = BT_GATT_CCC_NOTIFY;
		subscribe_params.ccc_handle = attr->handle;

		err = bt_gatt_subscribe(conn, &subscribe_params);
		if (err && err != -EALREADY) {
			printk("Subscribe failed (err %d)\n", err);
		} else {
			printk("[SUBSCRIBED]\n");
		}

		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_STOP;
}

static bool eir_found(struct bt_data *data, void *user_data)
{
	bt_addr_le_t *addr = user_data;
	int i;

	printk("[AD]: %u data_len %u\n", data->type, data->data_len);

	switch (data->type) {
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
		if (data->data_len % sizeof(uint16_t) != 0U) {
			printk("AD malformed\n");
			return true;
		}

		for (i = 0; i < data->data_len; i += sizeof(uint16_t)) {
			struct bt_le_conn_param *param;
			struct bt_uuid *uuid;
			uint16_t u16;
			int err;

			memcpy(&u16, &data->data[i], sizeof(u16));
			uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));
			if (bt_uuid_cmp(uuid, BT_UUID_HRS)) {
				continue;
			}

			err = bt_le_scan_stop();
			if (err) {
				printk("Stop LE scan failed (err %d)\n", err);
				continue;
			}

			param = BT_LE_CONN_PARAM_DEFAULT;
			err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
						param, &default_conn);
			if (err) {
				printk("Create conn failed (err %d)\n", err);
				start_scan();
			}

			return false;
		}
	}

	return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	char dev[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(addr, dev, sizeof(dev));
	printk("[DEVICE]: %s, AD evt type %u, AD data len %u, RSSI %i\n",
	       dev, type, ad->len, rssi);

	/* We're only interested in connectable events */
	if (type == BT_GAP_ADV_TYPE_ADV_IND ||
	    type == BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
		bt_data_parse(ad, eir_found, (void *)addr);
	}
}

static void start_scan(void)
{
	int err;

	/* Use active scanning and disable duplicate filtering to handle any
	 * devices that might update their advertising data at runtime. */
	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};

	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}

	printk("Scanning successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t conn_err)
{
	char addr[BT_ADDR_LE_STR_LEN];
	int err;

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (conn_err) {
		printk("Failed to connect to %s (%u)\n", addr, conn_err);

		bt_conn_unref(default_conn);
		default_conn = NULL;

		start_scan();
		return;
	}

	printk("Connected: %s\n", addr);

	if (conn == default_conn) {
		memcpy(&uuid, BT_UUID_HRS, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.func = discover_func;
		discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
		discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
		discover_params.type = BT_GATT_DISCOVER_PRIMARY;

		err = bt_gatt_discover(default_conn, &discover_params);
		if (err) {
			printk("Discover failed(err %d)\n", err);
			return;
		}
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	if (default_conn != conn) {
		return;
	}

	bt_conn_unref(default_conn);
	default_conn = NULL;

	start_scan();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};




void main(void)
{
	int err;
	err = bt_enable(NULL);

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
	// int err_read = read_binary();
	// if (err_read<0){
	// 	printk("File System init failed (err %d)\n", err_read);
	// }

	printk("Bluetooth initialized\n");
	// model = nnom_model_create();

	// memcpy(nnom_input_data, input, sizeof(nnom_input_data));

	
	struct df_algorithm_config algoConf;
	struct nvs_config config;

	printk("pdda start"); 

	config.cteTime8us = DF_DEFAULT_CONFIG_C211_CTE_TIME_8US;
	config.nAnt = DF_DEFAULT_CONFIG_C211_NUM_ANTENNAS;
	config.nPhysicalAnt = config.nAnt;
	config.dElem = DF_DEFAULT_CONFIG_C211_ANTENNA_DISTANCE;
	config.antennaLayout = DF_DEFAULT_CONFIG_C211_ANTENNA_LAYOUT;
	strcpy(config.antennaName, DF_DEFAULT_CONFIG_C211_ANTENNA_NAME);
	const uint8_t swPattern[2 * DF_ALGORITHM_ANTENNAS_MAX] = DF_DEFAULT_CONFIG_C211_SW_PATTERN;
	memcpy(config.swPattern, swPattern, sizeof(swPattern));
	config.swLen = DF_DEFAULT_CONFIG_C211_SW_LEN;
	config.nPhiElemSubarrays = DF_DEFAULT_CONFIG_C211_PHI_EL_SUBARRAYS;
	config.nThetaElemSubarrays = DF_DEFAULT_CONFIG_C211_THETA_EL_SUBARRAYS;
	const uint8_t nPhiElem[DF_ALGORITHM_SUBARRAYS_MAX] = DF_DEFAULT_CONFIG_C211_NUM_PHI_EL;
	const uint8_t nThetaElem[DF_ALGORITHM_SUBARRAYS_MAX] = DF_DEFAULT_CONFIG_C211_NUM_THETA_EL;
	memcpy(config.nPhiElem, nPhiElem, sizeof(nPhiElem));
	memcpy(config.nThetaElem, nThetaElem, sizeof(nThetaElem));
	const uint8_t phiElem[DF_ALGORITHM_SUBARRAYS_MAX][DF_ALGORITHM_ANTENNAS_MAX] =
		DF_DEFAULT_CONFIG_C211_PHI_EL;
	const uint8_t thetaElem[DF_ALGORITHM_SUBARRAYS_MAX][DF_ALGORITHM_ANTENNAS_MAX] =
		DF_DEFAULT_CONFIG_C211_THETA_EL;
	memcpy(config.phiElem, phiElem, sizeof(phiElem));
	memcpy(config.thetaElem, thetaElem, sizeof(thetaElem));

	// config->antennaType = U_DF_PROD_ANT_CFG_C211;
	config.invertPhi = DF_DEFAULT_CONFIG_C211_INVERT_PHI;
	config.invertTheta = DF_DEFAULT_CONFIG_C211_INVERT_THETA;
    int i, j;

// 	/* load default configuration */
    df_algorithm_get_default_config(&algoConf);
	
//     uDfCfgFillDefault(U_DF_PROD_ANT_CFG_C211, &nvsConf);
  for (j = 0; j < config.nPhiElemSubarrays; j++) {
        algoConf.n_phi_el[j]     = config.nPhiElem[j];
        for (i = 0; i < config.nPhiElem[j]; i++) {
            algoConf.phi_el[j][i] = config.phiElem[j][i];
        }
    }

    for (j = 0; j < config.nThetaElemSubarrays; j++) {
        algoConf.n_theta_el[j]    = config.nThetaElem[j];
        for (i = 0; i < config.nThetaElem[j]; i++) {
            algoConf.theta_el[j][i] = config.thetaElem[j][i];
        }
    }
    algoConf.nbr_rep = (82 - 8) / (config.swLen * 2);
    algoConf.sw_len = config.swLen * algoConf.nbr_rep;
    algoConf.n_ant = config.nAnt;
    algoConf.d_elem = config.dElem;
	algoConf.antenna_type = DF_ALGORITHM_ANTENNA_ROW;
    algoConf.n_ant_physical = 3;

	if (0 != df_algorithm_setup(&algoConf)) {
        printk("df_algorithm_setup() failed");
    } else {
        printk("Setup alg. successfully!\n");
    }
	printk("sps %u\n", algoConf.sps);

	float32_t pPhiEst;
	float32_t pThetaEst;
	uint8_t channel = 39;

	int8_t iqBuffer[82 * 2] = {32, -33, -25, 40, 26, -37, -27, 38, 28, -37, -23, 40, 24, -40, -20, 43, 1, 13, 14, 11, 19, 23, -4, 10, 15, 11, 16, 19, -11, 48, 1, 50, 17, 46, -6, 50, 7, 49, 26, 42, 7, 7, 18, 2, 24, 12, 5, 6, 19, -1, 25, 4, 21, 44, 32, 43, 35, 31, 18, 43, 37, 35, 48, 17, 14, 1, 16, -9, 27, -5, 12, 2, 17, -10, 22, -7, 37, 30, 47, 16, 45, -2, 43, 24, 50, 13, 51, -5, 9, -5, 10, -16, 20, -19, 11, -4, 9, -18, 18, -19, 51, 1, 50, -13, 40, -26, 48, -5, 49, -13, 43, -30, 9, -8, -1, -21, 8, -25, 9, -12, -2, -19, 6, -27, 44, -28, 35, -35, 25, -42, 42, -30, 31, -39, 18, -46, 4, -12, -12, -16, -11, -25, -1, -10, -15, -15, -13, -24, 22, -45, 15, -48, -6, -51, 15, -46, 7, -52, -14, -49, -2, -10, -19, -10};
	// int8_t iqBuffer[10 * 1] = {32, -33, -25, 40, 26, -37, -27, 38, 28, -37};
	df_algorithm_docalc((cint8_t *)iqBuffer, channel, &pPhiEst, &pThetaEst);
	printk("Estimates: %f, %f\n", pPhiEst, pThetaEst);	

	start_scan();

}
