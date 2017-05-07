#!/bin/bash
make -f Makefile.win clean 
make -f Makefile.win && wine veta.exe
