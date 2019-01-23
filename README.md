# TMatrix
Matrix representation in linux term just as in "The Matrix" film. At this moment available only for Linux.

# How to download?
Download repo's zip and unpack it, or using `git` you can do next:

If you have no folder for local repos:

`mkdir ~/repos/`

`cd ~/repos/`

`git clone git@github.com:vladik2738/matrix.git`

`make`

`./tm`

If you close matrix with Ctrl+C, don't forget to clean the terminal! 

`tput reset`

# Install dependencies:
`make install`

# Run TMatrix
`make`

`./tm`

# Debug TMatrix

`make debug`

`gdb dtm`

# Clean-up
`make clean`
