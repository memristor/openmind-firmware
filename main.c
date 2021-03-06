/*   OpenMind open source eeg adc board firmware
 *   Copyright (C) 2011  Sebastian Götte <s@jaseg.de>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "main.h"
#include "ads1194.h"
#include "debug.h"
#include <util/delay.h>

int main(void){
	debug_init();
	ads_init_pass1();
	DDRD |= 0x1C; //LED outputs
	PORTD |= 0x10; //Set LED1
	_delay_ms(500); //Wait for the power supplies to settle (Should not need that much time)
	ads_init_pass2();
	_delay_ms(1000); //Wait for the ads to initialize...
	ads_init_pass3();
	_delay_ms(500); //Wait for the ads's internal reference to settle
	ads_init_pass4();
	PORTD &= 0xEF; //Unset LED1
	PORTD |= 0x08; //Set LED2
	debug_send_p(PSTR("OpenMind version dev0.2 started."));
	//The ads is ready for use!
	ads_start();
	ads_rdatac();
	while(1){
		if(ads_data_ready()){
			sample_data sample;
			ads_read(&sample);
			debug_begin();
			for(uint8_t i=0; i<4; i++){
				debug_send_hex_raw(sample.ch[i]>>8);
				_delay_us(400);
				debug_send_hex_raw(sample.ch[i]&0xFF);
				_delay_us(400);
				spi_send(' ');
				_delay_us(400);
			}
			debug_end();
		}
	}
}

void debug_send_ads_registers(){
	uint8_t val[0x1A];
	debug_send_p(PSTR("Register dump:"));
	debug_begin();
	ads_read_registers(0x00, 0x1A, val);
	for(uint8_t i=0; i<0x1A; i++){
		debug_send_hex_raw(val[i]);
		_delay_us(400);
		spi_send(' ');
		_delay_us(400);
	}
	debug_end();
}

