
# Multithreaded-Webserver
Multithreaded Webserver that handles concurrent GET Requests using a custom scheduling policy.

## Directory Structure
```
Multithreaded-Webserver/
├── src/
│   ├── main.c
│   ├── server.c
│   ├── utils.c
│   └── ...
├── build/
│   └── Makefile
├── config/
│   └── server.conf
├── docs/
│   ├── README.md
│   ├── usage.md
│   └── ...
├── static/
│   └── index.html
├── tests/
│   ├── test_server.c
│   ├── test_utils.c
│   └── ...
└── README.md
```

## Description
This project is a multithreaded webserver that can handle multiple concurrent GET requests using a custom scheduling policy. The server is implemented in C and includes various features to manage and serve requests efficiently.

## Getting Started
### Prerequisites
- GCC compiler
- Make

### Building the Project
Navigate to the `build` directory and run `make` to build the project:
```bash
cd build
make
```

### Configuration
Configuration files are stored in the `config` directory. Modify `server.conf` to change the server settings.

### Running the Server
After building the project, you can run the server executable from the `src` directory:
```bash
cd src
./server [] [] []
```

### Static Files
HTML files are located in the `static` directory. These files will be served by the webserver in response to GET requests.

## Documentation
Additional documentation is available in the `docs` directory. This includes detailed usage instructions and project documentation.

## Contributing
Contributions are welcome. Please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
