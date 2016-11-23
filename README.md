# JPEG_Compression
JPEG encoding and decoding in C++


##Please visit [HERE](https://devendrapratapyadav.github.io/JPEG_Compression) for detailed analysis of code and experiment results.

Main code is present in 'code' folder in Github directory. 'jpeg.cpp' is the main program.
'jpeg.cpp' is also placed in the main directory but compilation must be done from one in 'code' folder.

'Sample_images_and_results.zip' contains some images and their decoded outputs. These outputs are analyzed in the above given link.


###HOW TO RUN : 
NOTE: Cmake and GNU make are required for compilation

1) Compile the program by entering the following command :
```sh
   cd code
   cmake .
   make
 ```
 2) Run using:
 ```sh
   ./jpeg
 ```
 
 The program provides two options via a menu:
1. Encode .tiff image to .dat
2. Decode .dat files to .jpg

For option 1, The user can enter name of .tiff image file.
The program generated three files - 'img1.dat','img2.dat','img3.dat' corresponding to BGR channels of image.

Option 2 takes these three .dat files and decodes them to output 'decoded.jpg'.

