set(req i2cdev)
idf_component_register(
    SRCS ads7828.c
    INCLUDE_DIRS .
    REQUIRES ${req}
)
