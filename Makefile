unsw:
	gcc main.c unsw.c unsw.h login_info.h -o unsw

unsw_debug:
	gcc -g main.c unsw.c unsw.h login_info.h -o unsw_debug

setup:
	gcc setup.c unsw.h -o setup
