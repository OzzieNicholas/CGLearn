set curpath="%~dp0"
set target="%curpath%..\..\.git\hooks\pre-commit"

IF EXIST "%target%" (
    echo "Hooks was installed"
) ELSE (
  copy "%curpath%pre-commit" "%target%"
)