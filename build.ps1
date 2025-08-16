<#
This PowerShell script configures and builds the EulerCPP project using CMake.
It supports an optional "-clean" switch to remove the existing build directory
before building.
#>

# Stop execution on any error
$ErrorActionPreference = "Stop"

param (
    [switch]$clean
)

# Clean the build directory if -clean is specified
if ($clean) {
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build"
    }
}

# Create the build directory if it does not exist
if (-Not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Enter the build directory
Push-Location build

# Configure project using CMake with Ninja generator
$cmakeCmd = 'cmake -G "Ninja" ..'
Invoke-Expression $cmakeCmd
if ($LASTEXITCODE -ne 0) {
    Pop-Location
    Write-Error "CMake configuration failed!"
    exit 1
}

# Build the project
Invoke-Expression "cmake --build ."
if ($LASTEXITCODE -ne 0) {
    Pop-Location
    Write-Error "Build failed!"
    exit 1
}

# Return to the root directory
Pop-Location