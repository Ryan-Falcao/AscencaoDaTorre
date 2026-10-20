param(
    [string]$AtlasProjetis = "C:\Users\Pichau\.codex\generated_images\01a14155-c041-7ae0-8209-860a4c5f9556\exec-34be5d97-0b8a-4f4a-9e56-89ac740354df.png",
    [string]$AtlasOriginais = "C:\Users\Pichau\.codex\generated_images\01a14155-c041-7ae0-8209-860a4c5f9556\exec-4999f582-95d7-4236-ba80-3673bfb4bc86.png",
    [string]$AtlasNovos = "C:\Users\Pichau\.codex\generated_images\01a14155-c041-7ae0-8209-860a4c5f9556\exec-79566180-6662-49ee-a8ac-edc1ac6a2e1c.png"
)

Add-Type -AssemblyName System.Drawing

$nomesProjetis = @(
    "bullet_rain", "bullet_side_wave", "bullet_spiral", "bullet_fast_rain",
    "bullet_wide_wave", "bullet_tight_spiral", "bullet_petal_orbit", "bullet_bounce_orbs",
    "bullet_moving_wall", "bullet_split_shots", "bullet_cross_lasers", "bullet_pause_darts",
    "bullet_ring_burst", "bullet_diagonal_rain", "bullet_chase_wave", "bullet_pinwheel",
    "bullet_gravity_flip", "bullet_shadow_bloom"
)

$nomesOriginais = @(
    "defense_mariposa_asas", "defense_sapo_saltos", "defense_passaro_mergulho",
    "defense_limo_crescimento", "defense_fagulha_luz", "defense_lebre_pulos",
    "defense_gato_emboscada", "defense_cavaleiro_duelo", "defense_sino_eco",
    "defense_torre_esmagamento", "defense_mariposa_trama", "defense_mariposa_corrente",
    "defense_sapo_barreiras", "defense_sapo_vitorias_regias", "defense_passaro_fogo_cruzado",
    "defense_passaro_tunel", "defense_chuva_fogo", "defense_limo_mare",
    "defense_limo_balsas", "defense_fagulha_canhoes", "defense_fagulha_orbita",
    "defense_lebre_barreiras", "defense_lebre_parkour", "defense_gato_impacto",
    "defense_gato_perseguicao", "defense_cavaleiro_lancas", "defense_cavaleiro_desafio",
    "defense_sino_canhoes", "defense_sino_impacto", "defense_sino_desafio",
    "defense_sino_gas_sombra", "defense_torre_parkour", "defense_torre_canhoes",
    "defense_torre_desafio"
)

$nomesNovos = @(
    "defense_sarcas_portoes", "defense_herbalista_frascos", "defense_legionario_escudos",
    "defense_bruxa_caldeirao", "defense_revenante_prisao", "defense_esqueleto_estoupido",
    "defense_acendedor_martelo", "defense_penitente_incensario", "defense_ladrao_reflexo",
    "defense_acolito_vitral", "defense_goblin_estocadas", "defense_automato_engrenagens",
    "defense_mercenario_ricochete", "defense_carcereiro_grade", "defense_cultista_labirinto",
    "defense_afogado_mare_coral", "defense_mago_espiral_cinzas", "defense_goblin_exercito",
    "defense_inquisidor_veredito"
)

function Exportar-Celulas([string]$caminho, [int]$colunas, [int]$linhas, [string[]]$nomes) {
    $atlas = [System.Drawing.Bitmap]::FromFile($caminho)
    try {
        for ($indice = 0; $indice -lt $nomes.Count; $indice++) {
            $coluna = $indice % $colunas
            $linha = [Math]::Floor($indice / $colunas)
            $x0 = [Math]::Floor($coluna * $atlas.Width / $colunas)
            $x1 = [Math]::Floor(($coluna + 1) * $atlas.Width / $colunas)
            $y0 = [Math]::Floor($linha * $atlas.Height / $linhas)
            $y1 = [Math]::Floor(($linha + 1) * $atlas.Height / $linhas)

            $minX = $x1; $minY = $y1; $maxX = $x0; $maxY = $y0
            for ($y = $y0; $y -lt $y1; $y++) {
                for ($x = $x0; $x -lt $x1; $x++) {
                    if ($atlas.GetPixel($x, $y).A -gt 12) {
                        if ($x -lt $minX) { $minX = $x }
                        if ($x -gt $maxX) { $maxX = $x }
                        if ($y -lt $minY) { $minY = $y }
                        if ($y -gt $maxY) { $maxY = $y }
                    }
                }
            }
            if ($maxX -lt $minX) { throw "Celula vazia inesperada: $indice em $caminho" }

            $largura = $maxX - $minX + 1
            $altura = $maxY - $minY + 1
            $escala = [Math]::Min(28.0 / $largura, 28.0 / $altura)
            $destinoLargura = [Math]::Max(1, [Math]::Round($largura * $escala))
            $destinoAltura = [Math]::Max(1, [Math]::Round($altura * $escala))
            $saida = New-Object System.Drawing.Bitmap 32, 32, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
            $grafico = [System.Drawing.Graphics]::FromImage($saida)
            try {
                $grafico.Clear([System.Drawing.Color]::Transparent)
                $grafico.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
                $grafico.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
                $origem = New-Object System.Drawing.Rectangle $minX, $minY, $largura, $altura
                $destino = New-Object System.Drawing.Rectangle ([Math]::Floor((32-$destinoLargura)/2)), ([Math]::Floor((32-$destinoAltura)/2)), $destinoLargura, $destinoAltura
                $grafico.DrawImage($atlas, $destino, $origem, [System.Drawing.GraphicsUnit]::Pixel)
                $arquivo = Join-Path "assets/sprites" ($nomes[$indice] + ".png")
                $saida.Save((Join-Path (Get-Location) $arquivo), [System.Drawing.Imaging.ImageFormat]::Png)
            } finally {
                $grafico.Dispose()
                $saida.Dispose()
            }
        }
    } finally {
        $atlas.Dispose()
    }
}

Exportar-Celulas $AtlasProjetis 6 3 $nomesProjetis
Exportar-Celulas $AtlasOriginais 7 5 $nomesOriginais
Exportar-Celulas $AtlasNovos 5 4 $nomesNovos

$todos = @($nomesProjetis) + @($nomesOriginais) + @($nomesNovos)
$colunasPrevia = 10
$linhasPrevia = [Math]::Ceiling($todos.Count / $colunasPrevia)
$previa = New-Object System.Drawing.Bitmap ($colunasPrevia * 48), ($linhasPrevia * 48), ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$graficoPrevia = [System.Drawing.Graphics]::FromImage($previa)
try {
    $graficoPrevia.Clear([System.Drawing.Color]::FromArgb(255, 18, 20, 29))
    $graficoPrevia.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
    for ($indice = 0; $indice -lt $todos.Count; $indice++) {
        $coluna = $indice % $colunasPrevia
        $linha = [Math]::Floor($indice / $colunasPrevia)
        $icone = [System.Drawing.Bitmap]::FromFile((Join-Path (Get-Location) "assets/sprites/$($todos[$indice]).png"))
        try {
            $graficoPrevia.DrawImage($icone, $coluna * 48 + 8, $linha * 48 + 8, 32, 32)
        } finally {
            $icone.Dispose()
        }
    }
    $previa.Save((Join-Path (Get-Location) "assets/sprites/_preview_ataques_defesa.png"), [System.Drawing.Imaging.ImageFormat]::Png)
} finally {
    $graficoPrevia.Dispose()
    $previa.Dispose()
}

Write-Host "Sprites defensivos exportados: $($nomesProjetis.Count + $nomesOriginais.Count + $nomesNovos.Count)"
