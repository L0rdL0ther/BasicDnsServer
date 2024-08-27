
# Basic Dns Server

It is an extremely simple DNS service. It is not suitable for daily functions. It is just an extremely simple structure that shows how DNS servers work.

Compatible for Linux, I did not test it for Windows, CLion ide was used
## Guide to Dns servers communication types and data types


- [RFC1035](https://datatracker.ietf.org/doc/html/rfc1035)
- [RFC1034](https://datatracker.ietf.org/doc/html/rfc1034)

## Run Locally

Clone the project

```bash
  git clone https://github.com/L0rdL0ther/BasicDnsServer.git
```

Go to the project directory

```bash
  cd BasicDnsServer
```

Build project

```bash
  g++ -o main main.cpp  
```

Start the server

```bash
  ./main
```

For Test

```bash
  nslookup example.com 127.0.0.1
```

