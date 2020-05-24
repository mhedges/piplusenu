#!/usr/bin/env python
#
import os, re, string, subprocess
Import('env')
from stntuple_helper import *
#------------------------------------------------------------------------------
# last two components of the path. Ex: /not/this/but/THIS/AND_THIS
#                                      "AND_THIS" is usually "src"
#------------------------------------------------------------------------------
x = subprocess.call('scripts/build_config',shell=True)
#------------------------------------------------------------------------------
# done
#------------------------------------------------------------------------------
env.Append(BUILDERS = {'StntupleCodegen'  : stntuple_codegen})
env.Append(BUILDERS = {'StntupleRootCint' : stntuple_rootcint})

Export('stntuple_helper')
