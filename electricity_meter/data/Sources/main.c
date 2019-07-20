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
 *   XPIN7 = <<UNUSED>>
 *   XPIN8 = special0 [BKGD Pin]
 *   XPIN9 = <<UNUSED>>
 *   XPIN10 = GND
 *   XPIN11 = adc1 [ADC Pin]
 *   XPIN12 = <<UNUSED>>
 *   XPIN13 = <<UNUSED>>
 *   XPIN14 = VCC REF
 *   XPIN15 = special0 [Association Pin]
 *   XPIN16 = <<UNUSED>>
 *   XPIN17 = <<UNUSED>>
 *   XPIN18 = <<UNUSED>>
 *   XPIN19 = adc0 [ADC Pin]
 *   XPIN20 = special0 [Commissioning Pin]
 *
 ************************************/

#include <xbee_config.h>
#include <types.h>
#include <string.h>
#include <wpan/aps.h>
#include <xbee/transparent_serial.h>
#include <xbee/atcmd.h>

#define SAMPLE_ARRAY_SIZE 16

enum irq_flag {
	FLAG_CLEAR = 0, 
	FLAG_SET = 1
};

uint8_t joined_network = FLAG_SET;
volatile uint8_t adc_timer = FLAG_CLEAR;
volatile uint8_t tx_timer = FLAG_CLEAR;

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
	/* Timer interrupt flag for analog-to-digital converter.
	   ADC takes 16 samples per period of 60Hz AC electricity. */
	adc_timer = FLAG_SET;
}
#endif

#ifdef timer1_irq 
void timer1_irq(void)
{
	tx_timer = FLAG_SET;
}
#endif

/* Sum the instantaneous power for all the sample points taken in one period.
 * Offset must be calculated and applied beforehand since the average voltage
 * and current in one period must be zero. */
static void process_samples(int16_t *v_ptr, int16_t *c_ptr, int32_t *power)
{
	int32_t volt_avg;
	int32_t curr_avg;
	uint8_t i;

	volt_avg = 0;
	curr_avg = 0;
	for (i = 0; i < SAMPLE_ARRAY_SIZE; i++)
	{
		volt_avg += v_ptr[i];
		curr_avg += c_ptr[i];
	}
	/* Since the ADC gives unsigned 12-bit samples we can divide the total by
	 * 16 (# of samples) using right shifts instead of a divide instruction. */
	volt_avg >>= 4;
	curr_avg >>= 4;
	
	sys_watchdog_reset();
	
	for (i = 0; i < SAMPLE_ARRAY_SIZE; i++)
	{
		*power += (int32_t)((v_ptr[i] - volt_avg) * (c_ptr[i] - curr_avg));
	}
}

void main(void)
{
	int16_t volt_samples[SAMPLE_ARRAY_SIZE];
	int16_t curr_samples[SAMPLE_ARRAY_SIZE];
	uint8_t sample_index;
	int32_t power_total;
	wpan_envelope_t env;
	char tx_buffer[20];

	sys_hw_init();
	sys_xbee_init();
	sys_app_banner();
	
	/* Create an envelope for sending data to coordinator. */
	wpan_envelope_create(&env, &xdev.wpan_dev, WPAN_IEEE_ADDR_COORDINATOR,
											   WPAN_NET_ADDR_UNDEFINED);
	
	/* Configure timer interrupts for ADC (timer0) and transmitting (timer1). */
	timer_config(timer0, TRUE, PERIODIC, 1042);
	timer_config(timer1, TRUE, PERIODIC, 10000000);
	
	env.payload = tx_buffer;
	sample_index = 0;
	power_total = 0;
	
	for (;;)
	{
		if (adc_timer)
		{
			volt_samples[sample_index] = adc_ch_read(adc0);
			curr_samples[sample_index] = adc_ch_read(adc1);
			++sample_index;	
			
			if (sample_index > 15)
			{
				/* Stop after 16 samples and begin processing them. 1*/
				timer_irq_enable(timer0, FALSE);
				process_samples(volt_samples, curr_samples, &power_total);
				sample_index = 0;
			}
			
			adc_timer = FLAG_CLEAR;
		}
		
		if (joined_network && tx_timer)
		{
			/* Construct the message and transmit it. The value being sent is
			 * not properly scaled to actual Volts*Amps. Receiver can be connected
			 * to a Raspberry Pi, which can handle double precision floating
			 * point numbers and can utilize an FPU. */
			env.length = sprintf(tx_buffer, "E %ld ", power_total);
			xbee_transparent_serial(&env);
			
			power_total = 0;
			timer_irq_enable(timer0, TRUE);
			tx_timer = FLAG_CLEAR;
		}
		
		sys_watchdog_reset();
		sys_xbee_tick();
	}
}
