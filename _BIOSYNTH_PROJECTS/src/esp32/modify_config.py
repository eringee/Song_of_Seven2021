import json
import os

# Path to your config.json (relative to project root)
CONFIG_PATH = "src/esp32/data/config.json"

def main():
    # Read existing config
    with open(CONFIG_PATH, 'r') as f:
        config = json.load(f)
    
    # Update with build flag values (using env variables)
    wifi_ssid = os.getenv("PIO_BUILD_FLAGS")
    # PIO_BUILD_FLAGS contains all flags in one string, parse from it:
    # Example: '-D WIFI_SSID="MyNewSSID" -D WIFI_PASS="MyNewPass"'
    # Let's extract values more reliably:

    import re

    def get_define_value(flag_name):
        pattern = rf'-D\s+{flag_name}=(\"[^\"]+\"|\'[^\']+\'|\S+)'
        match = re.search(pattern, wifi_ssid)
        if match:
            val = match.group(1)
            # strip quotes if any
            if val.startswith('"') or val.startswith("'"):
                val = val[1:-1]
            return val
        return None

    new_ssid = get_define_value("WIFI_SSID")
    new_pass = get_define_value("WIFI_PSK")
    new_ip= get_define_value("IP")

    if new_ssid:
        config['wifiSSID'] = new_ssid
    if new_pass:
        config['wifiPSK'] = new_pass
    if new_ip:
        config['oscIP1'] = new_ip

    # Save back
    with open(CONFIG_PATH, 'w') as f:
        json.dump(config, f, indent=4)

if __name__ == "__main__":
    main()
