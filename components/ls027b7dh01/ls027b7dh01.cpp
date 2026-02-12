#include "ls027b7dh01.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ls027b7dh01 {

static const char *const TAG = "ls027b7dh01";

void LS027B7DH01::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LS027B7DH01...");
  
  this->spi_setup();
  
  // DisplayBuffer už má buffer alokovaný - jen ho vynuluj
  memset(this->buffer, 0xFF, BUFFER_SIZE);
  
  this->clear_display_();
  
  ESP_LOGCONFIG(TAG, "LS027B7DH01 setup complete");
}


void LS027B7DH01::dump_config() {
  ESP_LOGCONFIG(TAG, "LS027B7DH01:");
  ESP_LOGCONFIG(TAG, "  Width: %d", LS027B7DH01_WIDTH);
  ESP_LOGCONFIG(TAG, "  Height: %d", LS027B7DH01_HEIGHT);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}

void LS027B7DH01::update() {
  ESP_LOGD(TAG, "Update called");
  
  // Toggle VCOM bit (must be done at least once per second)
  this->toggle_vcom_();
  
  ESP_LOGD(TAG, "About to call do_update_");
  
  // Update display if anything changed
  this->do_update_();
  
  ESP_LOGD(TAG, "About to write display data");  // ← PŘIDAT TOTO!
  
  // Write buffer to display
  this->write_display_data_();
  
  ESP_LOGD(TAG, "Update complete");
}

void LS027B7DH01::fill(Color color) {
  ESP_LOGD(TAG, "fill() called! color.is_on=%d", color.is_on());
  
  // ZAKOMENTOVÁNO - necháme buffer nedotčený
  // uint8_t fill_byte = color.is_on() ? 0x00 : 0xFF;
  // memset(this->buffer_, fill_byte, BUFFER_SIZE);
  
  ESP_LOGD(TAG, "fill() disabled - buffer unchanged");
}

void LS027B7DH01::draw_absolute_pixel_internal(int x, int y, Color color) {
  // První pixel - zaloguj
  static bool first_log = true;
  if (first_log) {
    ESP_LOGD(TAG, "draw_pixel called! x=%d, y=%d, color.is_on=%d", x, y, color.is_on());
    first_log = false;
  }
  
  // Boundary check
  if (x < 0 || x >= LS027B7DH01_WIDTH || y < 0 || y >= LS027B7DH01_HEIGHT)
    return;
  
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  const uint16_t byte_offset = y * bytes_per_row + (x / 8);
  const uint8_t bit_offset = 7 - (x % 8);
  
  if (color.is_on()) {
    this->buffer_[byte_offset] &= ~(1 << bit_offset);
  } else {
    this->buffer_[byte_offset] |= (1 << bit_offset);
  }
}

void LS027B7DH01::write_display_data_() {
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  
  ESP_LOGD(TAG, "write_display_data START");
  ESP_LOGD(TAG, "Bytes per row: %d, total lines: %d", bytes_per_row, LS027B7DH01_HEIGHT);
  
  this->enable();
  ESP_LOGD(TAG, "CS enabled");
  
  uint8_t command = SHARP_LCD_BIT_WRITECMD;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  ESP_LOGD(TAG, "Sending command: 0x%02X", command);
  this->write_byte(command);
  
  ESP_LOGD(TAG, "Sending first line (line 0, addr 1)");
  uint8_t line_addr = 1;
  this->write_byte(line_addr);
  
  // Pošli jen první řádek pro test
  for (uint16_t i = 0; i < bytes_per_row; i++) {
    this->write_byte(this->buffer_[i]);
  }
  this->write_byte(0x00);
  
  ESP_LOGD(TAG, "First 5 bytes: %02X %02X %02X %02X %02X", 
    this->buffer_[0], this->buffer_[1], this->buffer_[2], 
    this->buffer_[3], this->buffer_[4]);
  
  this->write_byte(0x00);
  this->disable();
  ESP_LOGD(TAG, "write_display_data END");
}

void LS027B7DH01::clear_display_() {
  // Clear display using clear command
  this->enable();
  
  uint8_t command = SHARP_LCD_BIT_CLEAR;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  
  // Send trailer
  this->write_byte(0x00);
  
  this->disable();
  
  // Also clear the buffer
  memset(this->buffer_, 0xFF, BUFFER_SIZE);
}

void LS027B7DH01::toggle_vcom_() {
  // Toggle VCOM bit (required at least once per second to prevent DC buildup)
  this->vcom_state_ = !this->vcom_state_;
  
  this->enable();
  
  uint8_t command = 0x00;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  
  // Send trailer
  this->write_byte(0x00);
  
  this->disable();
}

}  // namespace ls027b7dh01
}  // namespace esphome















