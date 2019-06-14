ZeroMQ protocol for PIX0MQ communications
=====================

Commands and acknowledgements are sent via a REQ-REP socket pair. Data
is broadcast via a PUB socket on the board and received via SUB.

Commands
--------

### Packets

Packets can be sent to LArPix with a REQ command:

```
SNDWORD 0x<16-hex-digit word> <IO Chain ID>
```

As an example:

```
SNDWORD 0x0020000000000003 0
```

sends the command to read configuration register 0.

The control board will reply with either `OK` or `ERR`.

### Clock

The clock frequency can be set with the command:

```
SETFREQ 0x<frequency>
```

Frequency is the hex representation of the frequency in kHz. The minimum
frequency is 5 MHz (0x1388). The maximum frequency is 25 MHz (0x61a8).

Response is `OK` or `ERR`.

### Test pulse divider

Set the frequency of the onboard test pulse LEMO (TST).

```
SETFTST 0x<div>
```

The new frequency will be equal to the clock frequency (CLK2X) divided
by the `div` value specified in the command.

Response is `OK` or `ERR`.

### Low-level stats

Retrieves the number of start and stop bits received since the last
system reset (the numbers should be
equal and represent the number of packets).

```
GETSTAT <channel>
```

Response is `<starts> <stops>`

### System reset

Send the reset signal to LArPix.

```
SYRESET
```

Response is `OK` or `ERR`.

### Ping

Ask system if it is still alive

```
PING_HB
```

Response is `OK` if all is good. Any other response indicates an error.
