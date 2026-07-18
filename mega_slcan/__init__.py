#!/usr/bin/env python3
import argparse
import can


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

    bus = can.Bus(interface="slcan", channel=args.device, bitrate=args.bitrate)

    if filters:
        bus.set_filters(filters)

    for msg in bus:
        print(f"{msg.arbitration_id:03X} [{msg.dlc}] {msg.data.hex().upper()}")


if __name__ == "__main__":
    main()
