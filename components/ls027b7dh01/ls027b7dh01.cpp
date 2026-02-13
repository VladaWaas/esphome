#include "ls027b7dh01.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ls027b7dh01 {

static const char *const TAG = "ls027b7dh01";

void LS027B7DH01::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Sharp Memory LCD...");
  
  // Alokace bufferu
  this->buffer_ = new uint8_t[BUFFER_SIZE];
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Failed to allocate buffer!");
    this->mark_failed();
    return;
  }
  
  // Inicializace SPI
  this->spi_setup();
  
  // Vyčištění displeje a bufferu
  memset(this->buffer_, 0xFF, BUFFER_SIZE);  // 0xFF = bílý
  
  // Pošli CLEAR příkaz na displej
  this->clear_display_();
  
  ESP_LOGCONFIG(TAG, "Sharp Memory LCD initialized");
}

void LS027B7DH01::dump_config() {
  ESP_LOGCONFIG(TAG, "Sharp Memory LCD LS027B7DH01:");
  ESP_LOGCONFIG(TAG, "  Width: %d", LS027B7DH01_WIDTH);
  ESP_LOGCONFIG(TAG, "  Height: %d", LS027B7DH01_HEIGHT);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}

void LS027B7DH01::update() {
  // Toggle VCOM (nutné každou sekundu)
  this->toggle_vcom_();
  
   // VYČISTIT displej VŽDY před update!
  this->clear_display_();  // ← PŘIDAT!
  
  // Zavolá lambda a vykreslí do bufferu
  this->do_update_();
  
  // Odešle buffer na displej
  this->write_display_data_();
}

void LS027B7DH01::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x < 0 || x >= LS027B7DH01_WIDTH || y < 0 || y >= LS027B7DH01_HEIGHT)
    return;
  
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  const uint16_t byte_offset = y * bytes_per_row + (x / 8);
  const uint8_t bit_offset = 7 - (x % 8);
  
  // Sharp LCD: 1 = bílý, 0 = černý
  if (color.is_on()) {
    // Černý pixel
    this->buffer_[byte_offset] &= ~(1 << bit_offset);
  } else {
    // Bílý pixel
    this->buffer_[byte_offset] |= (1 << bit_offset);
  }
}

uint8_t LS027B7DH01::reverse_bits_(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void LS027B7DH01::write_display_data_() {
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  
  this->enable();
  
  // Příkaz WRITE + VCOM bit
  uint8_t command = SHARP_LCD_BIT_WRITECMD;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  
  // Pošli všechny řádky
  for (uint16_t line = 0; line < LS027B7DH01_HEIGHT; line++) {
    // Adresa řádku (1-indexed)
    uint8_t line_addr = line + 1;
    
    // LSB first - reverse bits
    line_addr = this->reverse_bits_(line_addr);
    this->write_byte(line_addr);
    
    // Data řádku
    uint16_t offset = line * bytes_per_row;
    for (uint16_t i = 0; i < bytes_per_row; i++) {
      uint8_t data = this->buffer_[offset + i];
      
      // LSB first - reverse bits
      data = this->reverse_bits_(data);
      this->write_byte(data);
    }
    
    // Trailer byte
    this->write_byte(0x00);
  }
  
  // Final trailer
  this->write_byte(0x00);
  
  this->disable();
}

void LS027B7DH01::clear_display_() {
  this->enable();
  
  uint8_t command = SHARP_LCD_BIT_CLEAR;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  this->write_byte(0x00);
  
  this->disable();
  
  // Vyčisti i buffer
  memset(this->buffer_, 0xFF, BUFFER_SIZE);
}

void LS027B7DH01::toggle_vcom_() {
  this->vcom_state_ = !this->vcom_state_;
  
  this->enable();
  
  uint8_t command = 0x00;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  this->write_byte(0x00);
  
  this->disable();
}

}  // namespace ls027b7dh01
}  // namespace esphome




