# Tento soubor je kvůli ESPHome display platformě
# Re-exportuje komponentu jako display platform

from esphome.components import display
from . import ls027b7dh01_ns, LS027B7DH01

# Registrace jako display platforma
PLATFORM_SCHEMA = None  # Používáme schema z __init__.py
