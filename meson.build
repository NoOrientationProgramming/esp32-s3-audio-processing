
project(
	'esp32-audio-processing',
	'cpp',
)

cxx = meson.get_compiler('cpp')

deps = []
deps += dependency('threads')

args = []
args += '-DCONFIG_PROC_HAVE_LOG=1'
args += '-DCONFIG_PROC_INFO_BUFFER_SIZE=1024'
args += '-std=gnu++11'

if host_machine.system() == 'windows'
	args += '-D_WIN32_WINNT=_WIN32_WINNT_WIN10'
	args += '-DWINVER=_WIN32_WINNT_WIN10'
	deps += cxx.find_library('ws2_32')
endif

myApp = executable(
	'app',
	[
		'components/ProcessingCore/Processing.cpp',
		'components/ProcessingCore/Log.cpp',
		'components/ProcessingCore/SystemDebugging.cpp',
		'components/ProcessingCore/SystemCommanding.cpp',
		'components/ProcessingCore/TcpListening.cpp',
		'components/ProcessingCore/TcpTransfering.cpp',
		'components/ProcessingCommon/LibTime.cpp',
		'components/ProcessingCommon/ThreadPooling.cpp',
		'main/main.cpp',
		'main/AppSupervising.cpp',
		'main/AudioProcessing.cpp',
	],
	include_directories : include_directories([
		'components/ProcessingCore',
		'components/ProcessingCommon',
	]),
	dependencies : [
		deps,
	],
	cpp_args : [
		args,
	],
)

