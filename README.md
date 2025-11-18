# Filesystem Usage Analyzer

A terminal-based interactive tool for analyzing disk space usage in directories. Navigate through your filesystem and see which directories are taking up the most space, displayed in a clean ncurses interface.

## Features

- **Interactive Directory Navigation**: Browse through directories using keyboard controls
- **Recursive Size Calculation**: Automatically calculates the total size of each directory including all subdirectories
- **Human-Readable Sizes**: Displays file sizes in human-readable format (e.g., "12.3 MiB", "1.2 GiB")
- **Sorted Display**: Directories are sorted by size (largest first) for easy identification of space hogs
- **Safe Navigation**: Skips symlinks to avoid infinite loops

## Requirements

- GCC compiler
- ncurses library
- POSIX-compliant system (Linux, macOS, etc.)

## Building

Build the project using the provided Makefile:

```bash
make
```

This will create the executable `fs-usage-analyzer`.

## Usage

Run the program with an optional starting directory path:

```bash
./fs-usage-analyzer [directory]
```

If no directory is specified, it defaults to the current directory (`.`).

### Controls

- **Arrow Keys**: Navigate up/down through the directory list
- **Enter**: Enter the selected directory
- **Backspace/Left Arrow**: Go up to the parent directory
- **q**: Quit the application

## Project Structure

```
src/
├── main.c      - Entry point, handles command-line arguments
├── scanner.c   - Directory scanning and size calculation logic
├── scanner.h   - Scanner API definitions
├── ui.c        - ncurses-based user interface
└── ui.h        - UI API definitions
```

## Building from Source

The Makefile uses the following flags:
- `-Wall -Wextra -pedantic`: Strict warnings
- `-std=c11`: C11 standard
- `-g`: Debug symbols
- `-lncurses`: Link against ncurses library

To clean build artifacts:

```bash
make clean
```

## License

This project is provided as-is for educational and personal use.

