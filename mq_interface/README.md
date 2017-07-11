# MQTT Agent
## Dependencies

- [Lib MiniUPnPClient](https://github.com/miniupnp/miniupnp/tree/master/miniupnpc)
- [Lib POSIX Realtime Extension](https://docs.oracle.com/cd/E86824_01/html/E54772/librt-3lib.html)
- [Lib cJSON](https://github.com/DaveGamble/cJSON)

## Documentation

To genrate documentation files let run one of the follwing commands:

    doxygen Doxyfile
or

    make doc

## How to communicate with this process
Client code need send json string in the following format

    {
        "pid": 123456,
        "data": {
            "enable": true,
            "rules": [
                {
                    "eport": "9999",
                    "iport": "9999",
                    "proto": "UDP"
                },
                {
                    "eport": "6666",
                    "iport": "6666",
                    "proto": "TCP"
                }
            ]
        }
    }
