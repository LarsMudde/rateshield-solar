#ifndef Constants_h
#define Constants_h

// File name for storing the JSON data
#define FORECAST_FILE_NAME "/forecast.json"

// Default Wi-Fi credentials
#define WIFI_SSID "RateShield Solar"
#define WIFI_PASSWORD ""

// Timezone settings
#define TIMEZONE "Europe/Amsterdam"

// Maximum age of the forecast file in seconds (1 hour = 3600 seconds)
#define FORECAST_MAX_AGE 28800

// Base URL for the Energy Prices API
#define ENERGY_PRICES_BASE_URL "https://api.energyzero.nl/v1/energyprices"

// Interval and inclBtw constants
#define INTERVAL 4
#define INCL_BTW false

#endif // Constants_h
