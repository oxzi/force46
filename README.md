# force46

Enforce a program to either use IPv4 or IPv6 with this one weird trick.
Actually it is just a `LD_PRELOAD`-based script.


## Usage

```
$ make

# Overwrites the Internet Protocol to be used.
$ force4 curl icanhazip.com
123.123.123.123
$ force6 curl icanhazip.com
fd23::42

# Even overwrites if the Internet Protocol's version is specified within the tool.
$ force4 curl -6 icanhazip.com
123.123.123.123
$ force6 curl -4 icanhazip.com
fd23::42
```


## Disclaimer

I cannot promise that every connection will be upgraded to the desired protocol.
Also, some programs can or will fail!


## License

GNU GPL v3 or later.
