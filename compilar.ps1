$ErrorActionPreference = "Stop"

$compilador = "gcc"
$executavel = "monitorizacao.exe"

$ficheiros = @(
    "src\main.c",
    "src\menu.c"
)

$argumentos = @(
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-std=c11",
    "-Iinclude"
)

Write-Host "A compilar o projeto..."

& $compilador @argumentos @ficheiros "-o" $executavel

if ($LASTEXITCODE -ne 0) {
    Write-Error "A compilacao falhou."
    exit 1
}

Write-Host "Compilacao concluida: $executavel"