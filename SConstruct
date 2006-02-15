import os
import sys
import buildutils

#****************************************************************************
# The options are saved to a file and reapplied automatically
#****************************************************************************
opts = Options(['build.opts'], ARGUMENTS)
opts.Add('INSTALLDIR'                           , 'Sets the project installation directory', '/usr/local')
opts.Add(BoolOption('ENABLE_VIDEOWRAPPERSRC'    , 'Enables the videowrapper source', 1))
opts.Add(BoolOption('ENABLE_GLUTSINK'           , 'Enables the GLUT sink', 1))
opts.Add(BoolOption('ENABLE_GL_TEXTURE_2D_SINK' , 'Enables the GL texture sink', 1))
opts.Add(BoolOption('ENABLE_TESTSRC'            , 'Enables the test source', 1))
opts.Add(BoolOption('ENABLE_VIDEOSINK'          , 'Enables the video sink', 1))
opts.Add(BoolOption('ENABLE_OPENCV'             , 'Enables the video sink', 1))

#****************************************************************************
# Set up environment and save options to disk
#****************************************************************************
env = Environment(options = opts, ENV = os.environ)
opts.Save('build.opts', env)

#****************************************************************************
# Make sure ACE is available
#****************************************************************************
conf = Configure(env)
if conf.TryAction('pkg-config --exists ACE')[0]:
    env.ParseConfig('pkg-config --cflags --libs ACE')
else:
    print "****************************************************************************"
    print "ERROR: You need to have ACE installed and ACE.pc in your PKG_CONFIG_PATH"
    print "Make sure you installed ACE using the ./configure && make && make install"
    print "process..."
    print "****************************************************************************"
    sys.exit(-1)

if conf.TryAction('pkg-config --exists tinyxml')[0]:
    env.ParseConfig('pkg-config --cflags --libs tinyxml')
else:
    print "****************************************************************************"
    print "ERROR: You need to have TinyXml installed and tinyxml.pc in your PKG_CONFIG_PATH"
    print "Make sure you installed TinyXml from CVS using scons install"
    print "****************************************************************************"
    sys.exit(-1)

conf.Finish()

#****************************************************************************
# Add defines if using a set of nodes
#****************************************************************************
if env['ENABLE_VIDEOWRAPPERSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_VIDEOWRAPPERSRC'])
if env['ENABLE_GLUTSINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_GLUTSINK'])
if env['ENABLE_GL_TEXTURE_2D_SINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_GL_TEXTURE_2D_SINK'])
if env['ENABLE_TESTSRC']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_TESTSRC'])
if env['ENABLE_VIDEOSINK']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_VIDEOSINK'])
if env['ENABLE_OPENCV']:
    env.AppendUnique(CPPDEFINES = ['ENABLE_OPENCV'])

# Need to specify where to look for local include files
env.AppendUnique(CPPPATH = [Dir('#').abspath + os.sep + 'include'])
env.AppendUnique(CPPDEFINES = ['PLATFORM_' + sys.platform.upper()])

#****************************************************************************
# Set project details used in the package-config (.pc) file
# See tinyxml.pc.in
#****************************************************************************
env['OPENVIDEO_PROJECT_NAME']        = "OpenVideo"
env['OPENVIDEO_PROJECT_DESCRIPTION'] = "OpenVideo is a library which abstract video streams in a similar way as OpenTracker abstracts tracking data"
env['OPENVIDEO_PROJECT_VERSION']     = "1.0.0"
env['OPENVIDEO_PROJECT_LIBNAME']     = "openvideo"
buildutils.appendbuilders(env)
outname = env.Substitute('OpenVideo.pc', 'OpenVideo.pc.in')
env.AlwaysBuild(outname)
env.Alias(target = ["install"], source = env.AlwaysBuild(env.Install(dir = '${INSTALLDIR}/lib/pkgconfig', source = outname)))

#****************************************************************************
# Generate string of defines
#****************************************************************************
if env.has_key('CPPDEFINES'):
    defines = ''
    for define in env['CPPDEFINES']:
        defines += '-D' + define + " "
        
    env['OPENVIDEO_PROJECT_DEFINES'] = defines

#****************************************************************************
# Generate help message
#****************************************************************************
Help(opts.GenerateHelpText(env))

#****************************************************************************
# Build by executing SConscript file
#****************************************************************************

# Print welcoming message that makes you feel warm and fuzzy inside;)
print "\n...Building...\n"

# Call subsidiary build scripts.
env['BUILDDIR'] = 'build' + os.sep + sys.platform
SConscript(['src/SConscript'], exports='env', build_dir=env['BUILDDIR'], duplicate=1)

###########################################################################
# Done:)
###########################################################################