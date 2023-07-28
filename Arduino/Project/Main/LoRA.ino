// LoRA.ino

#include "SPI.ino"

// ESP-IDF SPI driver
#include "driver/spi_common.h"
#include "hal/spi_types.h"
#include "driver/spi_master.h"
#include "Helper.io"

// SPI Configuration
#define SPI_SPEED 1000000 // 1MHz
#define SPI_BITORDER MSBFIRST
#define SPI_MODE SPI_MODE0

#define PIN_SPI_MOSI 35
#define PIN_SPI_MISO 37
#define PIN_SPI_SCLK 36

#define PIN_LORA_DIO1 15
#define PIN_LORA_DIO3 16

// Chip select of LLCC68 (NSS)
#define PIN_LLCC68_NSS 38

// NSS is chip select 

void LoRA_setup(SPI_Settings *settings);

void LoRA_write(uint8_t *data, int data_len);

// Commands map
#define LORA_COMMAND_SET_SLEEP 0x84
#define LORA_COMMAND_SET_STANDBY 0x80
#define LORA_COMMAND_SET_FS 0xC1
#define LORA_COMMAND_SET_TX 0x83
#define LORA_COMMAND_SET_RX 0x82
#define LORA_COMMAND_STOP_TIMER_ON_PREAMBLE 0x9F
#define LORA_COMMAND_SET_RX_DUTY_CYCLE 0x94
#define LORA_COMMAND_SET_CAD 0xC5
#define LORA_COMMAND_SET_TX_CONTINUOUS_WAVE 0xD1
#define LORA_COMMAND_SET_TX_INFINITE_PREAMBLE 0xD2
#define LORA_COMMAND_SET_REGULATOR_MODE 0x96
#define LORA_COMMAND_CALIBRATE 0x89
#define LORA_COMMAND_CALIBRATE_IMAGE 0x98
#define LORA_COMMAND_SET_PA_CONFIG 0x95
#define LORA_COMMAND_SET_RX_TX_FALLBACK_MODE 0x93
#define LORA_COMMAND_WRITE_REGISTER 0x0D
#define LORA_COMMAND_READ_REGISTER 0x1D
#define LORA_COMMAND_WRITE_BUFFER 0x0E
#define LORA_COMMAND_READ_BUFFER 0x1E
#define LORA_COMMAND_SET_DIO_IRQ_PARAMS 0x08
#define LORA_COMMAND_GET_IRQ_STATUS 0x12
#define LORA_COMMAND_CLEAR_IRQ_STATUS 0x02
#define LORA_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL 0x9D
#define LORA_COMMAND_SET_DIO3_AS_TXCO_CTRL 0x97
#define LORA_COMMAND_SET_RF_FREQUENCY 0x86
#define LORA_COMMAND_SET_PACKET_TYPE 0x8A
#define LORA_COMMAND_GET_PACKET_TYPE 0x11
#define LORA_COMMAND_SET_TX_PARAMS 0x8E
#define LORA_COMMAND_SET_MODULATION_PARAMS 0x8B
#define LORA_COMMAND_SET_PACKET_PARAMS 0x8C
#define LORA_COMMAND_SET_CAD_PARAMS 0x88
#define LORA_COMMAND_SET_BUFFER_BASE_ADDRESS 0x8F
#define LORA_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT 0xA0
#define LORA_COMMAND_GET_STATUS 0xC0
#define LORA_COMMAND_GET_RSSI_INST 0x15
#define LORA_COMMAND_GET_RX_BUFFER_STATUS 0x13
#define LORA_COMMAND_GET_PACKET_STATUS 0x14
#define LORA_COMMAND_GET_DEVICE_ERRORS 0x17
#define LORA_COMMAND_CLEAR_DEVICE_ERRORS 0x07
#define LORA_COMMAND_GET_STATS 0x10
#define LORA_COMMAND_RESET_STATS 0x00

bool _lora_check_busy();

// Returns count of data in rx_buffer
int _send_lora_command(spi_device_handle_t dev, uint8_t opcode, uint8_t data, uint8_t *rx_buffer);
int _send_lora_command(spi_device_handle_t dev, uint8_t opcode, uint8_t *data, int data_len, uint8_t *rx_buffer);

// Set device to sleep
void _lora_sleep(spi_device_handle_t dev);

// Send config data
void LoRA_load_config(spi_device_handle_t dev);

// Convenience wrapper for single byte payloads
int _send_lora_command(spi_device_handle_t dev, uint8_t opcode, uint8_t data) {
  uint8_t tx_buffer[1] = {data};
  return _send_lora_command(dev, opcode, tx_buffer, 1, rx_buffer);
}

void _send_lora_command(spi_device_handle_t dev, uint8_t opcode, uint8_t *data, int data_len, uint8_t *rx_buffer) {
  spi_transaction_t t;
  t.cmd = opcode;
  t.length = data_len;
  t.tx_buffer = data;
  t.rx_buffer = rx_buffer;
  t.flags = 0; // I think zero is correct.

  // Acqure bus
  esp_err_t err = spi_device_acquire_bus(dev);
  check_err(err);

  // Should confirm device is not "busy"
  while (_lora_check_busy());

  err = spi_device_transmit(dev, &t);
  check_err(err);

  // Release bus
  err = spi_device_release_bus(dev);
  check_err(err);

  return t.rxlength;
}

spi_device_handle_t LoRA_setup(SPI_Settings *settings) {
  // Configure SPI bus
  spi_bus_config_t bus_conf;
  bus_conf.mosi_io_num = PIN_SPI_MOSI;
  bus_conf.miso_io_num = PIN_SPI_MISO;
  bus_conf.sclk_io_num = PIN_SPI_SCLK;

  bus_conf.flags = 0; // Looks like no flags

  // Features that are disabled:
  bus_conf.quadwp_io_num = -1;
  bus_conf.data0_io_num = -1;
  bus_conf.data1_io_num = -1;
  bus_conf.data2_io_num = -1;
  bus_conf.data3_io_num = -1;
  bus_conf.data4_io_num = -1;
  bus_conf.data5_io_num = -1;
  bus_conf.data6_io_num = -1;
  bus_conf.data7_io_num = -1;

  // Shared error
  esp_err_t err;

  err = spi_bus_initialize(
    SPI3_HOST, // Unsure about this. Could be SPI2_HOST
    &bus_conf,
    SPI_DMA_DISABLED
  );
  check_err(err);

  // LLCC68 SPI config
  spi_device_interface_config_t dev_conf;
  dev_conf.command_bits = 8;
  dev_conf.address_bits = 0;
  dev_conf.mode = 0; // CPOL, CPHA configuration
  dev_conf.spics_io_num = PIN_LLCC68_NSS;
  dev_conf.queue_size = 32; // Seems reasonable. Not sure what this does.

  spi_device_handle_t llcc68 = new spi_device_t();

  // Add LLCC68 as a SPI device
  err = spi_bus_add_device(
    SPI3_HOST,
    dev_conf,
    llcc68
  );
  check_err(err);

  LoRA_load_config(llcc68);
}

void LoRA_delete(spi_device_handle_t llcc68) {
  auto err = spi_bus_remove_device(llcc68);
  check_err(err);
}

void LoRA_write(spi_device_handle_t llcc68, uint8_t *data, int data_len) {
  // Only allowing writing the entire buffer at once. 
  // This means the first byte must be 0
  uint8_t *payload = malloc(sizeof(uint8_t) * (data_len + 1));
  
  payload[0] = 0;
  memcpy(payload + 1, data, data_len);

  // Enable standby mode
  _send_lora_command(llcc68, LORA_COMMAND_SET_STANDBY, 1);

  // Send data to llcc68
  _send_lora_command(llcc68, LORA_COMMAND_WRITE_BUFFER, payload, data_len+1);
  free(payload);

  // Set Tx mode (initiate transaction)
  // Fill in 
  _send_lora_command(llcc68, LORA_COMMAND_SET_TX, 1); 

  // Wait for TxDone or Timeout

  // Clear IRQ TxDone

  // Go into sleep mode(?)
}

void LoRA_read(spi_device_handle_t llcc68, uint8_t *buffer) {
  // Enable standby mode
  _send_lora_command(llcc68, LORA_COMMAND_SET_STANDBY, 1);

  // Define sync word value (?)

  // Enter rx mode
  _send_lora_command(llcc68, LORA_COMMAND_SET_RX, 1);

  // Wait for RxDone or Timeout

  // Check CRC with GetIRQStatus
  _send_lora_command(llcc68, LORA_COMMAND_GET_IRQ_STATUS, 1);

  // Clear RxDone / timeout

  // GetRxBufferStatus()
}

void _lora_sleep(spi_device_handle_t dev) {
  // Device packet
  uint8_t warm_start = 1; // As opposed to cold start - retains config data
  uint8_t RFU = 0; // Idk
  uint8_t timeout = 0; // Disable RTC timeout startup

  uint8_t payload = (warm_start << 2) | (RFU << 1) | (timeout << 0); 

  
}

void LoRA_load_config(spi_device_handle_t llcc68) {
  // Acquire bus (assume setup correctly)
  esp_err_t err = spi_device_acquire_bus(dev);
  check_err(err);

  // Issue configure commands

  // From datasheet section 14.2 / 14.3
  // Enter standby
  // Todo: fill in values
  _send_lora_command(llcc68, LORA_COMMAND_SET_STANDBY, 1);

  // Turn on DC-DC reg (more efficient than LDO)
  _send_lora_command(llcc68, LORA_COMMAND_SET_REGULATOR_MODE, 1);

  // Specify LoRA protocol
  _send_lora_command(llcc68, LORA_COMMAND_SET_PACKET_TYPE, 1);

  // Define RF frequency to 915MHz (Australian band)
  _send_lora_command(llcc68, LORA_COMMAND_SET_SET_RF_FREQUENCY, 1);

  // Define power amplifier configuration
  _send_lora_command(llcc68, LORA_COMMAND_SET_PA_CONFIG, 1);

  // Define output power and ramping time with SetTxParams
  _send_lora_command(llcc68, LORA_COMMAND_SET_TX_PARAMS, 1);

  // Define data payload with SetBufferBaseAddress
  _send_lora_command(llcc68, LORA_COMMAND_SET_BUFFER_BASE_ADDRESS, 1);

  // Send payload to data buffer with WriteBuffer (maybe only when actually about to Tx)
  _send_lora_command(llcc68, LORA_COMMAND_WRITE_BUFFER, 1);

  // Define modulation parameter with SetModulationParams
  _send_lora_command(llcc68, LORA_COMMAND_SET_MODULATION_PARAMS, 1);

  // Define frame format with SetPacketParams
  _send_lora_command(llcc68, LORA_COMMAND_SET_PACKET_PARAMS, 1);

  // Configure DIO and IRQ with SetDioIrqParams
  _send_lora_command(llcc68, LORA_COMMAND_SET_DIO_IRQ_PARAMS, 1);

  // Define Sync Word value using WriteReg (manually)
  _send_lora_command(llcc68, LORA_COMMAND_WRITE_REG, 1);

  // Begin Tx with SetTx() with optional timeout (probably a good thing)
  _send_lora_command(llcc68, LORA_COMMAND_SET_TX, 1);

  // Wait for IRX TxDone or Timeout

  // Clear IRQ TxDone flag
}