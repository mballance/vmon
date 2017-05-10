
#include "gtest/gtest.h"
#include "vmon_socket_path.h"
#include "vmon_monitor.h"
#include "vmon_client.h"
#include <pthread.h>

static void *monitor_main(void *ud) {
	vmon_monitor_t *mon = (vmon_monitor_t *)ud;
	fprintf(stdout, "--> monitor_main\n");
	vmon_monitor_run(mon);
	fprintf(stdout, "<-- monitor_main\n");
}

TEST(vmon,smoke1) {
	vmon_monitor_t mon;
	vmon_socket_path *h2m = new vmon_socket_path(true);
	vmon_socket_path *m2h = new vmon_socket_path(false);
	vmon_client *client = new vmon_client();

	vmon_monitor_init(&mon);
	vmon_monitor_add_h2m_path(&mon, &vmon_socket_path::data, h2m);
	vmon_monitor_add_m2h_path(&mon, &vmon_socket_path::data, m2h);

	client->add_m2h_if(m2h);
	client->add_h2m_if(h2m);

	pthread_t t;
	pthread_create(&t, 0, &monitor_main, &mon);

	fprintf(stdout, "--> connect\n");
	ASSERT_TRUE(client->connect());
	fprintf(stdout, "<-- connect\n");

	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());
	ASSERT_TRUE(client->ping());

	ASSERT_TRUE(client->exec(0x55EEAA00));
}
