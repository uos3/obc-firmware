FW_SRCS=$(FW_ROOT)/src/main/$(MAIN).c       \
    $(wildcard $(FW_ROOT)/src/*.c)          \
    $(wildcard $(FW_ROOT)/src/cc1125/*.c)   \
    $(wildcard $(FW_ROOT)/src/packet/*.c)   \
    $(wildcard $(FW_ROOT)/src/suz/*.c) 

UDEFS += $(DEFINES)
