#include <stdio.h>
#include <sput.h>

#include "cursor.c"


int main(int argc, char** argv){
	sput_start_testing();
	
	sput_enter_suite("cursor");
	sput_run_test(test_cursor);	
	
	sput_finish_testing();
	return sput_get_return_value();
}
