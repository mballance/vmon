
#include "vmon_socket_path.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

vmon_socket_path::vmon_socket_path(bool h2m) : m_h2m(h2m) {
	// Always

	int srv_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr, cli_addr;

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;

	bind(srv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	socklen_t addrlen = sizeof(serv_addr);
	getsockname(srv_sock, (struct sockaddr *)&serv_addr, &addrlen);

	fprintf(stdout, "port: %d\n", serv_addr.sin_port);

	listen(srv_sock, 1);

	m_harness_fd = socket(AF_INET, SOCK_STREAM, 0);
	connect(m_harness_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	socklen_t clilen = sizeof(cli_addr);
	m_monitor_fd = accept(srv_sock, (struct sockaddr *)&cli_addr, &clilen);

	fprintf(stdout, "m_monitor_fd=%d\n", m_monitor_fd);
}

int32_t vmon_socket_path::recv(void *data, uint32_t sz, int32_t timeout) {
	// Called by the harness side of h2m to receive data
	if (!m_h2m) {
		// TODO: should use select for timeout
		int ret;
		do {
			ret = ::recv(m_harness_fd, data, sz, 0);
			fprintf(stdout, "client::recv ret=%d errno=%d\n", ret, errno);
		} while (ret < 0);
		return ret;
	} else {
		return -1;
	}
}

int32_t vmon_socket_path::send(void *data, uint32_t sz) {
	// Called by the harness size of m2h
	if (m_h2m) {
		fprintf(stdout, "harness::send\n");
		fflush(stdout);
		return ::send(m_monitor_fd, data, sz, 0);
	} else {
		fprintf(stdout, "skip send\n");
		fflush(stdout);
		return -1;
	}
}

int32_t vmon_socket_path::data(void *ud, uint8_t *data, uint32_t sz) {
	vmon_socket_path *p = static_cast<vmon_socket_path *>(ud);
	if (p->m_h2m) {
		// data(), on the harness-to-monitor path is used to receive data
		fprintf(stdout, "monitor::recv\n");
		fflush(stdout);
		return ::recv(p->m_harness_fd, data, sz, 0);
	} else {
		// data(), on the monitor-to-harness path is used to send data
		fprintf(stdout, "monitor::send %02x\n", data[0]);
		fflush(stdout);
		return ::send(p->m_monitor_fd, data, sz, 0);
	}
}
