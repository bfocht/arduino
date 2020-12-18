unsigned char _address = 0x00;
void setup()
{
  TCCR0A = (0x01<<COM0A0) | (0x01<<WGM01); // CTC mode, toglle OC0A on compare match
  TCCR0B = (0x01<<CS00); // cpu clock, no prescaler
  OCR0A = 209; // Toggle/clear at 209 to give 38khz at 16mhz clock speed

  pinMode(6, OUTPUT); //connect led here
  pinMode(7, OUTPUT);
  Serial.begin(9600,SERIAL_8N1);
}

void loop()
{
  ir_off();

  while(Serial.available() > 0) {
    char character = Serial.read();
    set_color(character);
  }
}


void set_color(char character) {
  switch(character) {
    case 's':
    case '1':
      send_command(0xC0); // on
      return;
    case '0':
      send_command(0x40); // off
      return;

    //colors
    case 'g':
      send_command(0x20); // red
      return;
    case 'r':
      send_command(0xA0);  //green
      return;
    case 'b':
     send_command(0x60); //blue
      return;
    case 'w':
     send_command(0xE0); //rgb
      return;
    case 'y':
     send_command(0x08); //yellow
     return;

    //action
    case '2':
      send_command(0xC8);  //fade
      return;
    case '3':
      send_command(0xD0);  //flash
      return;
    case 'f':
    case 'p':
    case '4':
      send_command(0xE8);  //smooth
      return;
    case '5':
      send_command(0xF0);  //strobe
      return;
    case '[':
      send_command(0x80);  //dim
      return;
    case ']':
      send_command(0x00);  //brighten
      return;    

    //cycle through addresses
    case ',':
      _address = _address - 8;
      send_command(_address);
      return;
    case '.':
      _address = _address + 8;
      send_command(_address);
      return;
    case '/':
      send_command(_address); // repeat last address
      return;   
  }
}

void send_command(unsigned char data)
{
  _address = data;
  Serial.println(data);

  command_init();

  unsigned char address = 0x00;

  send_ir_byte(address);
  send_ir_byte(address ^ 0xff);

  send_ir_byte(data);
  send_ir_byte(data ^ 0xff);

  send_bit_low(); // stop-bit
}

void send_ir_byte(unsigned char data)
{
  unsigned char i;

  for (i=0;i<8;i++) // MSB first
  {
    if ((data<<i) & 0x80)
    {
      send_bit_high();
    } else
    {
      send_bit_low();
    }
  }
}

void send_bit_high()
{
  // 1.65ms
  ir_on();
  delayMicroseconds(560);
  ir_off();
  delayMicroseconds(1650);
}

void send_bit_low()
{
  // 0.55ms
  ir_on();
  delayMicroseconds(560);
  ir_off();
  delayMicroseconds(560);
}

void command_init()
{
  ir_on();
  delayMicroseconds(9000);
  ir_off();
  delayMicroseconds(4500);
}

void ir_on()
{
  PORTD |= 0x80; // debug output whithout carrier frequency

  TIFR0 = (0x01<<TOV0);
  TCNT0 = 0;
  TCCR0A = (0x01<<COM0A0) | (0x01<<WGM01);
  TCCR0B = (0x01<<CS00); // start the timer, no prescaler
}

void ir_off()
{
  PORTD &= 0b01111111; // debug output whithout carrier frequency

  TCCR0B = 0x00; // stop the timer
  TCCR0A = (0x01<<WGM01);
  PORTD &= 0b10111111;
}
