#include "keyboard.h"
#include "ps2dev.h"

const uint LED_PIN = 25;

const uint PS2_CLOCK_PIN = 26;
const uint PS2_DATA_PIN = 27;

unsigned char leds;

PS2dev ps2(PS2_CLOCK_PIN, PS2_DATA_PIN);
Keyboard keyboard(ps2);

void setup()
{
    ps2.keyboard_init();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void loop()
{
    ps2.keyboard_handle(&leds);

    keyboard.process_keys();

    sleep_ms(3);
}

int main()
{
    setup();

    while (true)
    {
        loop();
    }
}
