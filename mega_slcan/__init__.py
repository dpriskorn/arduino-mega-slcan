#!/usr/bin/env python3
import argparse
import errno
import sys

import can
import serial


def main():
    parser = argparse.ArgumentParser(description="MegaSLCAN CAN sniffer")
    parser.add_argument("--device", "-d", default="/dev/ttyACM0", help="Serial device")
    parser.add_argument("--bitrate", "-b", type=int, default=500000, help="CAN bitrate")
    parser.add_argument("--filter", "-f", action="append", help="Filter by CAN ID (hex)")
    args = parser.parse_args()

    filters = []
    if args.filter:
        for f in args.filter:
            filters.append({"can_id": int(f, 16), "mask": 0x7FF})

    try:
        bus = can.Bus(interface="slcan", channel=args.device, bitrate=args.bitrate)
    except serial.SerialException as e:
        if e.errno == errno.ENOENT:
            print(f"Error: Device '{args.device}' not found. Is the Arduino connected?", file=sys.stderr)
        elif e.errno == errno.EBUSY:
            print(f"Error: Device '{args.device}' is busy. Is another program using it (e.g. arduino-cli monitor)?", file=sys.stderr)
        elif e.errno == errno.EACCES:
            print(f"Error: Permission denied for '{args.device}'. Try adding your user to the 'dialout' group.", file=sys.stderr)
        else:
            print(f"Error: Could not open serial device '{args.device}': {e}", file=sys.stderr)
        sys.exit(1)
    except can.exceptions.CanInterfaceNotImplementedError:
        print("Error: The serial module is not installed. Run: uv pip install pyserial", file=sys.stderr)
        sys.exit(1)
    except can.exceptions.CanInitializationError as e:
        print(f"Error: Failed to initialize CAN interface: {e}", file=sys.stderr)
        print("Is the MCP2515 CAN module connected and working?", file=sys.stderr)
        sys.exit(1)

    if filters:
        bus.set_filters(filters)

    try:
        for msg in bus:
            print(f"{msg.arbitration_id:03X} [{msg.dlc}] {msg.data.hex().upper()}")
    except KeyboardInterrupt:
        pass
    finally:
        bus.shutdown()


if __name__ == "__main__":
    main()
