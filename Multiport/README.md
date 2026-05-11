# Multiport

**Author:** Ushikai  
**Version:** 1.1  
**Platform:** Ashita v4 (FFXI Private Server)

---

## Overview

Multiport is an Ashita v4 plugin that synchronizes homepoint teleports across multiboxed clients. When the main character interacts with a homepoint crystal and confirms a destination, all alt clients automatically teleport to the same location.

---

## Requirements

- Ashita v4
- Multisend plugin
- All clients must have the Multiport plugin loaded

---

## Installation

1. Place `multiport.dll` in your Ashita `plugins` folder
2. Load the plugin on each client: `/load multiport`

---

## Usage

### Basic Usage

1. Load Multiport on all clients
2. Have each alt character **target their local homepoint crystal**
3. On the main character, interact with the homepoint crystal normally and select a destination through the menu
4. All alts will automatically teleport to the same homepoint index

### Commands

| Command | Description |
|---|---|
| `/multiport stop` | Cancels any active teleport sequence and resets all state |
| `/multiport debug` | Toggles debug mode on/off (shows packet and sequence info in chat) |
| `/ms <command>` | Shorthand input |

> **Note:** `/multiport <index>` is reserved for internal use and injected commands only. Main characters should always interact with the crystal manually.

---

## How It Works

1. Main character interacts with homepoint crystal normally
2. Plugin intercepts the outgoing `0x05B` confirm packet
3. Broadcasts `/mso /multiport <index>` to all other clients
4. Each alt receives the command and executes `TeleportToIndex`:
   - Sends `0x001A` (crystal interact) to establish server event state
   - After a staggered delay, sends `0x05B` selection and confirm packets
   - Blocks incoming `0x063` and `0x034` packets to prevent menu from opening
5. Zone confirmation is detected via incoming `0x00A` packet
6. If no zone confirmation is received within the timeout window, the sequence retries automatically (up to 5 attempts)

### Stagger System

To avoid server contention when multiple clients interact simultaneously, each client calculates a unique tick offset based on a hash of their character name. This spreads out crystal interactions to prevent the server from rejecting simultaneous requests.

### Retry Loop

If a teleport attempt is rejected by the server (no zone confirmation within ~5 seconds), the plugin automatically retries up to 5 times. If all attempts fail, an error message is displayed. The sequence can be manually cancelled at any time with `/multiport stop`.

---

## Known Limitations

- **Homepoint validation** is not currently implemented. If an alt does not have the target homepoint unlocked, it will retry 5 times before giving up. Use `/multiport stop` to cancel early if you know the homepoint is not unlocked on a character.
- Alt characters must have a homepoint crystal **targeted** when the main triggers the teleport, or they will not participate.
- The teleport animation is skipped on alt clients as the sequence bypasses the normal menu interaction.

---

## Debug Mode

Toggle with `/multiport debug`. When enabled, the following information is logged to chat:

- Incoming `/multiport` command details and follower state
- Stagger offset and final tick values per character
- Outgoing packet sends (selection and confirm)
- Zone confirmation results and retry attempts
- Homepoint mask values when received from server

---

## File Reference

| File | Purpose |
|---|---|
| `Multiport.h` | Class definition and member variables |
| `main.cpp` | Plugin entry point and initialization |
| `commands.cpp` | Command handling, packet logic, and Direct3D tick loop |
| `HPTable.h` | Homepoint destination table (zone, x, y, z, rotation) |

---

## Acknowledgements

- Packet research: [XiPackets by atom0s](https://github.com/atom0s/XiPackets)
- Plugin framework: Ashita v4 SDK
- Multisend plugin
