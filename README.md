# CG2271-Project
Code for CG2271 Project

## Developing with uKeilVision
1. All header files should sit in the `include` directory. Do split them up into more folders for better organization. For example, `include/serial/` would contain `.h` files related to serial functionality.
2. Likewise, all `.c` files will sit in `src` following their hierarchy in `include`. `src/serial` will contain all the `.c` files containing implementation for serial. `main.c` is an exception and will sit on its own in `src`.
3. uKeilVision/the project has been set up such that the `include` folder is well, included. So, no worries for the `.h` files.
4. For the `.c` files however, they do still need to be added into Source Group 1 individually. To do so, in uKeilVision, right-click Source Group 1, click Add Existing Files and add the necessary `.c` files.
5. You're all set to compile and run your code! Run and debug the code as how we've always done in lab! Happy developing!