# Service 01 PID 01 - Monitor status since DTCs cleared

A request for this PID returns 4 bytes of data, labeled A, B, C and D.

The first byte (A) contains two pieces of information. Bit A7 (MSB of byte A) indicates whether or not the MIL (malfunction indicator light, aka. check engine light) is illuminated. Bits A6 through A0 represent the number of diagnostic trouble codes currently flagged in the ECU.

The second, third, and fourth bytes (B, C and D) give information about the availability and completeness of certain on-board tests ("OBD readiness checks"). The third and fourth bytes are to be interpreted differently depending upon whether the engine is spark ignition (e.g. Otto or Wankel engines) or compression ignition (e.g. Diesel engines). In the second byte (B), bit 3 indicates the engine type and thus how to interpret bytes C and D, with 0 being spark (Otto or Wankel) and 1 (set) being compression (Diesel). Bits B6 to B4 and B2 to B0 are used for information about tests that not engine-type specific, and thus termed *common* tests. Note that for bits indicating test **availability** a bit set to 1 indicates available, whilst for bits indicating test **completeness** a bit set to 0 indicates complete.

| Bits | Definition |
| :--- | :--- |
| A7 | State of the CEL/MIL (on/off). |
| A6-A0 | Number of confirmed emissions-related DTCs available for display. |
| B7 | Reserved (should be 0) |
| B6-B4 | Bitmap indicating completeness of *common* tests. |
| B3 | Indication of engine type<br>0 = Spark ignition (e.g. Otto or Wankel engines)<br>1 = Compression ignition (e.g. Diesel engines) |
| B2-B0 | Bitmap indicating availability of *common* tests. |
| C7-C0 | Bitmap indicating availability of engine-type specific tests. |
| D7-D0 | Bitmap indicating completeness of engine-type specific tests. |

Bits from byte B representing *common* test indicators (those not engine-type specific) are mapped as follows:

| | Test availability | Test completeness |
| :--- | :--- | :--- |
| **Components** | B2 | B6 |
| **Fuel System** | B1 | B5 |
| **Misfire** | B0 | B4 |

Bytes C and D are mapped as follows for spark ignition engine types (e.g. Otto or Wankel engines):

| | Test availability | Test completeness |
| :--- | :--- | :--- |
| **EGR and/or VVT System** | C7 | D7 |
| **Oxygen Sensor Heater** | C6 | D6 |
| **Oxygen Sensor** | C5 | D5 |
| **Gasoline Particulate Filter**[a] | C4 | D4 |
| **Secondary Air System** | C3 | D3 |
| **Evaporative System** | C2 | D2 |
| **Heated Catalyst** | C1 | D1 |
| **Catalyst** | C0 | D0 |

Bytes C and D are alternatively mapped as follows for compression ignition engine types (Diesel engines):

| | Test availability | Test completeness |
| :--- | :--- | :--- |
| **EGR and/or VVT System** | C7 | D7 |
| **PM filter monitoring** | C6 | D6 |
| **Exhaust Gas Sensor** | C5 | D5 |
| **- Reserved -** | C4 | D4 |
| **Boost Pressure** | C3 | D3 |
| **- Reserved -** | C2 | D2 |
| **NOx/SCR Monitor** | C1 | D1 |
| **NMHC Catalyst**[b] | C0 | D0 |

a. ^ A common misconception is that C4/D4 was A/C Refrigerant, however it had been listed as Reserved in J1979 for years, and was recently defined as GPF.

b. ^ NMHC *may* stand for Non-Methane HydroCarbons, but J1979 does not enlighten us. The translation would be the ammonia sensor in the SCR catalyst.

## Source Attribution

[OBD-II PIDs] (https://en.wikipedia.org/w/index.php?title=OBD-II_PIDs&oldid=1337528554)