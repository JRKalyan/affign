## affign
Affign is an automatic image aligner for photo-a-day projects. This is an application built to help users who need to align
potentially large sequences of images based on the face of the person in the image. It uses facial detection and landmark detection
(from OpenCV) to detect the facial features and performs affine transformations (scaling/rotating/translating) on the images to align
them.

### License / Copyright
© John Kalyan  
This project is licensed under the [MIT License](LICENSE).

This project relies on the following open source dependencies, which have their own respective licenses:  
  * **OpenCV** under the [3-Clause BSD License](https://opencv.org/license.html)  

### Using Affign
I plan to make Windows and MacOS pre-built executables available under "releases" but anyone should be able to build from source now that I have CMake support for the project. Users wishing to build from source must have OpenCV installed. I do suggest using CMake to build Affign; though it is not absolutely necessary, it makes setting up the project trivial.

Once Affign is built, you will need a cascade file for face detection and a trained face detector for landmark detection. You can supply your own but OpenCV has many cascade files available [here](https://github.com/opencv/opencv/tree/master/data/haarcascades). Any frontal face cascade files should work. A trained model can be found at https://github.com/kurnianggoro/GSOC2017/blob/master/data/lbfmodel.yaml. You can specify the path to these files or keep them in the executable directory and use the default names that Affign will look for: detector.xml and predictor.yaml.

### Inspiration
Credit to Matthew Earl's work [here](https://matthewearl.github.io/2016/01/22/portrait-timelapse/) for inspiration to make an automatic image aligner available to users.
