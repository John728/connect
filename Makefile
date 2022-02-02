unsw:
	gcc main.c unsw.c unsw.h login_info.h -o unsw

unsw_debug:
	gcc -g main.c unsw.c unsw.h login_info.h -o unsw_debug

unsw_setup:
	gcc setup.c unsw.h -o unsw_setup
	./unsw_setup
	sudo rm /opt/connect/unsw_setup
