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

## How It Works

### Activation Process
The application manages the Windows activation process through three sequential steps:

1. **Product Key Installation**: Executes the Windows licensing command to install the product key specific to your Windows edition
2. **KMS Server Configuration**: Configures the connection to a KMS (Key Management Service) server for activation
3. **Windows Activation**: Performs the actual activation through Windows' built-in licensing tools

All processes run in the background without displaying console windows, providing a seamless user experience. Status updates are synchronized to ensure accurate real-time feedback.

### OS Detection Process
- Queries Windows registry to determine the current Windows build number (22000+ indicates Windows 11)
- Reads the edition identifier from the registry to determine the specific Windows edition
- Uses Windows Management Instrumentation (WMI) to check the current activation status
- A license status value of 1 indicates an already-activated system

### User Interface System
- Renders a full-screen window with custom styling and animations
- Implements a carousel-style edition selector with smooth animated transitions
- Manages dynamic UI states including button opacity and status text visibility
- Draws custom animations including the loading spinner during activation
- Renders success indicators using vector graphics primitives
- Includes clickable author attribution with web links

## Usage Instructions

### Running the Application

1. **Launch as Administrator:**
   - The application requires administrator privileges to modify Windows activation settings
   - Right-click the executable and select "Run as administrator"
   - Modern builds automatically request UAC elevation

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

**Copyright © 2026 Saad Ahmed. All Rights Reserved.**

This documentation is provided for informational and reference purposes only. The Windows Activator application and all associated materials are proprietary software.

### Terms of Use

- This documentation may be referenced for informational purposes
- No permission is granted to distribute, sublicense, or sell the Windows Activator application
- Any use of the application must comply with applicable laws and Microsoft's licensing agreements
- The author retains all rights to the application and its documentation

### Permitted Use

- Reading and referencing this documentation for understanding the application's functionality
- Learning about Windows activation processes and mechanisms
- Using the application for legitimate Windows activation purposes

### Prohibited Activities

- Commercial distribution of the application
- Reverse engineering or modification of the application
- Using the application to circumvent legitimate software licensing
- Any use that violates Microsoft's terms of service or applicable laws

---

**Note:** This documentation provides comprehensive information about the functionality, features, and proper usage of the Windows Activator application. For questions or support, please contact the author through the provided links.
