
project(
	'esp-visualizing',
	'cpp',
)

cxx = meson.get_compiler('cpp')

deps = []
deps += dependency('threads')
deps += dependency('Qt6Core')
deps += dependency('Qt6Gui')
deps += dependency('Qt6Widgets')
deps += dependency('Python3')

args = []
args += '-DCONFIG_PROC_HAVE_LOG=1'
args += '-DCONFIG_PROC_INFO_BUFFER_SIZE=1024'
args += '-std=gnu++17'
args += '-isystem'
args += '/usr/include/python3.10'
args += '-isystem'
args += '/usr/include/x86_64-linux-gnu/qt6/QtWidgets/'

if host_machine.system() == 'windows'
	args += '-D_WIN32_WINNT=_WIN32_WINNT_WIN10'
	args += '-DWINVER=_WIN32_WINNT_WIN10'
	deps += cxx.find_library('ws2_32')
endif

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
warnings = [
	'-Wall',
	'-Wextra',
	'-Wpedantic',
	'-Werror',
	'-Wfatal-errors',
	'-Wreorder',
	'-Wswitch-enum',
	'-Wuseless-cast',
	'-Wparentheses',
	'-Wshift-overflow',
	'-Wsign-compare',
	'-Wzero-as-null-pointer-constant',
	'-Wcast-align',
	'-Wcast-qual',
	'-Wcatch-value',
	'-Wchar-subscripts',
	'-Wswitch-default',
	'-Wctor-dtor-privacy',
	'-Wduplicated-branches',
	'-Wduplicated-cond',
	'-Wempty-body',
	'-Wextra-semi',
	'-Wfloat-equal',
	'-Wformat',
	'-Wformat-extra-args',
	'-Wimplicit-fallthrough',
	'-Wmissing-field-initializers',
	'-Wnull-dereference',
	'-Wshadow',
]

args += warnings

myApp = executable(
	'app',
	[
		'../../components/ProcessingCore/Processing.cpp',
		'../../components/ProcessingCore/Log.cpp',
		'../../components/ProcessingCore/SystemDebugging.cpp',
		'../../components/ProcessingCore/SystemCommanding.cpp',
		'../../components/ProcessingCore/TcpListening.cpp',
		'../../components/ProcessingCore/TcpTransfering.cpp',
		'../../components/ProcessingCommon/LibTime.cpp',
		'../../components/ProcessingCommon/ThreadPooling.cpp',
		'../../components/ProcessingCommon/PhyAnimating.cpp',
		'main/main.cpp',
		'main/AppSupervising.cpp',
		'main/EspSimulating.cpp',
		'main/SimUserInteracting.cpp',
	],
	include_directories : include_directories([
		'../../components/ProcessingCore',
		'../../components/ProcessingCommon',
	]),
	dependencies : [
		deps,
	],
	cpp_args : [
		args,
	],
)

