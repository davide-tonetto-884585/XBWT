# XBWT Library

The XBWT library is designed to build compressed XBWT (eXtended Burrows-Wheeler Transform) from labeled trees. This library provides efficient algorithms and data structures to handle the creation and compression of the XBWT. The compressed XBWT can be used to navigate the tree and search for patterns in the tree using an efficient and space-efficient representation.

## Features

- Build compressed XBWT from labeled trees in liner time on the number of nodes
- Efficient compressed representation of the XBWT
- Support for various efficient operations on the compressed XBWT for tree navigation and tree search
- Reverse transformation from the compressed XBWT to the original labeled tree

## Requirements

- [sdsl-lite](https://github.com/simongog/sdsl-lite) library is required for this project. Make sure to install it before using the XBWT library.

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/XBWT.git
    ```
2. Install the sdsl-lite library;
3. Build the XBWT library using CMake.

## Usage

To use the XBWT library, include the necessary headers in your project and link against the sdsl-lite library.
