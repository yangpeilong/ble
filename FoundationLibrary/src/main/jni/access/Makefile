LOCAL_SRC_FILES := qdkey.c meshsub.c

OBJS := $(patsubst %.c, $(BUILD_OUTPUT_DIR)/%.o, $(LOCAL_SRC_FILES))

all : $(OBJS)

$(BUILD_OUTPUT_DIR)/%.o : $(LOCAL_SRC_FILES)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJS)
