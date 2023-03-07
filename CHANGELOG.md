# Changelog

Full changelog of _IX-Ray_ 1.0 project

## Release 0.3 (March 2023)

### Common

- Migration to __Visual Studio 2022__
- Fixed compilation errors
- Fixed a lot of issues with linking
- Enabled multicore building for all projects
- Enabled __x86-64__ toolchain for all projects
- Enabled __GitHub Actions__

### Core

- Replaced custom `xr_deque<T>` and `xr_vector<T>` with aliases of `std::deque<T>` and `std::vector<T>`
- Placed `clear_and_reserve()` method of `xr_vector<T>` class in a separate function
- Partially replaced STL extension aliases with `using` analogs
- Deleted `DEF_*` and `DEFINE_*` macroses from STL extensions

### Engine

- Fixed __VSync__

### Dependencies

- Deleted unused __Intel VTune__ functionality

### Resources

- Added resources
- Normalized encoding of scripts

## Release 0.2 (November 2022)

### Common

- Migration to __Visual Studio 2015__
- Fixed compilation errors
- Replaced deprecated functions to safe and modern analogs
- Replaced some custom functions and types to standard library analog
- Replaced `debug::make_final<T>` class to _C++11_ `final` specifier

### Core

- Removed __BugTrap__ and __minizip__
- Fixed `Debug` configuration workability
- Fixed window focus error

### Engine

- Unlocked console commands: `hud_fov`, `fov`, `jump_to_level`, `g_god`, `g_unlimitedammo`, `run_script`, `run_string`, `time_factor`

### Utilities

- Incompletely integrated __DirectXTex__

## Release 0.1 (November 2022)

### Common

- Migration to __Visual Studio 2013__
- Fixed compilation errors
- Configured engine projects building
