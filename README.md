# modular-link
Ableton link on Raspberry Pi

# Build
```
    make
```

## Install
```
    sudo cp modlink.service /etc/systemd/system/
    sudo chmod u-w-x /etc/systemd/system/modlink.service
    sudo systemctl enable modlink
```

## Start /Stop
```
    sudo systemctl start modlink
```

```
    sudo systemctl stop modlink
```
