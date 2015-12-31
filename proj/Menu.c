#include "Menu.h"



void subscribe(){
	IRQ_SET_KBD =  keyboard_subscribe_int();
	IRQ_SET_TIMER =timer_subscribe_int();
	IRQ_SET_MOUSE =  mouse_subscribe_int();
}

void unsubscribe(){
	if(keyboard_unsubscribe_int() != 0){
		vg_exit();
		return ;
	}

	if(timer_unsubscribe_int() != 0){
		vg_exit();
		return ;
	}

	while_out_buf_full();
	if(mouse_unsubscribe_int() != 0){
		vg_exit();
		return ;
	}
}