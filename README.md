# Simple HTTP Server

Implementation of a simple HTTP server with C++

## Features
- Run on Linux only
- It can handle a huge concurrent connections (> 10k).
- Only support HTTP/1.0 and HTTP/1.1, with basic request methods: GET, POST, PUT, DELETE.
- Support basic HTTP/1.1, persistent connections. So it can serve > 100k request per second on modern personal computer.
- Support basic request and response, request size < 4KB.
- The art of speed is about epoll, refer 63.4.5 of [The Linux Programming Interface book](https://sciencesoftcode.files.wordpress.com/2018/12/the-linux-programming-interface-michael-kerrisk-1.pdf)

![image](https://user-images.githubusercontent.com/13882316/222970282-075dc738-0847-4eab-aff3-fdf179f9b329.png)

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

- On virtual machine with 2 cores and 2GB RAM: It can serve > 20k rps with 10k concurrent connections

```
hiennm@hiennm-virt:~/github/wrk$ ./wrk -t10 -c10000 -d10s http://127.0.0.1:38080/helloworld
Running 10s test @ http://127.0.0.1:38080/helloworld
  10 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    44.05ms    8.59ms 260.41ms   76.68%
    Req/Sec     2.29k     0.88k    4.80k    64.10%
  228376 requests in 10.07s, 63.16MB read
  Socket errors: connect 8989, read 0, write 0, timeout 0
Requests/sec:  22671.06
Transfer/sec:      6.27MB
```

- On the Server with 24 cores and 64GB RAM: It can serve > 200k rps and 10k concurrent connections

```
hiennm@hiennm:~/projects/github/wrk$ ./wrk -t10 -c10000 -d60s http://127.0.0.1:38080/helloworld
Running 1m test @ http://127.0.0.1:38080/helloworld
  10 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.36ms    7.59ms 441.68ms   92.77%
    Req/Sec    21.71k     6.37k   46.40k    66.03%
  12944889 requests in 1.00m, 3.50GB read
  Socket errors: connect 8989, read 0, write 0, timeout 0
Requests/sec: 215437.57
Transfer/sec:     59.58MB
```

## Contributing

Contributions are welcome! Please open an issue or submit a pull request if you would like to contribute.
