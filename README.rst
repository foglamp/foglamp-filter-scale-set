=====================================
Fledge "scale-set" C++ Filter plugin
=====================================

Selectively apply a set of scale and offset values to data readings. 

A JSON document configures the set of scale factors and offset values
to add to readings. Each scale and offset value is associated with a
regular expression whcih is matches against the asset name of the reading
in the asset. Optionally a datapoint regular expression may also be given,
this is used to match the datapoint within a matching asset.

Caution, if multiple regular expressions match a single asset and datapoint
then *all* of the scale and offset values will be applied to the asset/datapoint.

Configuration
-------------

The filter supports the following configuration items:

enable
  A switch to control if the filter should take effect or not

factors
  A JSON document which contains an array of scale factors to apply. Each
  element in the arary is a JSON object with the following properties:

  asset
    A regular expression that should match the asset name

  datapoint
    A regular expression that should match the data point within the asset

  scale
    The scale factor to apply to the matching asset and data point

  offset
    An offset value to apply to the matching asset and data point


.. code-block:: JSON

  { "factors" : [
       {
         "asset"     : "environment",
         "datapoint" : "temperature",
         "scale"     : 1.8,
         "offset"    : 32
       },
       {
         "asset"     : "environment",
         "datapoint" : "humidity",
         "scale"     : 0.1,
         "offset"    : 0
       }
    ]
  }

Build
-----
To build Fledge "scale-set" C++ filter plugin:

.. code-block:: console

  $ mkdir build
  $ cd build
  $ cmake ..
  $ make

- By default the Fledge develop package header files and libraries
  are expected to be located in /usr/include/fledge and /usr/lib/fledge
- If **FLEDGE_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FLEDGE_ROOT directory.
  Please note that you must first run 'make' in the FLEDGE_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FLEDGE_SRC** sets the path of a Fledge source tree
- **FLEDGE_INCLUDE** sets the path to Fledge header files
- **FLEDGE_LIB sets** the path to Fledge libraries
- **FLEDGE_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FLEDGE_INCLUDE** option should point to a location where all the Fledge 
   header files have been installed in a single directory.
 - The **FLEDGE_LIB** option should point to a location where all the Fledge
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FLEDGE_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FLEDGE_ROOT set

  $ export FLEDGE_ROOT=/some_fledge_setup

  $ cmake ..

- set FLEDGE_SRC

  $ cmake -DFLEDGE_SRC=/home/source/develop/Fledge  ..

- set FLEDGE_INCLUDE

  $ cmake -DFLEDGE_INCLUDE=/dev-package/include ..
- set FLEDGE_LIB

  $ cmake -DFLEDGE_LIB=/home/dev/package/lib ..
- set FLEDGE_INSTALL

  $ cmake -DFLEDGE_INSTALL=/home/source/develop/Fledge ..

  $ cmake -DFLEDGE_INSTALL=/usr/local/fledge ..
