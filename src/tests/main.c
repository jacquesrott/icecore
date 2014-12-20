#include <stdio.h>
#include <sput.h>

#include "cursor.c"
#include "btree.c"
#include "filestore.c"
#include "array.c"


int main(int argc, char** argv){
	sput_start_testing();

	sput_enter_suite("cursor");
	sput_run_test(test_cursor);

	sput_enter_suite("btree");
	sput_run_test(test_btree);

	sput_enter_suite("filestore");
	sput_run_test(test_filestore);

	sput_enter_suite("array");
	sput_run_test(test_array);
	sput_run_test(test_big_array);

	sput_finish_testing();
	return sput_get_return_value();
}
