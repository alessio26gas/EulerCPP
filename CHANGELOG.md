# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.5.2] - 2025-08-28

### Changed

- Angles in input are now interpreted in degrees (previously radians).

### Fixed

- Removed trailing spaces from Doxygen comments.

## [0.5.1] - 2025-08-26

### Changed

- Improved solve routine code.

### Fixed

- Simulation time can no longer exceed specified maximum time.

## [0.5.0] - 2025-08-25

### Added

- Binary restart file format.
- Restart file format input setting (`restart_format`).
- Point probes.
- Point probes input settings.
- Point probes CSV output file (`*_probes.csv`).
- Reports.
- Reports CSV output file (`*_reports.csv`).
- Reports input settings.
- Face boundary flag safety check.

### Changed

- Default restart file format is set to binary.
- Improved axisymmetric mode initialization routine.
- Updated Doxygen documentation.

## [0.4.0] - 2025-08-19

### Added

- Mesh loading routine can assign boundary conditions using tags.
- Spherical regions for initial conditions definition.
- Spherical regions for boundary conditions definition.
- Elements dimension safety check.
- 2D Explosion example.

### Changed

- Mesh loading routine now reads element tags instead of skipping them.

## [0.3.1] - 2025-08-19

### Added

- Reverse flow check in pressure outlet boundary condition.

### Changed

- Improved boundary conditions code.

### Fixed

- Fixed [`build.ps1`](build.ps1) script behavior.
- Fixed typos.

## [0.3.0] - 2025-08-18

### Added

- Riemann solver input setting (`riemann`).
- Rusanov Riemann solver.
- HLL (Harten-Lax-van Leer) Riemann solver.
- Examples folder.

### Changed

- Initialization routine now also initializes Riemann solver.
- Input parsing now ignores lines starting with `#`.
- Improved global residuals computation with OpenMP.
- Improved [`CMakeLists.txt`](CMakeLists.txt).

## [0.2.0] - 2025-08-17

### Added

- CSV output format.
- Missing Doxygen documentation.

### Changed

- Export primitive variables instead of conservative variables.

## [0.1.0] - 2025-08-16

### Added

- Initial project setup.