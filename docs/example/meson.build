project(
  'valve-examples',
  'c',
  meson_version: '>=1.1',
  default_options: ['c_std=c23'],
)

valve_dep = dependency(
  'valve',
  fallback: ['valve', 'valve_dep'],
)

executable(
  'demo-inline',
  'inline.c',
  dependencies: valve_dep,
)

executable(
  'demo-separate',
  'separate.c',
  dependencies: valve_dep,
)
