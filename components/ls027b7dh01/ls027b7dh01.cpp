#include "ls027b7dh01.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ls027b7dh01 {

static const char *const TAG = "ls027b7dh01";

void LS027B7DH01::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LS027B7DH01...");
  
  // Allocate buffer for framebuffer
  // Buffer size = (width / 8) * height = (400 / 8) * 240 = 50 * 240 = 12000 bytes
  this->buffer_ = new uint8_t[BUFFER_SIZE];
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Failed to allocate buffer");
    this->mark_failed();
    return;
  }
  
  // Initialize SPI
  this->spi_setup();
  
  // Clear buffer and display
  memset(this->buffer_, 0xFF, BUFFER_SIZE);  // 0xFF = all white (Sharp LCD uses 1=white, 0=black)
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
  ESP_LOGD(TAG, "Update called");  // ← PŘIDAT
  // Toggle VCOM bit (must be done at least once per second)
  this->toggle_vcom_();
  ESP_LOGD(TAG, "About to call do_update_");  // ← PŘIDAT
  // Update display if anything changed
  this->do_update_();
  ESP_LOGD(TAG, "Update complete");  // ← PŘIDAT
  // Write buffer to display
  this->write_display_data_();  // ← TOTO CHYBĚLO!
}

void LS027B7DH01::fill(Color color) {
  // Fill entire buffer with color
  // Sharp LCD: 1 = white, 0 = black
  uint8_t fill_byte = color.is_on() ? 0x00 : 0xFF;
  memset(this->buffer_, fill_byte, BUFFER_SIZE);
}

void LS027B7DH01::draw_absolute_pixel_internal(int x, int y, Color color) {
  // Boundary check
  if (x < 0 || x >= LS027B7DH01_WIDTH || y < 0 || y >= LS027B7DH01_HEIGHT)
    return;
  
  // Calculate buffer position
  // Each row is 50 bytes (400 pixels / 8 bits per byte)
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  const uint16_t byte_offset = y * bytes_per_row + (x / 8);
  const uint8_t bit_offset = 7 - (x % 8);  // MSB first within each byte
  
  // Set or clear the bit
  // Sharp LCD: 1 = white, 0 = black
  if (color.is_on()) {
    // Black pixel
    this->buffer_[byte_offset] &= ~(1 << bit_offset);
  } else {
    // White pixel  
    this->buffer_[byte_offset] |= (1 << bit_offset);
  }
}

void LS027B7DH01::write_display_data_() {
  // Write all lines to the display
  const uint16_t bytes_per_row = LS027B7DH01_WIDTH / 8;
  
  this->enable();
  
  // Send write command
  uint8_t command = SHARP_LCD_BIT_WRITECMD;
  if (this->vcom_state_) {
    command |= SHARP_LCD_BIT_VCOM;
  }
  this->write_byte(command);
  
  // Send each line
  for (uint16_t line = 0; line < LS027B7DH01_HEIGHT; line++) {
    // Line address (1-indexed)
    uint8_t line_addr = line + 1;
    
    // Reverse bits in line address for LSB first transmission
    line_addr = ((line_addr & 0x01) << 7) | ((line_addr & 0x02) << 5) | 
                ((line_addr & 0x04) << 3) | ((line_addr & 0x08) << 1) |
                ((line_addr & 0x10) >> 1) | ((line_addr & 0x20) >> 3) | 
                ((line_addr & 0x40) >> 5) | ((line_addr & 0x80) >> 7);
    
    this->write_byte(line_addr);
    
    // Send line data
    uint16_t offset = line * bytes_per_row;
    for (uint16_t i = 0; i < bytes_per_row; i++) {
      // Reverse bits in each byte for LSB first transmission
      uint8_t data = this->buffer_[offset + i];
      data = ((data & 0x01) << 7) | ((data & 0x02) << 5) | 
             ((data & 0x04) << 3) | ((data & 0x08) << 1) |
             ((data & 0x10) >> 1) | ((data & 0x20) >> 3) | 
             ((data & 0x40) >> 5) | ((data & 0x80) >> 7);
      this->write_byte(data);
    }
    
    // Send dummy byte (trailer)
    this->write_byte(0x00);
  }
  
  // Send final trailer
  this->write_byte(0x00);
  
  this->disable();
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


