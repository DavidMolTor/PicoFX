# PICO FX

A simple stereo guitar effects platform using an RP Pico 2 development board.

## Introduction

This module is designed to be used as a development platform for guitar effects running on the RP2350 architecture.

The module contains a CS4272 codec with stereo inputs and outputs. It also includes a set of clean buffers to match the guitar and other effects in the chain.

All the power regulation is done through LDO regulators that prioritize low ripple in exchange for worse efficiency.

It is designed to work with 9V intput voltage. It also has a set of headers for it to be able to interface with a control board in the future.

<p align="center"><img src="/Build/PicoFX.png" alt="PICO FX"></p>

## Features

 - Clear separation between digital and analog sections.

 - Steareo input and output.

 - Clean voltage delivery.

 - Flexible interface for controls.

 - Easy future development.

 - Afforable BOM.
