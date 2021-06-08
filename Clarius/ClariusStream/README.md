# MTEC-Clarius-OpenIGTLink

## How to install

Copy the =ClariusStream.apk= file to the Android tablet and install it.

## How to use ClariusStream

On the tablet containing the Clarius App:
1. Install the ClariusStream.apk application
2. Open the Clarius app and begin an imaging session with the Clarius probe.
3. Close the Clarius app (imaging should still be running), and run the ClariusStream app.
4. Click on "Start Service".
5. Obtain the local IP address of the tablet, which you can see by finding the clarius stream notification.

On the computer:
1. Install 3D Slicer
2. Install the OpenIGTLink extension
3. Navigate to "Modules" > "OpenIGTLinkIF"
4. Create a new node via the "+" button, and set it to "client"
5. Enter in the tablet IP, and the port is 18944.
6. Check the button "Active", and it should begin receiving images.

## How to build

1. Install and open Android Studio, and open the ClariusStream project.
2. Edit/create a ClariusStream/local.properties file:
```
## This file must *NOT* be checked into Version Control Systems,
# as it contains information specific to your local configuration.
#
# Location of the SDK. This is only used by Gradle.
# For customization when using a Version Control System, please read the
# header note.
#Tue Jun 01 11:56:38 EDT 2021
sdk.dir=C\:\\Users\\YOUR_USERNAME\\AppData\\Local\\Android\\Sdk
cmake.dir=C\:\\Users\\YOUR_USERNAME\\AppData\\Local\\Android\\Sdk\\cmake\\3.18.1
swig.dir=C\:\\Program Files\\swigwin-4.0.2\\Lib
swig.exe=C\:\\Program Files\\swigwin-4.0.2\\swig.exe
ndk.ar.exe=C\:\\Users\\YOUR_USERNAME\\AppData\\Local\\Android\\Sdk\\ndk\\22.1.7171670\\toolchains\\arm-linux-androideabi-4.9\\prebuilt\\windows-x86_64\\bin\\arm-linux-androideabi-ar.exe
```
2. Press the gradle sync button
3. Press `Ctrl` twice to open the "Run Anywhere" dialog, and type "gradle app:gatherClarius"
3. Press `Ctrl` twice to open the "Run Anywhere" dialog, and type "gradle generateEmptyPthread"
3. Press `Ctrl` twice to open the "Run Anywhere" dialog, and type "gradle app:externalNativeBuildRelease"
  - If you want the debug version, run "gradle app:externalNativeBuildDebug"
4. Click on "Build" in the top bar, and run "Make".

### How to run on Android

1. Enable adb debugging on the device
2. Connect the device to the computer, and allow computer access.
3. On the toolbar, middle-right, select your device from the second drop-down menu. Then, click the Run arrow button nearby to run the app.
