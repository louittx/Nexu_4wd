idf_component_register(SRCS "main.cpp" "lib/BT/BT.c" "lib/config/config.cpp" "lib/RS485/RS485.cpp"
PRIV_REQUIRES spi_flash
                    REQUIRES esp32-shell
INCLUDE_DIRS ".")
