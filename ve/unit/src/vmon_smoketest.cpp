
#include "gtest/gtest.h"
#include "vmon_socket_path.h"
#include "vmon_monitor.h"
#include "vmon_client.h"
#include "vmon_monitor_services.h"
#include <pthread.h>

static uint8_t prv_data[4096] = {0};

uint8_t vmon_monitor_read8(uint64_t addr) {
	if (addr < sizeof(prv_data)) {
		return prv_data[addr];
	} else {
		return 0xFF;
	}
}

void vmon_monitor_write8(uint64_t addr, uint8_t d) {
	fprintf(stdout, "Write 0x%04x\n", (int)addr);
	if (addr < sizeof(prv_data)) {
		prv_data[addr] = d;
	}
}

void vmon_monitor_exec(uint64_t addr) {
	fprintf(stdout, "EXEC: 0x%08llx\n", addr);
}

static void *monitor_main(void *ud) {
	vmon_monitor_t *mon = (vmon_monitor_t *)ud;
	fprintf(stdout, "--> monitor_main\n");
	vmon_monitor_run(mon);
	fprintf(stdout, "<-- monitor_main\n");
}

TEST(vmon,smoke1) {
	vmon_monitor_t mon;
	uint8_t data[16];
	vmon_socket_path *h2m_1 = new vmon_socket_path(true);
	vmon_socket_path *m2h_1 = new vmon_socket_path(false);
	vmon_socket_path *h2m_2 = new vmon_socket_path(true);
	vmon_socket_path *m2h_2 = new vmon_socket_path(false);
	vmon_client *client = new vmon_client();

	vmon_monitor_init(&mon);
	vmon_monitor_add_h2m_path(&mon, &vmon_socket_path::data, h2m_1);
	vmon_monitor_add_m2h_path(&mon, &vmon_socket_path::data, m2h_1);
	vmon_monitor_add_h2m_path(&mon, &vmon_socket_path::data, h2m_2);
	vmon_monitor_add_m2h_path(&mon, &vmon_socket_path::data, m2h_2);

	client->add_m2h_if(m2h_1);
	client->add_h2m_if(h2m_1);
	client->add_m2h_if(m2h_2);
	client->add_h2m_if(h2m_2);

	pthread_t t;
	pthread_create(&t, 0, &monitor_main, &mon);

	fprintf(stdout, "--> connect\n");
	ASSERT_TRUE(client->connect());
	fprintf(stdout, "<-- connect\n");

	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());

	ASSERT_TRUE(client->set_h2m_path(1));

	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());

	ASSERT_TRUE(client->set_m2h_path(1));

	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());

	for (int i=0; i<16; i++) {
		data[i] = i;
	}

	ASSERT_TRUE(client->write(0, data, 16));

	ASSERT_TRUE(client->read(0, data, 16));

	for (int i=0; i<16; i++) {
		fprintf(stdout, "data[%d] = 0x%02x\n", i, data[i]);
	}

	ASSERT_TRUE(client->exec(0x55EEAA00));
}
