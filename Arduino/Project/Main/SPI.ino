// SPI.ino

#include <stdint.h>
#include <SPI.h>

// ESP-IDF SPI driver
#include "driver/spi_common.h"
#include "hal/spi_types.h"

// Prototypes
#define SPI_SPEED 1000000 // 1MHz
#define SPI_BITORDER MSBFIRST
#define SPI_MODE SPI_MODE0

#define PIN_SPI_MOSI 35
#define PIN_SPI_MISO 37
#define PIN_SPI_SCLK 36
#define PIN_SPI_CS 38

void SPI_Setup() {
  spi_bus_config_t conf;
  conf.mosi_io_num = PIN_SPI_MOSI;
  conf.miso_io_num = PIN_SPI_MISO;
  conf.sclk_io_num = PIN_SPI_SCLK;

  // Features that are disabled:
  conf.quadwp_io_num = -1;
  conf.data0_io_num = -1;
  conf.data1_io_num = -1;
  conf.data2_io_num = -1;
  conf.data3_io_num = -1;
  conf.data4_io_num = -1;
  conf.data5_io_num = -1;
  conf.data6_io_num = -1;
  conf.data7_io_num = -1;

  auto err = spi_bus_initialize(
    SPI3_HOST, // Unsure about this. Could be SPI2_HOST
    &conf,
    SPI_DMA_DISABLED
  );
}


void SPI_setup() {
  SPI.setPins();
  SPI.begin(
    PIN_SPI_SCLK,
    PIN_SPI_MISO,
    PIN_SPI_MOSI,
    PIN_SPI_CS
  );
  return new SPISettings(
    SPI_SPEED,
    SPI_BITORDER,
    SPI_MODE
  );
}

void SPI_delete(SPISettings *settings) {
  delete settings;
}

void SPI_command(SPISettings *settings, uint8_t *bytes, int count) {
  SPI.beginTransaction(*settings);

}

