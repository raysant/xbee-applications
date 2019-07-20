/***** XBEE APPLICATION PROJECT *****
 * 
 * Auto-generated header with information about the 
 * relation between the XBee module pins and the 
 * project components.
 * 
 ************ XBEE LAYOUT ***********
 * 
 * This layout represents the XBee ZB (S2CTH) module 
 * selected for the project with its pin distribution:
 *               _________________
 *              /     ________    \ 
 *             /     |   __   |    \ 
 *            /      | //  \\ |     \ 
 *   XPIN1  -|       | \\__// |      |- XPIN20
 *   XPIN2  -|       |________|      |- XPIN19
 *   XPIN3  -|                       |- XPIN18
 *   XPIN4  -| ===================== |- XPIN17
 *   XPIN5  -| #   # ####  #### #### |- XPIN16
 *   XPIN6  -|  # #  #   # #    #    |- XPIN15
 *   XPIN7  -|   #   ####  ###  ###  |- XPIN14
 *   XPIN8  -|  # #  #   # #    #    |- XPIN13
 *   XPIN9  -| #   # ####  #### #### |- XPIN12
 *   XPIN10 -| ===================== |- XPIN11
 *           |_______________________|
 * 
 ************ PINS LEGEND ***********
 * 
 * The following list displays all the XBee Module pins 
 * with the project component which is using each one:
 * 
 *   XPIN1 = VCC
 *   XPIN2 = uart0 [TX Pin]
 *   XPIN3 = uart0 [RX Pin]
 *   XPIN4 = <<UNUSED>>
 *   XPIN5 = special0 [Reset Pin]
 *   XPIN6 = special0 [RSSI PWM Pin]
 *   XPIN7 = irq0 [IRQ Pin]
 *   XPIN8 = special0 [BKGD Pin]
 *   XPIN9 = <<UNUSED>>
 *   XPIN10 = GND
 *   XPIN11 = <<UNUSED>>
 *   XPIN12 = <<UNUSED>>
 *   XPIN13 = <<UNUSED>>
 *   XPIN14 = VCC REF
 *   XPIN15 = special0 [Association Pin]
 *   XPIN16 = <<UNUSED>>
 *   XPIN17 = <<UNUSED>>
 *   XPIN18 = <<UNUSED>>
 *   XPIN19 = <<UNUSED>>
 *   XPIN20 = special0 [Commissioning Pin]
 *
 ************************************/

#include <xbee_config.h>
#include <types.h>
#include <string.h>
#include <wpan/aps.h>
#include <xbee/transparent_serial.h>
#include <xbee/atcmd.h>

enum irq_flag {
	FLAG_CLEAR = 0, 
	FLAG_SET = 1
};

uint8_t joined_network = FLAG_SET;
volatile uint8_t tx_timer = FLAG_CLEAR;
volatile uint16_t rotation_count = 0;

#ifdef ENABLE_XBEE_HANDLE_MODEM_STATUS_FRAMES
int xbee_modem_status_handler(xbee_dev_t *xbee, const void FAR *payload,
                              uint16_t length, void FAR *context)
{
	/* Determine whether XBee has joined a network. */
	const xbee_frame_modem_status_t FAR *frame = payload;

	if (frame->status == XBEE_MODEM_STATUS_JOINED)
		joined_network = FLAG_SET;
	if (frame->status == XBEE_MODEM_STATUS_DISASSOC)
		joined_network = FLAG_CLEAR;
	
	return 0;
}
#endif

#ifdef timer0_irq 
void timer0_irq(void)
{
	tx_timer = FLAG_SET;
}
#endif

#ifdef irq0_irq
void irq0_irq(void)
{
	/* Increment counter when pulse from sensor is detected. */
	++rotation_count;
}
#endif

void main(void)
{
	wpan_envelope_t env;
	char tx_buffer[20];
	
	sys_hw_init();
	sys_xbee_init();
	sys_app_banner();

	/* Create an envelope for sending data to coordinator. */
	wpan_envelope_create(&env, &xdev.wpan_dev, WPAN_IEEE_ADDR_COORDINATOR,
											   WPAN_NET_ADDR_UNDEFINED);
	env.payload = tx_buffer;
	
	/* Configure timer interrupt for transmitting data every 10 seconds. */
	timer_config(timer0, TRUE, PERIODIC, 10000000);
	
	for (;;)
	{
		if (joined_network && tx_timer)
		{
			/* Construct the message and transmit it. The relationship between
			 * rotation count and amount of water passing through the sensor
			 * can be obtained through testing. */
			env.length = sprintf(tx_buffer, "W %d ", rotation_count);
			xbee_transparent_serial(&env);
			
			rotation_count = 0;
			tx_timer = FLAG_CLEAR;
		}
		
		sys_watchdog_reset();
		sys_xbee_tick();
	}
}
