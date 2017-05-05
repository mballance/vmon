

The VMon package implements a framework for a test harness to 
interact with software running on an SoC. VMon supports both
bi-directional interaction and streaming-style interation,
which are useful at different times.

The communication mechanisms between SoC and the external world
differ and have different tradeoffs according to the verification
platform. High-speed environments, such as a prototype, can often
use an existing communications interface, such as a UART. Relative 
to the amount of information being communicated, a serial interface
imposes a relatively low overhead. 
In a simulation (and maybe even an emulation) environment, a serial
interface imposes too high an overhead on communication. There are
two primary approaches that can be employed to address this:
- Special-purpose hardware can be added to the design to provide a 
simulation-friendly fast communication path between software and
the test harness.
- A fast path from software to test harness can be introduced
to improve one half of the communication -- typically by adding
a passive bus monitor.

These approaches can be combined:
- Always perform initialization via an always-available path, such 
as the UART.
- Once initialization is complete, switch communication in one
or both directions to a faster path.

This puts the test harness (which has the most awareness of
the characteristics of the environemnt) in control of the 
communication mechanism(s).

# Message format
VMon receives and sends information as a byte stream. 
VMon recognizes two basic message formats: variable-
and fixed-length messages. Fixed-length messages have
less header overhead than variable-length messages.

All messages start with a marker byte.

All target harness to SoC messages are acknowledged. 

- Single-byte messages

-- Ping --
  The ping message is used to detect that the target is alive 
  and ready to respond to other messages. The client may send
  this message at any time.
  Message:  0xEA
  Response: 0xE5
  
  All remaining messages start with a header byte of 0xE8

- Fixed-length messages

  Fixed-length messages can be 2, 4, 8, or 16 bytes in length.
  (2-byte overhead)
  Message: 0xA5
  [7:3] EP ID
  [2:1] length (0=2, 1=4, 2=8, 3=16)
  [0]   header parity
  Response: 0xE0 (OK)
            0xE1 (Error)
            
- Variable-length messages
  (5-byte overhead)
  Message: 0xA6
    - [7:3] EP ID
    - [0]   header and length parity
    - Length (16 bits)
    - Payload (N bytes)
    - Checksum (1 byte)

# Endpoints
- Assume 32 endpoints (including 0). 1 for maintenance, and 31 for users.

Communication between software and test harness is performed 
in terms of endpoints. The software and test harness are 
expected to have a matching view of endpoints. 

# Endpoint 0
VMon reserves Endpoint0 for predefined messages. Endpoint0 
messages:
-

Bi-directional communication is often used when initializing
the SoC and establishing synchronization:
-> On boot, the SoC must wait for a connection from the harness
-> The harness will often upload software to the SoC
-> Finally, the harness will typically launch the test program

The bi-directional link between test harness and SoC is typically
very slow in simulation -- possibly a UART or JTAG port. Consequently,
especially in simulation, bi-directional communication will be minimized.

