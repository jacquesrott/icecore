#include <stdio.h>
#include <sput.h>

#include "cursor.c"
#include "btree.c"


int main(int argc, char** argv){
	sput_start_testing();
	
	sput_enter_suite("cursor");
	sput_run_test(test_cursor);
	
	sput_enter_suite("btree");
	sput_run_test(test_btree);
	
	sput_finish_testing();
	return sput_get_return_value();
}