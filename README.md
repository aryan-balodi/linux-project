# Filesystem Usage Analyzer

A high-performance, terminal-based interactive tool for analyzing disk space usage across directory hierarchies. Built with C11 and ncurses, this application provides an intuitive, real-time interface for identifying space-consuming directories and navigating through complex filesystem structures.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [User Interface](#user-interface)
- [Technical Details](#technical-details)
- [Project Structure](#project-structure)
- [Building from Source](#building-from-source)
- [Performance Considerations](#performance-considerations)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Overview

The Filesystem Usage Analyzer is a command-line utility designed to help system administrators, developers, and power users quickly identify which directories are consuming the most disk space. Unlike traditional tools like `du`, this application provides an interactive, visual interface that allows users to navigate through directory structures while viewing size information in real-time.

The tool recursively calculates the total size of each directory (including all subdirectories and files) and presents the results in a sorted, easy-to-read format. Directories are automatically sorted by size in descending order, making it immediately apparent where disk space is being utilized.

### Key Advantages

- **Interactive Navigation**: Browse directories without leaving the application
- **Visual Representation**: Bar graphs provide immediate visual feedback on relative sizes
- **Efficient Scanning**: Optimized recursive algorithms minimize I/O operations
- **Safe Operation**: Intelligent symlink handling prevents infinite loops
- **Cross-Platform**: Works on any POSIX-compliant system (Linux, macOS, BSD, etc.)

## Features

### Core Functionality

- **Recursive Size Calculation**: Automatically computes the total size of each directory by recursively traversing all subdirectories and summing file sizes
- **Interactive Directory Navigation**: Seamlessly navigate through the filesystem using keyboard controls without exiting the application
- **Human-Readable Size Display**: Converts raw byte counts into intuitive units (B, KiB, MiB, GiB, TiB) with two decimal precision
- **Automatic Sorting**: Directories are sorted by size in descending order, with largest directories appearing first
- **Visual Bar Graphs**: Proportional bar charts provide immediate visual comparison of directory sizes relative to the largest entry
- **Real-Time Rescanning**: Refresh the current directory view to reflect changes in the filesystem

### Safety Features

- **Symlink Protection**: Symbolic links are automatically skipped to prevent infinite recursion loops
- **Permission Handling**: Gracefully handles directories with insufficient read permissions
- **Path Validation**: Validates and resolves paths to prevent navigation errors
- **Error Recovery**: Robust error handling ensures the application remains stable even when encountering problematic filesystem entries

### User Experience

- **Intuitive Controls**: Simple keyboard shortcuts for all operations
- **Scrollable Interface**: Efficiently handles directories with hundreds or thousands of subdirectories
- **Visual Feedback**: Highlighted selection cursor and reverse video for the active entry
- **Status Information**: Displays current path, total directory size, and entry count
- **Responsive Design**: Adapts to terminal size and provides appropriate scrolling

## Architecture

The application is built using a modular architecture with clear separation of concerns:

### Component Overview

1. **Scanner Module** (`scanner.c`, `scanner.h`): Handles all filesystem operations
   - Directory traversal and recursive size calculation
   - Entry collection and sorting
   - Memory management for directory views
   - Human-readable size formatting

2. **User Interface Module** (`ui.c`, `ui.h`): Manages the ncurses-based terminal interface
   - Screen rendering and layout
   - Keyboard input handling
   - Navigation state management
   - Visual bar graph generation

3. **Main Entry Point** (`main.c`): Application initialization
   - Command-line argument parsing
   - Error handling and exit codes
   - Application lifecycle management

### Data Structures

The application uses two primary data structures:

- **`DirEntry`**: Represents a single directory entry with name, full path, and recursive size
- **`DirView`**: Contains a complete view of a directory, including all subdirectories, total size, and metadata

### Algorithm Design

The recursive size calculation uses depth-first traversal, efficiently computing sizes while minimizing memory overhead. The sorting algorithm uses the standard library's `qsort` function with a custom comparator that sorts by size (descending) and then alphabetically by name for entries of equal size.

## Requirements

### System Requirements

- **Operating System**: Any POSIX-compliant system
  - Linux (all distributions)
  - macOS (10.9+)
  - FreeBSD, OpenBSD, NetBSD
  - Other Unix-like systems

### Build Dependencies

- **Compiler**: GCC 4.9+ or Clang 3.5+ with C11 support
- **Build System**: GNU Make (standard on most Unix systems)
- **Libraries**:
  - **ncurses**: Terminal UI library (version 5.0+ recommended)
    - On Debian/Ubuntu: `libncurses5-dev` or `libncurses-dev`
    - On Red Hat/CentOS: `ncurses-devel`
    - On macOS: Included with Xcode Command Line Tools
    - On FreeBSD: `ncurses` (in base system or via ports)

### Runtime Dependencies

- ncurses library (shared library, typically pre-installed on most systems)

## Installation

### Installing ncurses Development Libraries

#### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential libncurses5-dev
```

#### Red Hat/CentOS/Fedora
```bash
sudo yum install gcc make ncurses-devel
# or for newer versions:
sudo dnf install gcc make ncurses-devel
```

#### macOS
```bash
# Install Xcode Command Line Tools (includes ncurses)
xcode-select --install
```

#### FreeBSD
```bash
# ncurses is in base system, but you may need:
pkg install gcc
```

### Building the Application

1. **Clone or download the repository**:
   ```bash
   git clone <repository-url>
   cd balodi_linux
   ```

2. **Build using Make**:
   ```bash
   make
   ```

3. **Verify the build**:
   ```bash
   ./fs-usage-analyzer --help
   # or simply:
   ./fs-usage-analyzer
   ```

4. **Optional: Install system-wide** (requires root):
   ```bash
   sudo cp fs-usage-analyzer /usr/local/bin/
   ```

### Build Options

The Makefile supports the following targets:

- `make` or `make all`: Build the executable (default)
- `make clean`: Remove all build artifacts (object files and executable)

### Compiler Flags

The build system uses the following compiler flags for quality and safety:

- `-Wall -Wextra -pedantic`: Enable comprehensive warning messages
- `-std=c11`: Use the C11 standard for modern C features
- `-g`: Include debug symbols for debugging
- `-lncurses`: Link against the ncurses library

## Usage

### Basic Usage

Launch the application with an optional starting directory:

```bash
./fs-usage-analyzer [directory]
```

If no directory is specified, the application starts in the current working directory (`.`).

### Examples

```bash
# Analyze current directory
./fs-usage-analyzer

# Analyze home directory
./fs-usage-analyzer ~

# Analyze root filesystem (may require permissions)
./fs-usage-analyzer /

# Analyze a specific project directory
./fs-usage-analyzer /path/to/project
```

### Exit Codes

The application returns the following exit codes:

- `0`: Success
- `1`: Error (e.g., cannot scan directory, invalid path)

## User Interface

### Screen Layout

The interface consists of several distinct regions:

1. **Header Bar**: Application title and quick reference for keyboard shortcuts
2. **Path Display**: Current directory being viewed (full absolute path)
3. **Total Size**: Total size of the current directory including all contents
4. **Directory List**: Scrollable list of subdirectories with:
   - Directory name (left-aligned)
   - Size in human-readable format (right-aligned)
   - Proportional bar graph (visual size indicator)
5. **Footer**: Status information showing visible entries and total count

### Keyboard Controls

| Key | Action |
|-----|--------|
| `↑` / `↓` | Navigate up/down through the directory list |
| `Enter` | Enter the selected directory |
| `Backspace` / `←` / `h` | Navigate up to the parent directory |
| `r` / `R` | Rescan the current directory (refresh view) |
| `q` / `Q` | Quit the application |

### Visual Elements

- **Selection Highlight**: The currently selected directory is displayed with reverse video (highlighted background)
- **Bar Graphs**: Horizontal bars (`#`) provide visual representation of relative sizes
- **Scrolling**: When the directory list exceeds the visible area, the view automatically scrolls to keep the selection visible

### Example Session

1. Launch the application: `./fs-usage-analyzer ~/projects`
2. View the list of subdirectories sorted by size
3. Use arrow keys to navigate to a large directory
4. Press Enter to enter that directory
5. Press Backspace to return to the parent directory
6. Press `r` to refresh if files have changed
7. Press `q` to exit

## Technical Details

### Filesystem Traversal

The application uses `opendir()` and `readdir()` for directory traversal, with `lstat()` to obtain file metadata. The recursive algorithm:

1. Opens the target directory
2. Iterates through all entries
3. For directories: recursively calculates size
4. For regular files: adds file size directly
5. Skips symbolic links to prevent loops
6. Handles permission errors gracefully

### Memory Management

- Dynamic memory allocation using `malloc()` and `realloc()`
- Automatic cleanup on directory view changes
- Proper deallocation of all resources on exit
- Memory-efficient growth strategy (doubling capacity)

### Size Calculation

Sizes are calculated using `off_t` type, which supports large files (typically 64-bit on modern systems). The recursive calculation includes:

- All regular files in the directory
- All subdirectories (recursively)
- All nested files and directories

### Human-Readable Format

Size conversion uses binary units (1024-based):
- Bytes (B) for sizes < 1024
- Kibibytes (KiB) for sizes < 1024²
- Mebibytes (MiB) for sizes < 1024³
- Gibibytes (GiB) for sizes < 1024⁴
- Tebibytes (TiB) for sizes ≥ 1024⁴

Display format: `"%.2f %s"` (e.g., "12.34 MiB", "1.23 GiB")

### Sorting Algorithm

Directories are sorted using `qsort()` with a custom comparator:
1. Primary sort: Size (descending - largest first)
2. Secondary sort: Name (ascending - alphabetical)

This ensures the largest directories appear first, with consistent ordering for directories of equal size.

## Project Structure

```
balodi_linux/
├── README.md           # This file
├── makefile            # Build configuration
├── src/
│   ├── main.c         # Application entry point
│   ├── scanner.c       # Filesystem scanning implementation
│   ├── scanner.h       # Scanner API definitions
│   ├── ui.c            # User interface implementation
│   └── ui.h            # UI API definitions
└── fs-usage-analyzer   # Compiled executable (after build)
```

### Source File Descriptions

- **`main.c`**: Handles command-line arguments, initializes the UI, and manages application lifecycle
- **`scanner.c`**: Implements directory scanning, recursive size calculation, entry sorting, and size formatting
- **`scanner.h`**: Defines the `DirEntry` and `DirView` structures and public API functions
- **`ui.c`**: Implements the ncurses interface, including rendering, input handling, and navigation logic
- **`ui.h`**: Declares the public UI function `ui_run()`

## Building from Source

### Standard Build

```bash
make
```

This compiles all source files and links them into the `fs-usage-analyzer` executable.

### Clean Build

To remove all build artifacts:

```bash
make clean
```

### Manual Compilation

If you prefer to compile manually:

```bash
gcc -Wall -Wextra -pedantic -std=c11 -g \
    -o fs-usage-analyzer \
    src/main.c src/ui.c src/scanner.c \
    -lncurses
```

### Debug Build

The default build includes debug symbols (`-g`). For additional debugging information, you can add:

```bash
make CFLAGS="-Wall -Wextra -pedantic -std=c11 -g -O0 -DDEBUG"
```

### Release Build

For optimized release builds:

```bash
make CFLAGS="-Wall -Wextra -pedantic -std=c11 -O2 -DNDEBUG"
```

## Performance Considerations

### Scanning Performance

- **First Scan**: Initial directory scan may take time for large directory trees
- **Recursive Calculation**: Each subdirectory requires a full recursive traversal
- **I/O Bound**: Performance is primarily limited by filesystem I/O speed
- **Memory Usage**: Memory consumption scales with the number of subdirectories

### Optimization Strategies

1. **Start Small**: Begin analysis in specific subdirectories rather than root
2. **Use Rescan Sparingly**: The `r` key rescans the entire directory
3. **SSD vs HDD**: Performance is significantly better on solid-state drives
4. **Network Filesystems**: May be slower on NFS, CIFS, or other network mounts

### Scalability

The application handles:
- Directories with thousands of subdirectories
- Deep directory hierarchies (hundreds of levels)
- Large files (multi-terabyte support)
- Mixed filesystem types

### Known Limitations

- Very large directory trees (100,000+ entries) may take considerable time to scan
- Network filesystems may experience significant latency
- Some special filesystem types may not be fully supported

## Troubleshooting

### Common Issues

#### Build Errors

**Problem**: `fatal error: ncurses.h: No such file or directory`

**Solution**: Install ncurses development headers:
```bash
# Debian/Ubuntu
sudo apt-get install libncurses5-dev

# Red Hat/CentOS
sudo yum install ncurses-devel
```

**Problem**: `undefined reference to 'initscr'`

**Solution**: Ensure `-lncurses` is included in the linker flags. Check the Makefile.

#### Runtime Errors

**Problem**: `Error: could not scan directory`

**Possible Causes**:
- Insufficient read permissions
- Directory does not exist
- Path is invalid or contains special characters

**Solution**: 
- Check permissions: `ls -ld /path/to/directory`
- Verify path exists: `test -d /path/to/directory && echo "exists"`
- Try with a different directory

**Problem**: Application appears frozen during scanning

**Solution**: This is normal for large directories. The application is calculating sizes recursively. Wait for the scan to complete.

#### Display Issues

**Problem**: Interface appears corrupted or misaligned

**Solution**:
- Ensure terminal supports ncurses
- Try resizing terminal window
- Check terminal emulator compatibility

**Problem**: Colors or highlighting not working

**Solution**: Some terminal emulators have limited ncurses support. The application should still function with basic text display.

### Getting Help

If you encounter issues not covered here:

1. Verify all dependencies are installed
2. Check that you have read permissions for the target directory
3. Try running with a simple directory first (e.g., `./fs-usage-analyzer .`)
4. Review error messages for specific details

## Contributing

Contributions are welcome! Areas for potential improvement include:

- Performance optimizations for large directory trees
- Additional keyboard shortcuts and features
- Configuration file support
- Color themes and customization
- Export functionality (save results to file)
- Filtering and search capabilities
- Multi-threaded scanning for improved performance

### Development Guidelines

- Follow existing code style and formatting
- Maintain compatibility with POSIX systems
- Test on multiple platforms when possible
- Update documentation for new features
- Ensure memory safety and proper error handling

## License

This project is provided as-is for educational and personal use. Please refer to the license file (if present) for specific terms and conditions.

## Acknowledgments

Built with:
- **ncurses**: Terminal UI library
- **GNU Make**: Build system
- **GCC/Clang**: C compilers

---

**Note**: This tool is designed for interactive use. For automated scripts or batch processing, consider using traditional tools like `du` or `find` with appropriate flags.
