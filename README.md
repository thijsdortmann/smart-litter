# SmartLitter

This project aims to provide software for weighing a cat without its knowledge. It does this by invading the cats privacy while it's using the litter box. Knowing how much your cat weighs and how much waste it has produced has never been so easy.

## Hardware

The software is designed for an ESP32 microcontroller in combination with generic load cells and an HX711 amplifier and ADC. For wiring instructions, check out the project this software was inspired by: https://www.instructables.com/IoT-Cat-Litter-Box-with-ESP32-Arduino-IDE-Thingspe/.

## Configuration

Configuration can be done through the `src/settings.h` file. As this file contains secrets, only an example file is supplied in this repository.

Copy the example file:

```
cp src/settings.example.h src/settings.h
```

Then open the `src/settings.h` file in your editor of choice and follow the instructions provided in the comments.
