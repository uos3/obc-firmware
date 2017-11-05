FW_SRCS=$(FW_ROOT)/src/main/$(MAIN).c       \
    $(wildcard $(FW_ROOT)/src/cc1125/*.c)   \
    $(wildcard $(FW_ROOT)/src/buffer/*.c)   \
    $(wildcard $(FW_ROOT)/src/packet/*.c)   \
    $(wildcard $(FW_ROOT)/src/configuration/*.c) \
    $(wildcard $(FW_ROOT)/src/mission/*.c)

UDEFS += $(DEFINES)
