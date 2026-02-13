#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace ls027b7dh01 {

// Display rozměry
static const uint16_t LS027B7DH01_WIDTH = 400;
static const uint16_t LS027B7DH01_HEIGHT = 240;

// Sharp LCD příkazy
static const uint8_t SHARP_LCD_BIT_WRITECMD = 0x01;
static const uint8_t SHARP_LCD_BIT_VCOM = 0x02;
static const uint8_t SHARP_LCD_BIT_CLEAR = 0x04;

class LS027B7DH01 : public display::DisplayBuffer,
                    public spi::SPIDevice<spi::BIT_ORDER_LSB_FIRST, 
                                          spi::CLOCK_POLARITY_LOW,
                                          spi::CLOCK_PHASE_LEADING, 
                                          spi::DATA_RATE_2MHZ> {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  
  display::DisplayType get_display_type() override { 
    return display::DisplayType::DISPLAY_TYPE_BINARY; 
  }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_height_internal() override { return LS027B7DH01_HEIGHT; }
  int get_width_internal() override { return LS027B7DH01_WIDTH; }
  
  void write_display_data_();
  void clear_display_();
  void toggle_vcom_();
  uint8_t reverse_bits_(uint8_t b);
  
  uint8_t *buffer_{nullptr};
  bool vcom_state_{false};
  
  static const size_t BUFFER_SIZE = (LS027B7DH01_WIDTH / 8) * LS027B7DH01_HEIGHT;
};

}  // namespace ls027b7dh01
}  // namespace esphome
