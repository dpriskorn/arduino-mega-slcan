# MegaSLCAN

Arduino Mega + MCP2515 som SLCAN USB-adapter för CAN-sniffning.

## Hardware

- Arduino Mega
- MCP2515 CAN-modul
- Pins: CS=53, IRQ=2, MOSI=51, MISO=50, SCK=52

## Installation

### 1. Arduino

Installera [arduino-cli](https://arduino.github.io/arduino-cli/latest/) och kör:

```bash
# Kompilera
just compile

# Ladda upp
just upload

# Öppna serialmonitor
just monitor
```

### 2. Python

```bash
uv sync
```

## Användning

1. Anslut till bilens CAN-buss (H=H, L=L)
2. Kör sniffern:

```bash
just sniff
```

### Avancerade alternativ

```bash
# Ange annan enhet
just sniff -- -d /dev/ttyACM1

# Filtrera på CAN-ID (hex)
just sniff -- -f 0x123 -f 0x456

# Kör direkt med uv
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
| `V` | Pingar MCP2515 (för internt bruk) |

## CAN frames

Ta emot: `t<ID><DLC><DATA>\r`
Skicka: stöds ej (endast mottagning)

## Exempel

```python
# Filtrera på specifik ID
for msg in bus:
    if msg.arbitration_id == 0x123:
        print(msg)
```
