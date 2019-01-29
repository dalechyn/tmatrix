# TMatrix
Matrix representation in linux term just as in "The Matrix" film. At this moment available only for Linux.

# How to use?
Download repo's zip and unpack it, or using `git` you can do next:

If you have no folder for local repos:

`mkdir ~/repos/`

`cd ~/repos/`

`git clone git@github.com:vladik2738/tmatrix.git`

`make`

`./tm`

If you close matrix with Ctrl+C, don't forget to clean the terminal! 

`./rs` or `tput reset`

# Install dependencies:
`make install`

# Run TMatrix
`make`

`./tm`

# Help menu

`./tm --help`

# Debug TMatrix

`make debug`

`gdb dtm`

# Clean-up
`make clean`
