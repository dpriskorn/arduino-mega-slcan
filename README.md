# MegaSLCAN

Arduino Mega + MCP2515 som SLCAN USB-adapter för CAN-sniffning.

## Hardware

- Arduino Mega
- MCP2515 CAN-modul
- Pins: CS=53, IRQ=2, MOSI=51, MISO=50, SCK=52

## Software

Installera med uv:
```bash
uv sync
```

## Användning

1. Ladda upp MegaSLCAN.ino till Arduino Mega
2. Anslut till bilens CAN-buss (H=H, L=L)
3. Kör sniffern:

```bash
uv run mega-slcan-sniff
uv run mega-slcan-sniff -d /dev/ttyACM1
uv run mega-slcan-sniff -f 0x123 -f 0x456
```

## SLCAN kommandon

| Kommando | Beskrivning |
|----------|-------------|
| `O` | Öppna CAN |
| `C` | Stäng CAN |
| `S` | ignoreras (hardkodad 500k) |
| `F` | Returnerar status |

## CAN frames

Ta emot: `t<ID><DLC><DATA>\r`
Skicka: `t<ID><DLC><DATA>\r`

## Exempel

```python
# Filtrera på specifik ID
for msg in bus:
    if msg.arbitration_id == 0x123:
        print(msg)

# Skicka frame
msg = can.Message(arbitration_id=0x200, data=[1,2,3,4], is_extended_id=False)
bus.send(msg)
```
