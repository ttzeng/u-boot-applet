#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <exports.h>

static void pwm_init(int ch, int div, int tc)
{
    uint32_t base = (ch? ((ch==1)? PWM1_BASE : PWM2_BASE) : PWM0_BASE);
    /* set the PWM clock frequency */
    out_8(base + PWM_DIV_OFS, div);
    /* normal mode */
    clrbits_8(base + PWM_SYNC_OFS, 1);
    /* output is not inverted */
    clrbits_8(base + PWM_INV_OFS, 1);
    /* set cycle time */
    (ch < 2)? out_8(base + PWM_TC_OFS, tc) : out_32(base + PWM_TC_OFS, tc);
    /* enable the PWM */
    setbits_8(base + PWM_ENB_OFS, 1);
}

static void pwm_set_dutycycle(int ch, int dc)
{
    uint32_t base = (ch? ((ch==1)? PWM1_BASE : PWM2_BASE) : PWM0_BASE);
    (ch < 2)? out_8(base + PWM_DC_OFS, dc) : out_32(base + PWM_DC_OFS, dc);
}

static uint dutycycle_in_sine(uint degree)
{
    uchar sine_table[] =
/* degree: 0  5 10  15  20  25  30  35  40  45  50  55  60  65  70  75  80  85  90 */
        {  1, 4, 9, 13, 17, 21, 25, 28, 31, 35, 38, 40, 42, 44, 46, 47, 48, 49, 49 };
    int i;
    degree %= 180;
    if (degree > 90)
        degree = 180 - degree;
    for (i = 0; degree > 5; degree -= 5, i++);
    return sine_table[i];
}

#define KEY_CTRL_C  0x03
#define KEY_ESC     0x1b

int main(int argc, char *const argv[])
{
    app_startup(argv);

    /* Enable XCLK to PWMs */
    setbits_32(PCSR, PCSR_PWMCE);

    /* Set all PWM clock frequency to 500KHz (25MHz/50, 2 usec), 100us cycle time */
    pwm_init(0, 50, 49);
    pwm_init(1, 50, 49);
    pwm_init(2, 50, 49);

    uint phase[] = { 0, 30, 60 };

    printf ("Press ESC or CTRL-C to exit ...\n");
    int key;
    for (key = 0; !(tstc() &&
                    ((key = getc()) == KEY_ESC || key == KEY_CTRL_C)); key = 0) {
        int i;
        /* set PWM duty cycle */
        for (i = 0; i < sizeof(phase) / sizeof(uint); i++) {
            uint *dc = phase + i;
            pwm_set_dutycycle(i, dutycycle_in_sine(*dc));
            *dc = (*dc + 5) % 360;
        }
        mdelay(200);

        switch (key) {
            default:
                if (key)
                    printf("Key '%02x' is not supported, press other key ...\n", key);
        }
    }

    return 0;
}

void *memcpy(void* dest, const void* src, size_t len)
{
    uchar *s = (uchar*)src, *d = (uchar*)dest;
    for (; len > 0; len--)
        *d++ = *s++;
    return d;
}

void hang(void)
{
    for (;;);
}
