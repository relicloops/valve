project(
  'valve',
  'c',
  version: '1.0.0-000',
  license: 'Apache-2.0',
  meson_version: '>=1.4',
  default_options: [
    'c_std=c23',
    'warning_level=3',
    'buildtype=debug',
  ],
)

# binary version
valve_version = meson.project_version()

# include directory

subdir('include')
subdir('src')

# Test system: Meson prepares an OS/architecture/compiler-agnostic Ceedling
# toolchain, runs coverage (gcovr) via `meson test`, and leaves a wrapper so
# subsequent direct Ceedling use reuses the same selection.
ceedling = find_program('ceedling', required: get_option('tests'))
gcovr = find_program('gcovr', required: get_option('tests'))

if ceedling.found() and gcovr.found()
  c_compiler = meson.get_compiler('c')
  compiler_id = c_compiler.get_id()
  compiler_major = c_compiler.version().split('.')[0]
  compiler_cmd = ' '.join(c_compiler.cmd_array())

  if compiler_id == 'gcc'
    gcov = find_program('gcov-' + compiler_major, 'gcov', required: true)
    gcov_command = gcov.full_path()
  elif host_machine.system() == 'darwin' and compiler_id in ['clang', 'apple_clang']
    xcrun = find_program('xcrun', required: true)
    gcov_command = xcrun.full_path() + ' llvm-cov gcov'
  elif compiler_id in ['clang', 'apple_clang']
    llvm_cov = find_program('llvm-cov-' + compiler_major, 'llvm-cov', required: true)
    gcov_command = llvm_cov.full_path() + ' gcov'
  else
    error('Coverage requires GCC or Clang (got ' + compiler_id + ')')
  endif

  gcovr_command = gcovr.full_path()
  gcov_mixin = meson.current_source_dir() / 'test' / 'ceedling-gcov.yml'

  toolchain_conf = configuration_data()
  toolchain_conf.set('C_COMPILER', compiler_cmd)
  toolchain_conf.set('GCOV_EXECUTABLE', gcov_command)
  toolchain_conf.set('GCOVR_EXECUTABLE', gcovr_command)

  ceedling_toolchain = configure_file(
    input: 'test/ceedling-toolchain.yml.in',
    output: 'ceedling-toolchain.yml',
    configuration: toolchain_conf,
  )

  wrapper_conf = configuration_data()
  wrapper_conf.set('CEEDLING', ceedling.full_path())
  wrapper_conf.set('SOURCE_ROOT', meson.current_source_dir())
  wrapper_conf.set(
    'TOOLCHAIN_MIXIN',
    meson.current_build_dir() / 'ceedling-toolchain.yml',
  )
  wrapper_conf.set('GCOV_MIXIN', gcov_mixin)

  valve_ceedling = configure_file(
    input: 'test/valve-ceedling.in',
    output: 'valve-ceedling',
    configuration: wrapper_conf,
  )

  # configure_file does not set the executable bit; fix it for direct use.
  run_command('chmod', '755', valve_ceedling, check: true)

  message(
    'Ceedling toolchain: system=@0@ cpu=@1@ compiler=@2@ (@3@) gcov=@4@ gcovr=@5@'.format(
      host_machine.system(),
      host_machine.cpu_family(),
      compiler_id,
      compiler_cmd,
      gcov_command,
      gcovr_command,
    ),
  )
  message('Direct Ceedling wrapper: ' + (meson.current_build_dir() / 'valve-ceedling'))

  # First automated gate: full coverage run with gcovr HTML report.
  # Day-to-day unit runs (no coverage) use the same wrapper:
  #   build/valve-ceedling test:all
  test(
    'coverage',
    valve_ceedling,
    args: [
      'clobber',
      'gcov:all',
    ],
    workdir: meson.current_source_dir(),
    timeout: 300,
    suite: ['ceedling', 'coverage'],
  )

  # Put the wrapper on PATH inside `meson devenv -C <builddir>`.
  devenv = environment()
  devenv.prepend('PATH', meson.current_build_dir())
  meson.add_devenv(devenv)
endif
