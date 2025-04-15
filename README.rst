============================
Wireless Network Transceiver
============================

The name simply serves a descriptive function. Lacking an idea and desire for a concise or catchy term; I just think of it as my radio or *das Funkgerät*.

------------------
What it looks like
------------------

.. image:: docs/device.jpg
  :width: 100%
  :alt: Wireless Network Transceiver bundled into an enclosure with a RaspberryPi Zero

-------------
Why it exists
-------------

- There was no wireless network transceiver which can connect through some hundred meters of dense woodland while reaching at least telephone modem speeds.

.. _driver: https://github.com/markqvist/Reticulum/discussions/421

- IEEE 802.11ah Wi-Fi HaLow under planning since 2017 could solve this and since 2024 there seem to be some devices available but the devices' stability and driver_ support look as bad as early WiFi on Linux.

------------
What it does
------------

**Reliable bidirectional transmission of regular network packets at 56kbit/s over some hundreds of meters bundled as standard USB CDC ECM ethernet device.** *Bringing WiFi back to the 90ies*

Reliable meaning a rate of packetloss well below one percent, because above this mark TCP tends to become unstable. Regular WiFi in a brickwalled apartment for comparison experiences between five and ten percent of packetloss, which gets covered by automatic retransmission. This radio device tested for 10000 pings spread over two hours loses 0.15% of handshakes and thus 0.075% of packets.

**Handling statistical errors**

The radio spectrum as medium is inherently noisy thanks to sources like the cosmic radiation background, the sun as thermal and particle emitter, and human interferences. Combined with the imperfections of technology this manifests as biterrors which can generally occur sparsely distributed called random or densely packed as bursterrors.

We counter this by checksumming and retransmitting on mismatch or by the use of forward error correction derived from Coding Theory. An additional interleaver makes this task easier for the decoder because it spreads bursterrors out into sparse biterrors.

**Preventing systematic errors**

Modern radio transceiver modules are inexpensive because they use intelligently designed circuits to keep track of the signal and recover a useful interpretation. Most common are continuously running control loops namely automatic frequency correction AFC tracking the carrier, automatic gain control AGC steadying the amplitude, and bit clock recovery BCR following the timing of the transmitted symbols.

We encourage the control loops' correct operation by ensuring enough transitions between symbols. This requirement can be reduced to establishing a limitation on the runlength of consecutively equal symbols.

**Delimiting the problem**

My observations using subGHz transceivers showed me that the handling of statistical errors is of lesser importance compared to the prevention of systematic errors.

Receiving a long run of zeroes for example often unlocks the control loop for bit clock recovery which then produces a substantial run of falsely sampled bits.
These bursterrors are hard to compensate and would require computationally expensive error correction codes like Reed-Solomon or LDPC which to my knowledge surpass the capabilities of simple MCUs for my target datarate of one megabit per second.

**Implementing the Math**

Extended binary Golay codes halve the bitrate, transforming 12bit messages into 24bit codewords, and are designed to correct three sometimes four errors per codeword which suffices for the given problem. Furthermore their construction and properties are well studied and understood.

This gives rise to the opportunity of adding runlength limitation into the encoder itself, avoiding the generation of separate pseudo noise sequences, and eliminating the error propagation of scramblers.

The encoder equals the multiplication of message vector and generator matrix which comprises the identity matrix and the complement of the adjacency matrix of the great dodecahedron.
The columns of the matrix were permuted conforming to the constructive principle given in *Transcontrol Codes with Run-Length Limitation* by Farkaš and Weinrichter.
The constant modifier was selected using computerised search.
The decoder was implemented following the deduction in *Error Control Coding* by Lin and Costello.

The resulting encoded bitstream excludes the problematic bytes 0b00000000 and 0b11111111 for binary FSK, as well as 0b01010101 and 0b10101010 for  quaternary FSK. This can be regarded as bandwidth limited baseband modulation providing the desired line coding.

The decoder's sourcecode was optimised using some vectorisation, and processes 3 Mbit/s on the ARM CortexM0 with the core clocked at 48MHz and the flash at 24MHz.

.. image:: docs/matrices.png
  :width: 100%

-------
Caveats
-------

.. _SAMD21: https://www.seeedstudio.com/xiao-series-page

The project is bound to the SAMD21_ microcontroller because it uses its USB controller with builtin DMA as well as its event system for DMA SPI in order to reliably translate network traffic at fullspeed.

.. _math: code_rfm12bp/do3rb.c

I am aware that the used radio transceiver Si4421 known as RFM12B has reached end-of-life and should be replaced by the more capable and faster Si4463. Good that the math_ is already fast enough and implements run-length limited line coding for 2FSK and 4FSK.

========================
Tour around the Firmware
========================

.. _tinyusb: https://github.com/hathach/tinyusb

Simplified treeview showing the structure and possible points of interest also known as things to draw inspiration from, or copy. Notice the integration of tinyusb_ without becoming part of their project environment while keeping updates possible by simple copy from their upstream.

.. image:: docs/firmware.png
  :width: 100%
  :alt: Treeview of firmware parts like the board support package in samd21, the codec under code_rfm12bp/do3rb and the tinyusb integration

=====================
Building the Firmware
=====================

This project uses the classical baremetal C toolchain for embedded ARM including the common build essentials. Flashing needs double-tapping the microcontroller's reset the first time, later the firmware restarts into the bootloader when *load* is entered on the serial prompt, which is automated inside the *flash* target.

.. code:: bash

  pacman -S arm-none-eabi-{binutils,gcc,newlib} make
  git clone github.com/DO3RB/WirelessNetworkTransceiver.git
  cd WirelessNetworkTransceiver/code_rfm12bp
  make all flash

=====================
Building the Hardware
=====================

The microcontroller SAMD21_ and the radio transceiver Si4421 as module RFM12BP get connected using wires or a printed circuit board, needing no further external components. The transceiver RFM12B without amplifier works the same, just leave the pins leading to RXEN and TXEN disconnected.

.. image:: docs/hardware.jpg
  :width: 100%
  :alt: Three views: One of two modules connected by flying wires, one of the PCB layout, and the labeled wiring plan

========================
Timeline with Laudations
========================

.. _`Long Shots`: https://media.ccc.de/v/22C3-1078-en-wifi_long_shots
.. _`TV-Whitespace`: https://media.ccc.de/v/camp2015-6738-freifunk_in_tv-whitespace

- `Long Shots`_ in `TV-Whitespace`_ by downmixing WiFi

.. _core: https://github.com/PaulStoffregen/cores
.. _Teensy: https://www.pjrc.com/teensy/
.. _pitfall: https://forum.pjrc.com/index.php?threads/teensy-3-hard-fault-due-to-sram_l-and-sram_u-boundary.25256/
.. _still: https://forum.pjrc.com/index.php?threads/a-dangerous-teensy-3-6-quirk.45489/
.. _bootcore: https://www.pjrc.com/store/ic_mkl02.html

- Learned ARM Cortex by dismantling the Arduino core_ of Teensy_ 3.1 and subsequently grew to distrust frameworks. Found the cause by myself after an unreasonable amount of time, only to learn later that the pitfall_ is still_ open. The proprietary bootcore_ making the SWD port inaccessible, did not help.

..
  Though i understand the need to combat counterfeits.

.. _HRD70: https://media.ccc.de/v/afu-tm18-1008-hrd70_high_speed_70_cm_daten_transceiver_fuer_hamnet

- HRD70_ using an MCU with integrated transceiver would have been nice

.. _LoRa: https://www.semtech.com/lora
.. _LoRaWAN: https://www.semtech.com/lora/lorawan-standard
.. _`physical layer`: https://media.ccc.de/v/33c3-7945-decoding_the_lora_phy
.. _modulation: https://www.epfl.ch/labs/tcl/wp-content/uploads/2020/02/Reverse_Eng_Report.pdf
.. _Meshtastic: https://meshtastic.org

- Semtech LoRa_'s proprietary `physical layer`_ implementing coded chirp-spectrum modulation_ brings reliability and range into the ISM world promoting subsequent monetary LoRaWAN_ and communal Meshtastic_ applications

.. _NewPacketRadio: https://hackaday.io/project/164092-npr-new-packet-radio
.. _NPR-H: https://hackaday.io/project/175907-npr-h-new-packet-radio-modem
.. _`NPR 3.0`: https://npr.m0ahn.co.uk/
.. _NPR-VSAT: http://f4hdk.free.fr/NPR_VSAT/

- NewPacketRadio_ implementing TDMA coordinated IP traffic via slightly coded Si4463. Original hardware was continued by `NPR-H`_ and currently `NPR 3.0`_.  Meanwhile the first author changed platform to develop a coded evolution `NPR-VSAT`_ for satellite use

.. _M17: https://m17project.org/
.. _replacement: https://spec.m17project.org/pdf/M17_spec.pdf

- M17_ is a nice coded replacement_ for proprietary digital voice modes

.. _Reticulum: https://reticulum.network/
.. _`network stack`: https://markqvist.github.io/Reticulum/manual/
.. _can: https://unsigned.io/rnode/

- Reticulum_ is a nice encrypted `network stack`_ with signatures instead of addresses which can_ use LoRa but is generally medium agnostic

.. _HAMRAN: https://github.com/oevsv/hamran-tools
.. _project: https://hamran.oevsv.at/
.. _RPX-100: https://wiki.oevsv.at/wiki/RPX-100
.. _WRAN: https://wiki.oevsv.at/wiki/WRAN_IEEE802.22_(Super_WIFI)
.. _OFDM: https://repositum.tuwien.at/bitstream/20.500.12708/78121/1/Honek%20Marek%20-%202022%20-%20SDR%20OFDM%20frame%20generation%20according%20to%20IEEE%2080222%20OFDMA...pdf
.. _MAC: https://wiki.oevsv.at/w/nsfr_img_auth.php/4/41/Medium_Access_Control_for_WRAN.pdf

- HAMRAN_ project_ using `RPX-100`_ SBC+SDR for WRAN_ via OFDM_ with MAC_

.. _CATS: https://cats.radio/
.. _modernisation: https://gitlab.scd31.com/cats/cats-standard

- CATS_ is a nice, oxidised, and coded modernisation_ of APRS

.. _SXceiver: https://sxceiver.com
.. _SX1255: https://www.semtech.com/products/wireless-rf/lora-core/sx1255

- SXceiver_ more flexibility and bandwidth using SX1255_ IQ transceiver but missing amplifier

.. _transport: https://github.com/gretel/rns-if-espnow

- fast HDLC and Reticulum transport_ via ESP-NOW is nice

..
  I am an engineer, and this is my manifesto.