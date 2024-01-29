#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <exports.h>

static void setup(void);
static void show_digit(int digit);

#if CONFIG_IS_ENABLED(PLATFORM_EVALUATOR7T)
#define DIGIT0      0x17c00
#define DIGIT1      0x01800
#define DIGIT2      0x0ec00
#define DIGIT3      0x0bc00
#define DIGIT4      0x19800
#define DIGIT5      0x1b400
#define DIGIT6      0x1f400
#define DIGIT7      0x01c00
#define DIGIT8      0x1fc00
#define DIGIT9      0x19c00

static const unsigned DigitMap[] = { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9 };

static void inline setup()
{
    clrbits(32, REG_IOPMODE, (1 << 8));     // Set I/O[8] (i.e. INT0) in input mode
    setbits(32, REG_IOPMODE, (0x7f << 10)); // Set I/O[16:10] (i.e. 7-Segment LED) in output mode
    out_32(REG_IOPCON, (in_32(REG_IOPCON) & ~0x1f) | 0x1d);
}

static void inline show_digit(int digit)
{
    clrbits(32, REG_IOPDATA, DigitMap[8]);
    setbits(32, REG_IOPDATA, DigitMap[digit % 10]);
}
#elif CONFIG_IS_ENABLED(PLATFORM_NETSTART)
#define DIGIT0      0x21
#define DIGIT1      0xaf
#define DIGIT2      0x32
#define DIGIT3      0x26
#define DIGIT4      0xac
#define DIGIT5      0x64
#define DIGIT6      0x60
#define DIGIT7      0x2f
#define DIGIT8      0x20
#define DIGIT9      0x24

static const uchar DigitMap[] = { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9 };

static void inline setup()
{
}

static void inline show_digit(int digit)
{
    out_8(IO_BANK(1), DigitMap[digit % 10]);
}
#endif

int main (int argc, char * const argv[])
{
    register unsigned count = 0;
    app_startup(argv);

    setup();
    printf ("Press any key to exit ... ");
    while (!tstc()) {
        if (++count > 9) count = 0;
        show_digit(count);
        mdelay(1000);
    }
    (void) getc();
    printf("\r\n");

    return (0);
}
