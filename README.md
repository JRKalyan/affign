## affign
Affign is an automatic image aligner for photo-a-day projects. This is a desktop application built to help users who need to align
potentially large sequences of images based on the face of the person in the image. It uses facial detection and landmark detection
(from Dlib) to detect the facial features and performs affine transformations (scaling/rotating/translating) on the images to align
them. The image manipulation is done with OpenCV and the GUI is built with wxWidgets.

### License / Copyright
© John Kalyan  
This project is licensed under the [MIT License](LICENSE).

This project relies on the following open source dependencies, which have their own respective licenses:  
  * **OpenCV** under the [3-Clause BSD License](https://opencv.org/license.html)  
  * **Dlib** under the [Boost Software License](http://dlib.net/license.html)  
  * **wxWidgets** under the [wxWindows Library Licence](http://docs.wxwidgets.org/3.1/page_copyright_wxlicense.html)  

### Using Affign
I plan to make Windows and MacOS pre-built executables available under "releases" but anyone should be able to build from source now that I have CMake support for the project. Users wishing to build from source must have wxWidgets and OpenCV installed as these are large libraries and wxWidgets doesn't yet have the CMake support I want, but __Dlib is dealt with entirely by the CMakeLists.txt file.__ On that note, I do suggest using CMake to build Affign; though it is not absolutely necessary, it makes setting up the project trivial. Be sure to enable SSE4 or AVX instructions during CMake configuration to [make Dlib fast](http://dlib.net/faq.html#Whyisdlibslow).
