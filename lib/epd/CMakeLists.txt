set(req freertos)
idf_component_register(
    SRCS epd.c
    INCLUDE_DIRS .
    REQUIRES ${req}
)
