# Traumatic Brain Injury Toolkit (tbitk)

<img src="Documentation/images/Clarius Phantom.gif" alt="Clarius Phantom" width="800"/>

## About
The Traumatic Brain Injury Toolkit (tbitk) is an open source (Apache 2.0) collection of software libraries for automated processing and streaming of point-of-care ultrasound (POCUS) devices in order to support diagnosis of traumatic brain injury.  It currently has levels of support for Clarius, Butterfly, Sonivate, Sonoque, and Interson probes.

A version of our automatic algorithm for optic nerve sheath diameter (ONSD) measurement (Moore et al. 2020) has been published [here](https://github.com/KitwareMedicalPublications/2020-MICCAI-ASMUS-Automatic-ONSD). 

The ONSD estimation work is still active and updates will be pushed to this repository in the near future.

The Traumatic Brain Injury Toolkit is developed by [Kitware, Inc.](https://www.kitware.com) in collaboration with Duke University.  This effort was sponsored by the U.S. Government under Other Transactions Number W81XWH-15-9-0001/W81XWH-19-9-0015 (MTEC 19-08-MuLTI-0079).

## Usage
See the individual device folders for additional documentation.  Streaming APIs are dependent on the manufacturer's API (e.g. Android/Java for Clarius, MFC C++ for Sonivate, C++ for Interson).  The toolkit wraps the native device APIs to the [OpenIGTLink](http://openigtlink.org) message protocol.  The folder [tbitk](tbitk) contains the _tbitk_ Python library for preprocessing video files.  Video files from the various manufacturers are converted into Insight Toolkit ([ITK](https://github.com/InsightSoftwareConsortium/ITK)) ```Image``` objects and file formats for image processing.

## Publications
* [MTEC 2021 Prototype of the Year](https://www.mtec-sc.org/mtec-presents-annual-awards-at-the-6th-annual-membership-meeting/)
* Moore, Brad T., Sean Montgomery, M. Niethammer, Hastings Greer and S. Aylward. “Automatic Optic Nerve Sheath Measurement in Point-of-Care Ultrasound.” ASMUS/PIPPI@MICCAI (2020). <http://dx.doi.org/10.1007/978-3-030-60334-2_3>
* [MTEC 2020 April Research Highlight](https://www.mtec-sc.org/april-2020-research-highlight/)

## Contact
For bugs/issues, submit an issue using the Github tracker.  For questions about the project or future collaboration, contact [Stephen Aylward](mailto:stephen.aylward@kitware.com).
