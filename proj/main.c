#include <minix/drivers.h>
#include <stdlib.h>
#include "copter.h"
#include "vbe.h"
#include "keyboard.h"
#include "timer.h"
#include "i8254.h"
#include "KBD.h"
#include "lmlib.h"
#include "video_gr.h"
#include "GameState.h"
#include "Margins.h"

int main() {

	/* Initialize service */

	sef_startup();


	int ipc_status;
	unsigned long irq_set_kbd =  keyboard_subscribe_int();
	int irq_set_timer=timer_subscribe_int();
	message msg;
	int r,scancode=0,over=1;
	int fps=30,counter=0,interruptions;
	int spacePress=0;

	Copter* c=newCopter(200,400,40,10);
	Margin* m1=newMargin(0,0,800,100);
	Margin* m2=newMargin(0,500,800,100);
	Margin **margins;
	margins=(Margin **) malloc(2*sizeof(Margin *));
	margins[0]=m1;
	margins[1]=m2;

	//vg_init(MODE_105);
	vg_init(MODE_103);

	while( over ) { /* You may want to use a different condition */
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			/* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					scancode=keyboard_c_handler();
					if(scancode==MAKE_SPACE){
						spacePress=1;
					}
					else {
						spacePress=0;
					}
					if(scancode==BREAK_ESC){
						over=0;
					}
				}
				if(msg.NOTIFY_ARG & irq_set_timer){
					counter++;
					interruptions=counter%(60/fps);
					if(interruptions==0){
						if(spacePress==0)
							update_copter(c,1);
						else
							update_copter(c,0);


						updateGame(c,margins);
					}

				}
				break;

			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if(keyboard_unsubscribe_int() != OK){
		vg_exit();
		return 1;
	}

	if(timer_unsubscribe_int()==1){
		vg_exit();
		return 1;
	}

	free(c);
	int i;
	for(i=0;i<2;i++){
		free(margins[i]);
	}
	free(margins);//free(m1);free(m2);
	vg_exit(); //the function will go to text mode and to thw wrong terminal, then change to terminal ( alt + f1 )

	return 0;
}