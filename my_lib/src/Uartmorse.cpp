/*
 * Uartmose.cpp
 *
 *  Created on: 31. jan. 2020
 *      Author: Rasmus
 */

#include <Uartmorse.h>

Uart_morse::Uart_morse(morse_encoder& mo0, LpcUart& uartr0) : mo(mo0), uartr(uartr0){
	s = sa;

}

void Uart_morse::read_uart(){
	char tmp[20];
	int out;
	while(1){
		s = sa; //reset array
		s = gets();
		if (!strncmp(s, "wpm ", 4)){
			int wpm;
			s += 4*sizeof(char);
			if (sscanf(s, "%d", &wpm) == EOF)
				uartr.write("nan\r\n");
			else{
				mo.set_wpm(wpm);
				uartr.write("wpm set to ");
				sprintf(tmp, "%d", wpm);
				uartr.write(tmp);
				uartr.write("\r\n");

			}
		} else if (!strncmp(s, "send ", 5)){
			s += 5*sizeof(char);
			mo.send(s);
		} else if (!strcmp(s, "set")){
			out = mo.get_period();
			sprintf(tmp, "%d", out);
			uartr.write("Time unit: ");
			uartr.write(tmp);
			uartr.write("\r\n");

			out = mo.get_wpm();
			sprintf(tmp, "%d", out);
			uartr.write("wpm: ");
			uartr.write(tmp);
			uartr.write("\r\n");
		} else {
			uartr.write("Unrecognised input.\r\n");
		}
	}
}

char* Uart_morse::gets(){
	int i = 0;
	char c;
	int read = 1;
	bool eol = false;

	do {
		if ((read = uartr.read(c)) != 0 && c != '\r'){
			s[i] = c;
			++i;
			uartr.write(c);
		} else if (read != 0 && c == '\r') eol = true;
	} while(!eol && i < (BUFLEN - 1));

	uartr.write('\r');
	uartr.write('\n');
	s[i] = '\0';

	return s;
}

Uart_morse::~Uart_morse() {
	// TODO Auto-generated destructor stub
}

