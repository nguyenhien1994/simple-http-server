# Simple HTTP Server

Implementation of a simple HTTP server with C++

## Features
- Run on Linux only
- It can handle a huge concurrent connections (> 10k).
- Only support HTTP/1.0 and HTTP/1.1, with basic request methods: GET, POST, PUT, DELETE.
- Only support basic HTTP/1.1, **persistent connections**, focus on serving as much RPS as possible.
- Support basic request and response, request size < 4KB.
- The art of speed is about epoll, refer 63.4.5 of [The Linux Programming Interface book](https://sciencesoftcode.files.wordpress.com/2018/12/the-linux-programming-interface-michael-kerrisk-1.pdf)

![image](https://user-images.githubusercontent.com/13882316/222970282-075dc738-0847-4eab-aff3-fdf179f9b329.png)

## Design
TBU

## Clone and build the source code

To install this server, follow these steps:

1. Clone this repository: `git clone git@github.com:nguyenhien1994/simple-http-server.git`
2. Navigate to the repository directory: `cd simple-http-server`
3. Build the source code:
```
mkdir build
cd build
cmake .. && make
```

## Usage

To start the server, run `./HttpServer` from the build directory.

The server will start on port 38080 by default.

Access the demo API via: `localhost:38080/helloworld`

**Note:** you may need to increase the resource limit with `ulimit` before running the server.

## Benchmark
Benchmarking with [wrk](https://github.com/wg/wrk)

- On the Laptop with 8 cores and 16GB RAM: It can serve > 200k rps and 10k concurrent connections

![image](https://user-images.githubusercontent.com/13882316/223301639-cdc661e6-a4dc-4974-9998-a42aef5027b5.png)


## Contributing

Contributions are welcome! Please open an issue or submit a pull request if you would like to contribute.

## TODO:
[] Add basic design
[] Add unittests
[] Support more features HTTP/1.1
[] Support send a receive message with large payload
[]...
