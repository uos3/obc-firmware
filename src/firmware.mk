FW_SRCS=$(FW_ROOT)/src/$(MAIN).c       \
    $(wildcard $(FW_ROOT)/src/driver/*.c)   \
    $(wildcard $(FW_ROOT)/src/component/*.c) \
    $(wildcard $(FW_ROOT)/src/utility/*.c) \
    $(wildcard $(FW_ROOT)/src/mission/*.c)

UDEFS += $(DEFINES)
