# Windows Activator

A desktop application for Windows operating system activation, built with C++ and featuring a modern graphical user interface. This tool provides an automated solution for activating various Windows editions through a clean and intuitive interface.

## Overview

Windows Activator is a native Windows application that streamlines the process of activating Windows operating systems. The application automatically detects the installed Windows version and edition, providing appropriate activation keys and handling the activation process through Windows' built-in Software Licensing Management tool (slmgr.vbs).

The application features a modern, borderless window interface with smooth animations, custom theming, and an organized workflow that guides users through the activation process with clear visual feedback.

## Key Features

### Automatic OS Detection
- Detects Windows version (Windows 10 or Windows 11) by reading system registry values
- Identifies the specific Windows edition (Home, Pro, Enterprise, Education, etc.)
- Automatically pre-selects the appropriate edition in the user interface
- Verifies current activation status on startup using WMI queries

### Comprehensive Edition Support
The application supports activation for 18 different Windows editions:
- Windows Home (Standard, N, Single Language, Country Specific)
- Windows Pro (Standard, N, for Workstations, for Workstations N)
- Windows Education (Pro Education, Pro Education N, Education, Education N)
- Windows Enterprise (Standard, N, G, G N, LTSC 2019, N LTSC 2019)

### Modern User Interface
- Borderless window design with rounded corners
- Custom dark theme with cyan accent colors
- Smooth slide and fade animations for edition selection
- Interactive navigation buttons with hover effects
- Visual status indicators including:
  - Animated loading spinner during activation
  - Success checkmark upon completion
  - Real-time status text updates
- Draggable window for flexible positioning
- Embedded custom font (Insight) for consistent typography

### Activation Workflow
1. **License Status Check**: Automatically verifies if Windows is already activated
2. **Product Key Installation**: Installs the Generic Volume License Key (GVLK) for the selected edition
3. **KMS Server Configuration**: Connects to a KMS (Key Management Service) server
4. **Activation Execution**: Performs the actual activation through Windows licensing tools
5. **Status Reporting**: Provides clear feedback at each step of the process

### Technical Implementation
- **Native Win32 API**: Direct Windows API integration for window management and system calls
- **DirectX 11 Rendering**: Hardware-accelerated graphics rendering for smooth UI performance
- **Dear ImGui Framework**: Immediate mode GUI library for flexible interface design
- **Multi-threading**: Background activation process to prevent UI freezing
- **Mutex-based State Management**: Thread-safe state synchronization between UI and activation threads

## Project Structure

```
Windows Activator/
├── Windows Activator/           # Main project directory
│   ├── src/                     # Source code
│   │   ├── Main.cpp             # Application entry point, window creation, DirectX setup
│   │   ├── core/                # Core functionality modules
│   │   │   ├── Activator.cpp/h  # Activation process logic
│   │   │   ├── OSDetector.cpp/h # OS version and edition detection
│   │   │   ├── ImageLoader.cpp/h# Texture loading utilities
│   │   │   └── AppState.h       # Global application state structure
│   │   └── ui/                  # User interface components
│   │       ├── MainWindow.cpp/h # Main UI rendering logic
│   │       ├── Theme.cpp/h      # ImGui theme configuration
│   │       ├── Font.h           # Embedded font data
│   │       └── Icons/           # Embedded icon resources
│   │           ├── ArrowLeft.h
│   │           ├── ArrowRight.h
│   │           └── Close.h
│   ├── vendor/                  # Third-party libraries
│   │   ├── imgui/               # Dear ImGui library
│   │   └── stb/                 # STB image loader
│   ├── Windows Activator.vcxproj# Visual Studio project file
│   └── resource.h               # Resource definitions
├── ic_logo.ico                  # Application icon
└── Windows Activator.slnx       # Visual Studio solution file
```

## Technical Architecture

### Core Components

#### Main Application (`Main.cpp`)
- Initializes Win32 window with `WS_POPUP` style for borderless design
- Sets up DirectX 11 device, swap chain, and render target
- Configures ImGui context and loads embedded resources
- Implements main render loop with V-Sync enabled
- Handles window messages including custom title bar dragging

#### Activator Module (`Activator.cpp`)
Manages the Windows activation process through three sequential steps:
1. Executes `slmgr.vbs /ipk [KEY]` to install the product key
2. Executes `slmgr.vbs /skms kms8.msguides.com` to set KMS server
3. Executes `slmgr.vbs /ato` to activate Windows

All processes run hidden using `CREATE_NO_WINDOW` flag to prevent console windows from appearing. Status updates are synchronized using mutex locks to ensure thread safety.

#### OS Detector (`OSDetector.cpp`)
- Queries `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion` registry key
- Reads `CurrentBuild` value (≥22000 indicates Windows 11)
- Reads `EditionID` value to determine specific Windows edition
- Uses PowerShell WMI query to check activation status: `Get-WmiObject SoftwareLicensingProduct`
- LicenseStatus value of `1` indicates activated system

#### Main Window (`MainWindow.cpp`)
- Renders full-screen ImGui window with custom styling
- Implements carousel-style edition selector with animated transitions
- Manages UI state including button opacity and status text visibility
- Draws custom spinner animation during activation
- Renders success checkmark using ImGui draw primitives
- Includes clickable author attribution with web links

#### Image Loader (`ImageLoader.cpp`)
Utilizes STB image library to load embedded PNG images from memory and convert them to DirectX 11 shader resource views for rendering in ImGui.

### Dependencies

**Core Libraries:**
- **DirectX 11** - Graphics rendering and hardware acceleration
- **Dear ImGui v1.89+** - Immediate mode GUI framework
- **STB Image** - Image loading from memory

**Windows APIs:**
- Win32 API - Window management and system integration
- Windows Registry API - OS detection
- Process API - Executing activation commands
- WMI (via PowerShell) - License status verification

**Build Requirements:**
- Visual Studio 2022 or later
- Windows 10 SDK (version 10.0 or higher)
- C++20 Standard (configured in project settings)
- Platform Toolset v143

## Installation and Build Instructions

### Prerequisites
1. Windows 10 or Windows 11 operating system
2. Visual Studio 2022 with the following workloads:
   - Desktop development with C++
   - Windows 10 SDK

### Building from Source

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd "Windows Activator"
   ```

2. **Open the solution:**
   - Launch Visual Studio 2022
   - Open `Windows Activator.slnx`

3. **Select build configuration:**
   - For development: Select `Debug` configuration
   - For release: Select `Release` configuration
   - Platform: `x64` (recommended) or `x86`

4. **Build the project:**
   - Press `F7` or navigate to `Build -> Build Solution`
   - The executable will be generated in:
     - Debug: `x64/Debug/Windows Activator.exe`
     - Release: `x64/Release/Windows Activator.exe`

### Release Configuration Details
The Release build is optimized with:
- Multi-threaded static runtime library (`/MT`)
- Function-level linking and intrinsic functions enabled
- Subsystem: Windows (no console window)
- UAC Execution Level: `RequireAdministrator`

## Usage Instructions

### Running the Application

1. **Launch as Administrator:**
   - The application requires administrator privileges to modify Windows activation settings
   - Right-click `Windows Activator.exe` and select "Run as administrator"
   - The Release build automatically requests UAC elevation

2. **Review Detected OS:**
   - Upon launch, the application displays the detected Windows edition
   - The status text shows current activation state

3. **Select Edition (if needed):**
   - Use the left/right arrow buttons to navigate through available editions
   - The application pre-selects the detected edition

4. **Activate Windows:**
   - Click the green "ACTIVATE" button
   - The button is only enabled when the correct OS edition is selected
   - Wait for the activation process to complete (typically 30-60 seconds)

5. **Verify Success:**
   - A green checkmark appears upon successful activation
   - Status text confirms "Windows Activated Successfully"

### Important Notes

- The application must be run with administrator privileges
- An active internet connection is required for KMS server communication
- The activation uses Generic Volume License Keys (GVLKs) provided by Microsoft
- KMS activation typically expires after 180 days and requires reactivation

### Troubleshooting

**Application won't start:**
- Ensure you're running as administrator
- Verify DirectX 11 runtime is installed
- Check Windows version compatibility (Windows 10/11 required)

**Activation fails:**
- Verify internet connection is active
- Ensure Windows Firewall allows outbound connections
- Try running the activation process again
- Check if antivirus software is blocking the application

**UI appears blank or corrupted:**
- Update graphics drivers
- Ensure DirectX 11 is properly installed
- Try running in compatibility mode

## Technical Specifications

### System Requirements
- **Operating System**: Windows 10 (Build 10240+) or Windows 11
- **Architecture**: x64 or x86
- **Graphics**: DirectX 11 compatible GPU
- **Memory**: 50 MB RAM minimum
- **Disk Space**: 5 MB
- **Privileges**: Administrator access required
- **Network**: Internet connection required for activation

### Performance Characteristics
- Startup time: < 1 second
- Memory footprint: ~30-40 MB
- CPU usage: < 1% (idle), 5-10% (during activation)
- Render loop: 60 FPS (V-Sync enabled)

### Security Considerations
- All activation commands are executed through official Windows tools (slmgr.vbs)
- No modification of system files or registry beyond standard activation procedures
- Uses publicly available Generic Volume License Keys (GVLKs) from Microsoft documentation
- Requires explicit administrator privileges (UAC prompt)

## Development Information

### Code Style and Standards
- C++20 standard compliance
- RAII principles for resource management
- Mutex-based thread synchronization
- Unicode character set (UNICODE/_UNICODE defined)

### Adding New Windows Editions
To add support for additional Windows editions:

1. Open `src/core/OSDetector.cpp`
2. Add the new edition to the `g_state.editions` vector with format:
   ```cpp
   { prefix + "Edition Name", "PRODUCT-KEY-XXXXX-XXXXX" }
   ```
3. Add corresponding registry EditionID mapping:
   ```cpp
   else if (currentEditionID == "RegistryEditionID") g_state.detectedOSIndex = INDEX;
   ```

### Modifying UI Theme
Theme customization is located in `src/ui/Theme.cpp`. The `ApplyBeautifulTheme()` function configures:
- Window rounding and border properties
- Color scheme for various UI elements
- Default style parameters

### Debugging
Debug builds include:
- Console subsystem for debugging output
- Full debug symbol generation
- SDL security checks enabled

## Credits and Attribution

**Author:** Saad Ahmed  
**Website:** [saadahmedev.com](https://saadahmedev.com)  
**GitHub:** [github.com/saadahmedscse](https://github.com/saadahmedscse)

**Third-Party Libraries:**
- [Dear ImGui](https://github.com/ocornut/imgui) - MIT License
- [STB Image](https://github.com/nothings/stb) - Public Domain

## Disclaimer

This application is provided for educational and legitimate use purposes only. Users are responsible for ensuring compliance with Microsoft's licensing terms and applicable laws in their jurisdiction. The software utilizes publicly available Generic Volume License Keys (GVLKs) and official Windows activation mechanisms as documented by Microsoft.

The author does not condone or support software piracy. This tool is intended for use in scenarios such as:
- Evaluation and testing environments
- Volume licensing deployments
- Educational purposes
- Troubleshooting activation issues with legitimate licenses

Users should ensure they have proper licenses for their Windows installations according to Microsoft's licensing agreements.

## Copyright and License

**Copyright © 2018-2026 Saad Ahmed. All Rights Reserved.**

This software and its source code are provided for educational and reference purposes only. No license is granted for commercial use, distribution, modification, or redistribution of this software or its derivatives.

### Terms of Use

- This code may be viewed and studied for educational purposes
- No permission is granted to distribute, sublicense, or sell copies of this software
- Any use of this software must comply with applicable laws and Microsoft's licensing agreements
- The author retains all rights to this software and its source code

### Permitted Activities

- Viewing the source code for learning and educational purposes
- Studying the implementation techniques and architecture
- Using the knowledge gained for legitimate software development projects

### Prohibited Activities

- Commercial use or distribution of this software
- Modification and redistribution of this software
- Using this software to circumvent legitimate software licensing
- Any use that violates Microsoft's terms of service or applicable laws

---

**Note:** This README provides comprehensive documentation for technical understanding and proper usage of the Windows Activator application. For questions, issues, or contributions, please refer to the project repository or contact the author through the provided links.
