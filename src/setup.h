// --- Capture ---
#define CAPTURE_BUTTON_DELAY 5000               // Delay after input capture is triger
#define CAPTURE_TIMEOUT 30000                   // Delay after quit capture mode

// --- Blink LED (for Capture) ---
#define BLINK_LED_CAPTURE_INTERVAL 200          // Blinking frequency (waiting for capture)
#define BLINK_LED_CAPTURE_DONE_INTERVAL 25      // Blinking frequency (capture done)
#define BLINK_LED_CAPTURE_DONE_DURATION 500     // delay during LED is ON

// --- Blink Led (for Execute command) ---
#define BLINK_LED_EXECUTE_INTERVAL 60           // Blinking frequency (for execute command)
#define BLINK_LED_EXECUTE_DURATION 480          // delay during LED is ON

// --- File System ---
#define FORMAT_FILE_SYSTEM_IF_FAILED false      // Format file system partition if failed.
#define ENABLED_IO_FILESYSTEM_SAVING true       // Allow writing file (if File system is enabled for this hardware)
//#define COMMANDS_JSON_FILENAME "/commands.json" // File name must begin with "/" and not exced 31 charactères.
#define COMMAND_EXTENSION_FILENAME "cde"
#define SAVE_COMMAND_JSONPRETTY false
// #define LOADING_RAW_DATA true
#define ENABLED_MEMORY_REDUCTION true           // True: ne charge pas toutes les commandes au demarrage mais seulement à la demande.

// --- Packets handler ---
#define DEFAULT_CHUNK_SIZE 20                   // Don't forget to change also in client BLE
