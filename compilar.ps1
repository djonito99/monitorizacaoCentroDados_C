$ErrorActionPreference = "Stop"

$compilador = "gcc"
$executavel = "monitorizacao.exe"

$ficheiros = @(
    "src\main.c",
    "src\menu.c",
    "src\menu_alertas.c",
    "src\menu_historico.c",
    "src\estado.c",
    "src\sensores.c",
    "src\alertas.c",
    "src\historico.c",
    "src\persistencia.c",
    "src\ordenacao.c"
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