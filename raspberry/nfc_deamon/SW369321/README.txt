            _____________________________________________________

                        NxpNfcRdLib LINUX SW PACKAGE :                       
                              (V05.22.00)
            _____________________________________________________


Table of Contents
_________________

1 Document Purpose
2 Description of the package
3 Restrictions
4 Configurations
5 Package Contents
6 Mandatory materials (not included)
7 Hardware Configuration
8 Software Configuration
9 Steps to follow
10 List of supported NFC Reader Boards/ICs


1 Document Purpose
==================

  This document describes the content of the NxpNfcRdLib Linux package and
  steps how to run examples on Raspberry Pi. It also describes known problems
  and restrictions.
  More complete information can be find in:
  - AN11802 NFC Reader Library for Linux Installation Guidelines

  
2 Description of the package
===================================================

  The package contains implementation of the NxpNfcReaderLibrary, Linux OS
  support and examples to demonstrate how to use library.


3 Restrictions
================================================

  Package contains implementation for Linux only and it does not provide 
  support for any other platform.
  

4 Configurations 
==================================================

  This package is preconfigured for Linux OS (running on Raspberry Pi) and
  CLRC663+ (CLEV6630B demo board in SPI configuration).
  Any other configuration requires software adaptation as described in:
   - AN11802 NFC Reader Library for Linux Installation Guidelines.


5 Package Contents
==================

  - docs
    + Doxygen documentation of the NxpNfcReaderLibrary as well as DAL and OSAL layers
  - Examples 
    + code examples of the NFC Reader Library use 
  - CompliancyApp 
    + code of applications for compliancy testing 
  - NxpNfcRdLib
    + NxpNfcReaderLibrary source code
  - Platform
    + DAL layer source code (relates to RaspberryPi)
  - RTOS  
    + OSAL layer source code (relates to Linux)
  - CMakeLists.txt
    + input file for makefile structure creation
  - README.txt
    + this readme file
  - NXP_SLDA.pdf
    + NXP Software License and Distribution Agreement

    
6 Mandatory materials (not included)
====================================

  - Raspberry Pi 2 or Raspberry Pi 3
  - Raspbian Jessie Lite from Raspberry Pi
  - NXP's NFC Reader demoboard (see chapter 10)

  
7 Hardware Configuration
========================

  Before starting this application, HW Changes may be required for the used
  board.  Refer to the following User Manuals / Application notes before
  starting with this example:
   - AN11211: Quick Start Up Guide RC663 Blueboard
   - AN11744: PN5180 Evaluation board quick start guide

  
8 Software Configuration
========================

  Default software configuration relates to RaspberryPi communicating to CLEV6630B via 
  SPI from User mode. Others configuration are described in:
   - AN11802 NFC Reader Library for Linux Installation Guidelines


9 Steps to follow
=================

  Details described in:
   - AN11802 NFC Reader Library for Linux Installation Guidelines


10 List of supported NFC Reader Boards/ICs
==========================================

  1) CLEV6630B 2.0 Customer Evaluation Board
  2) PNEV5180B v2.0 Customer Evaluation Board
  
  ---------------------------------------------------------------------------

  For updates of this package, see www.nxp.com/pages/:NFC-READER-LIBRARY
  For more complete description see www.nxp.com/documents/application_note/AN11802.pdf
