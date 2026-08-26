docker build -t orbit-os-build .
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

docker run --rm -v ${PWD}:/orbit-os orbit-os-build make all iso
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Build complete! orbitos.iso generated."
