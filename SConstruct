#!/usr/bin/env python

import sys
import os
sys.path.append('ICGBuilder')
import icgbuilder
import buildutils

#======================== CONFIGURATION SECTION =============================
# Following is the configuration section. In it most of the options for
# building the specified targets will be set, and then written to a file
# called config.user. This file can be later personalized to allow some
# other user defined configuration.
#===========================================================================#


# The targets file contains the definitions for all targets that will be built 
# using this scipt. Read the targets.py file to find out about those options
from targets import *


# Following are default flags for creating the supported target types. 
# Default flags will be applied to all targets of the same sort. In this
# section flags for different types of builds in different platforms can be
# set so that they apply to all targets of the same sort. Supported targets
# are dynamic libraries (dllflags), static libraries (libflags), and 
# executables (prgflags).


if sys.platform == 'win32':
	from win32Flags import defaultBuildFlags
	libraryList +=['tinyxml']	
	# list of libraries that might be used by this project
	scannerType = 'env'

	# directories needed for the build and install process

	buildDir     = os.path.join(os.getcwd(), 'build', 'win32')



	# attempt to add the define DLLNAME_EXPORTS to all targets of type DL
	# in most of the cases it might solve the problem of exporting symbols
	for t in targetList:
		if t['type'] == 'DL':
			if t.has_key('defines'):
				t['defines'].append(t['name'].upper() + '_EXPORTS')
			else:
				t['defines'] = [t['name'].upper() + '_EXPORTS']
	
elif sys.platform == 'linux' or sys.platform == 'linux2':
	from linuxFlags import dllflags, libflags, prgflags
	
	scannerType = 'mix'


	# directories needed for the build and install process
	buildDir     = os.path.join(os.getcwd(), 'build', 'linux')



elif sys.platform == 'darwin':
	from darwinFlags import dllflags, libflags, prgflags
	
	# list of libraries that might be used by this project
	scannerType = 'mix'

	buildDir     = os.path.join(os.getcwd(), 'build', 'darwin')


srcDir       = os.path.join(os.getcwd(), 'src')	

config_file='config.user'
	
print 'CONFIGURE: Searching for installed libraries'
# to get the include path for all libraries we need to retrieve 
# the environment vars pointing to the root of each lib
envvars = os.environ


targets = []
# create the builder with an empty target list
buildConfig = icgbuilder.ConfigBuilder(project, scannerType, envvars,
				       targets, libraryList)

# add extra configuration flags
buildConfig.version  = version
buildConfig.prefix   = prefix
buildConfig.buildDir = buildDir
buildConfig.srcDir   = srcDir

# setup the default build flags in the configurator
buildConfig.setDefaultBuildFlags(defaultBuildFlags)

# add main include directory to all configurations
buildConfig.addIncDir(includeDir)

# add enable flags
for eflag in enableFlags:
	buildConfig._enable[eflag] = enableFlags[eflag]

# create the build targets
buildConfig.setTargetList(targetList)

# this call makes all the targets try to guess their include and library flags
# and prepares the configuration for the build
buildConfig.createBuildEnvironments()
# write a config file to be read by scons and used to build each target
if not os.path.exists (config_file):
	buildConfig.writeConfigFile(config_file)

#-----------------------------------------------------------------------------
# Read the options from the config file and update the various necessary flags
# 
#
#-----------------------------------------------------------------------------


list_opts = []
user_options = Options (config_file, ARGUMENTS)
user_options_env = Environment (ENV = os.environ, options = user_options)

#win32 options
optlist = buildConfig.generateOptionList()
for opt in optlist:
	user_options.Add(opt)
	
uselist = buildConfig.generateUseList()
for use in uselist:
	user_options.Add(BoolOption(use[0], use[1], use[2]))

enablelist = buildConfig.generateEnableList()
for enable in enablelist:
	user_options.Add(BoolOption (enable[0], enable[1], enable[2]))

user_options.Add(EnumOption('BUILD_BINARY', buildConfig.buildType,'Select a release or debug binary.', allowed_values = ('release', 'debug', 'custom')))
	
#==========================================================================
# Intermediate configuration. The config file, config.opts is autogenerated
# using the options from config.user (is it necessasry to have 2 of them
#                                     ????? Eduardo)
#==========================================================================


user_options.Update (user_options_env)
user_options.Save('config.opts', user_options_env)
user_options_dict = user_options_env.Dictionary()
Help(user_options.GenerateHelpText(user_options_env))
env = Environment (ENV = os.environ)
ibuilder = icgbuilder.IcgBuilder(user_options_dict, env)

conf = Configure(env)
#****************************************************************************
# Add defines if using a set of nodes
#****************************************************************************

if user_options_dict['ENABLE_VIDEOWRAPPERSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_VIDEOWRAPPERSRC'])

if user_options_dict['ENABLE_SPECTECSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_SPECTECSRC'])
    
if user_options_dict['ENABLE_GLUTSINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_GLUTSINK'])
#    gllibs.append ('glut')
    env.AppendUnique(LIBS = gllibs)
    
if user_options_dict['ENABLE_GL_TEXTURE_2D_SINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_GL_TEXTURE_2D_SINK'])
    env.AppendUnique(LIBS = gllibs)
    
if user_options_dict['ENABLE_TESTSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_TESTSRC'])
    
if user_options_dict['ENABLE_V4LSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_V4LSRC'])

if user_options_dict['ENABLE_V4L2SRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_V4L2SRC'])
#    env.AppendUnique(CCFLAGS = ['-g'])

if user_options_dict['ENABLE_LIVE555SRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_LIVE555SRC'])

if user_options_dict['ENABLE_FFMPEGSINK']:
    env.ParseConfig('ffmpeg-config --cflags --libs avformat')
    env.AppendUnique(CPPDEFINES = ['ENABLE_FFMPEGSINK'])

if user_options_dict['ENABLE_FFMPEGSRC']:
    env.ParseConfig('ffmpeg-config --cflags --libs avformat')
    env.AppendUnique(CCFLAGS = ['-g'])
    env.AppendUnique(CPPDEFINES = ['ENABLE_FFMPEGSRC'])

if user_options_dict['ENABLE_VIDEOSINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_VIDEOSINK'])

if user_options_dict['ENABLE_DSVLSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_DSVLSRC'])

if user_options_dict['ENABLE_OPENCV']:
    if conf.TryAction('pkg-config --exists opencv')[0]:
        env.ParseConfig('pkg-config --cflags --libs opencv')
        env.AppendUnique(CPPDEFINES = ['ENABLE_OPENCV'])
    else:
        print "****************************************************************************"
        print "WARNING: You need to have OpenCV installed and opencv.pc in"
        print "your PKG_CONFIG_PATH in order for OpenCV stuff to work."
        print "****************************************************************************"

conf.Finish()


print "\n"
print "============================================================"
print "=      Configuration options for "+ project +" compile       ="
print "============================================================"

#==================    BUILD SECTION    ====================================
#
# In this section, the targets are compiled using the options from 
# config.opts, which are derived themselves from those in config.user
#
#===========================================================================


ibuilder.buildAll()
dlls = ibuilder.dlls
libs = ibuilder.libs
prgs = ibuilder.prgs

ibuilder.installAll()

###########################################################################
# Done:)
###########################################################################

