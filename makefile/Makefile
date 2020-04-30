################################################################################

BIN = bin
BUILD = build

################################################################################


.PHONY: all flash terminal dump_eeprom flash_all 
.PHONY: chip_test directory clean size

CHIP = x128a1
PROG = atmelice
ELF  = $(BUILD)/$(BIN).elf
AVRDUDE = avrdude -p $(CHIP) -c $(PROG)

CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

CFLAGS += -x c -funsigned-char -funsigned-bitfields 
CFLAGS += -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums 
CFLAGS += -Wall -mmcu=atxmega128a1 
CFLAGS += -B "../include/atxmega128a1" 
CFLAGS += -c -gdwarf-2 -std=gnu99 
CFLAGS += -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   
CFLAGS += -o "$@" "$<" 

LDFLAGS += -Wl,-Map="$(BUILD)/$(BIN).map" 
LDFLAGS += -Wl,--start-group 
LDFLAGS += -Wl,-lm  
LDGLAGS += -Wl,--end-group 
LDFLAGS += -Wl,--gc-sections 
LDFLAGS += -mmcu=atxmega128a1 
LDFLAGS += -B "../include/atxmega128a1"  

INCLUDES += \
	-I" ../include"

CFLAGS += $(INCLUDES)

RM := rm -rf

CHIP = x128a1
PROG = atmelice
AVRDUDE = avrdude -p $(CHIP) -c $(PROG)

# Add inputs and outputs from these tool invocations to the build variables 
SRCS +=  \
../main.c \
../dma_driver.c \
../dac_driver.c

OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))

# AVR32/GNU C Compiler

$(BUILD)/%.o: .././%.c
	@: $<
	@echo Invoking: AVR/GNU C Compiler 
	@$(CC) $(CFLAGS) 
	@echo Finished building: $<
	
all: directory $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).hex $(BUILD)/$(BIN).eep $(BUILD)/$(BIN).lss $(BUILD)/$(BIN).srec size

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo LD : $@
	@$(CC) -o$@ $(OBJS) $(LIBS) $(LDFLAGS) 
	@echo Finished building target: $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $^ $@

$(BUILD)/$(BIN).eep: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex $^ $@ || exit 0

$(BUILD)/$(BIN).lss: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -h -S "$(BUILD)/$(BIN).elf" > "$(BUILD)/$(BIN).lss"

$(BUILD)/$(BIN).srec: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $^ $@


clean:
	@echo clean
	rm -rf $(BUILD)
	
directory:
	@mkdir -p $(BUILD)

size: $(BUILD)/$(BIN).elf
	@$(SIZE) -t $^

terminal:
	$(AVRDUDE) -t

flash:
	$(AVRDUDE) -U flash:w:$(ELF):e

flash_all:
	$(AVRDUDE) -U flash:w:$(ELF):e -U eeprom:w:$(ELF):e

chip_test:
	$(AVRDUDE)

build_and_flash: all flash
