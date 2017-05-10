
#include "gtest/gtest.h"
#include <stdint.h>
#include <string>
#include <stdarg.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int status;

	testing::InitGoogleTest(&argc, argv);

	status = RUN_ALL_TESTS();

	return 0;
}
