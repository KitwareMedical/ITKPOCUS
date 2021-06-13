# SonivateStream

## Requirements
1. OpenIGTLink
2. Sonivate API (a folder with Demo and Demo/MCE in it)
3. Visual Studio 2019
4. Windows SDK 10.0.19041.0
5. CMake 3.18 or greater
6. Installed the Cypress FX3 driver included with the Sonivate software
7. 3D Slicer (4.11.2021.02.26 or greater) and OpenIGTLinkIF module installed

## Building
Important Note: The SonivateStream.sln file is only for bootstrapping the MFC project.  This does not actually build the application.  CMake must be used with an out-of-source build directory.
1. Build OpenIGTLink
2. Configure CMake (assuming you have Git Bash)
	1. cd WORKSPACE_DIR
	2. git clone ...
	3. mkdir SonivateStream-build
    4. cd SonivateStream-build
	5. cmake-gui ../SonivateStream
	6. Sonivate_INCLUDE_DIR needs to point to Demo/MCE
	7. OpenIGTL_DIR needs to point to OpenIGTLink build director (e.g. OpenIGTLink-build)
	8. Press Configure, then Generate, then Open Project
3. Confirm the following Project Settings in Visual Studio
	1. While debugging/running code from VS, make sure your working directory is set to your BUILD directory, not the SOLUTION directory (default)
	2. Make sure you are dynamic linking MFC (e.g. ```/D _AFXDLL``` and ```/MD```)
	3. Make sure you have reserved your stack to 10000000 (Linker -> System -> Reserve Stack Size
	4. Make sure you are using Precompiled Headers (TODO: confirm this requirement)
	5. Confirm that you have many files, including UltrasoundInterfaceLib.dll in your build directories (should be copied by the CMake Configure command)
4. Build in either Debug or Release mode

## Common Errors
1.  Application crashes while initializing the probe
	1.  Access Violation : Sonivate files are missing from the build directory
		1.  Compare your build directory with the Demo build directory
		2.  Check your VS working directory is your build directory and not the solution directory
	2.  Stack overflow
		1.  Make sure you have reserved your stack to 10000000
2.  Initializing failed without crashing the application
	1.  Make sure the Sonivate probe is turned on and plugged in to your USB port
	2.  Make sure the Cypress FX3 driver was installed
	
## Running the SonivateStream
1.  Setup OpenIGTLink Server on 3D Slicer
	1.  Open 3D Slicer
	2.  Modules -> IGT -> OpenIGTLinkIF
	3.  Press '+' under Connectors
	4.  Set Type to 'Client'
	5.  Confirm hostname/port are localhost:18944
	6.  Click the checkbox next to 'Active' to start listening
2.  Double-click SonivateStream.exe
3.  After connection is successful, click Live to begin sending data to Slicer
	1.  If you aren't seeing anything in Slicer, click the drop-down on top of the view to and you should see Sonivate as an option
	2.  By default, Slicer auto-detects the min/max intensity values.  To get normal-looking B-mode, select the Volumes module and set the min/max to 0/255.
4.  Click Freeze to stop acquiring images.  Closing the SonivateStream window will be disconnect the probe.