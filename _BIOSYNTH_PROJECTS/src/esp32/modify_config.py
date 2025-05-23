Import("env")
import json
import os

def get_flag_value(flags, flag_name):
    prefix = f"-D{flag_name}="
    for flag in flags:
        if flag.startswith(prefix):
            val = flag[len(prefix):]
            # Strip outer quotes if present (both normal and escaped)
            if val.startswith('"') and val.endswith('"'):
                val = val[1:-1]
            if val.startswith('\\"') and val.endswith('\\"'):
                val = val[2:-2]
            return val
    return None

def main():
    print(">> Running modify_config.py script")

    build_flags = env["BUILD_FLAGS"]
    # DEBUG print of build flags:
    # print("Build flags:", build_flags)

    wifi_ssid = get_flag_value(build_flags, "WIFI_SSID")
    wifi_psk = get_flag_value(build_flags, "WIFI_PSK")
    wifi_ip = get_flag_value(build_flags, "IP")
    board_id = get_flag_value(build_flags, "BOARD_ID")

    print(f"Got WIFI_SSID = {wifi_ssid}")
    print(f"Got WIFI_PSK = {wifi_psk}")
    print(f"Got IP = {wifi_ip}")
    print(f"Got BOARD_ID = {board_id}")

    # Path to config.json (relative to project root)
    config_path = os.path.join("data", "config.json")

    # Read existing config file
    try:
        with open(config_path, "r") as f:
            config = json.load(f)
    except Exception as e:
        print(f"Error reading config.json: {e}")
        config = {}

    # Update config with build flag values if found
    if wifi_ssid:
        config['wifiSSID'] = wifi_ssid
    if wifi_psk:
        config['wifiPSK'] = wifi_psk
    if wifi_ip:
        config['oscIP1'] = wifi_ip
    if board_id:
        config['id'] = board_id

    # Write updated config back
    try:
        with open(config_path, "w") as f:
            json.dump(config, f, indent=4)
        print(f"Updated {config_path} successfully.")
    except Exception as e:
        print(f"Error writing config.json: {e}")

main()
