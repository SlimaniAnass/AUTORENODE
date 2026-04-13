CC      = arm-none-eabi-gcc

CFLAGS  = -mcpu=cortex-m4 -mthumb -O0 -g \
          -nostdlib -ffreestanding \
          -Wall -Wextra \
          -Iplatform/include \
          -Iplatform/S32K144 \
          -Iautosar/Can \
          -Iautosar/CanIf

SRCS    = autosar/Can/Can.c \
          autosar/Can/Can_Cfg.c

OBJS    = $(SRCS:.c=.o)

all: $(OBJS)
	@echo "Build OK"

clean:
	rm -f $(OBJS)

syntax-check:
	$(CC) $(CFLAGS) -fsyntax-only $(SRCS)
	@echo "Syntax OK"
