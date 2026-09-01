#!/usr/bin/env python3
"""Gera as figuras do relatório a partir dos CSVs dos experimentos."""

from __future__ import annotations

import csv
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import PercentFormatter, ScalarFormatter


RAIZ = Path(__file__).resolve().parent
DADOS = RAIZ / "dados"
FIGURAS = RAIZ.parent / "figuras"

CORES = {
    "azul": "#2864a5",
    "laranja": "#e07a26",
    "verde": "#37966f",
    "vermelho": "#c43b3b",
    "cinza": "#707070",
}


def ler_csv(nome: str) -> list[dict[str, str]]:
    with (DADOS / nome).open(encoding="utf-8", newline="") as arquivo:
        return list(csv.DictReader(arquivo))


def salvar(figura: plt.Figure, nome: str) -> None:
    FIGURAS.mkdir(parents=True, exist_ok=True)
    figura.savefig(FIGURAS / f"{nome}.pdf", bbox_inches="tight")
    figura.savefig(FIGURAS / f"{nome}.png", dpi=220, bbox_inches="tight")
    plt.close(figura)


def configurar_estilo() -> None:
    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 8,
            "axes.titlesize": 9,
            "axes.labelsize": 8,
            "legend.fontsize": 7,
            "xtick.labelsize": 7,
            "ytick.labelsize": 7,
            "axes.spines.top": False,
            "axes.spines.right": False,
            "axes.grid": True,
            "grid.alpha": 0.22,
            "grid.linewidth": 0.6,
            "figure.dpi": 140,
        }
    )


def grafico_frequencias_ioc() -> None:
    frequencias = ler_csv("frequencias.csv")
    ioc = ler_csv("ioc_periodos.csv")

    letras = [linha["letra"] for linha in frequencias]
    esperada = np.array([float(linha["esperada_pt"]) for linha in frequencias]) * 100
    claro = np.array([float(linha["claro"]) for linha in frequencias]) * 100
    cifrado = np.array([float(linha["cifrado"]) for linha in frequencias]) * 100

    figura, eixos = plt.subplots(1, 2, figsize=(7.15, 2.55), constrained_layout=True)
    ax = eixos[0]
    x = np.arange(26)
    ax.bar(x, cifrado, width=0.78, color="#c7d8ec", label="Cifrado (SEGURANCA)")
    ax.plot(x, esperada, color=CORES["laranja"], linewidth=1.4, label="Português esperado")
    ax.scatter(x, claro, color=CORES["azul"], s=11, zorder=3, label="Texto claro observado")
    ax.set_title("(a) Frequências antes e depois da cifra")
    ax.set_xlabel("Letra")
    ax.set_ylabel("Frequência (%)")
    ax.set_xticks(x)
    ax.set_xticklabels(letras)
    ax.set_ylim(0, max(esperada.max(), claro.max()) * 1.13)
    ax.legend(frameon=False, loc="upper right")

    ax = eixos[1]
    periodos = np.array([int(linha["periodo"]) for linha in ioc])
    valores = np.array([float(linha["ioc_medio"]) for linha in ioc])
    cores = [CORES["vermelho"] if p in (9, 18) else "#a9bfd8" for p in periodos]
    ax.bar(periodos, valores, color=cores, width=0.76)
    ax.axhline(1 / 26, color=CORES["cinza"], linestyle="--", linewidth=1.0)
    ax.set_title("(b) IC médio por período candidato")
    ax.set_xlabel("Tamanho candidato da chave")
    ax.set_ylabel("Índice de Coincidência médio")
    ax.set_xticks(periodos)
    ax.set_xticklabels(periodos, rotation=90)
    ax.set_ylim(0.035, 0.082)
    ax.text(1.2, 1 / 26 + 0.0008, "uniforme: 1/26", color=CORES["cinza"], fontsize=6.5)
    for periodo in (9, 18):
        valor = valores[np.where(periodos == periodo)][0]
        ax.annotate(
            f"{periodo}: {valor:.3f}",
            (periodo, valor),
            xytext=(0, 4),
            textcoords="offset points",
            ha="center",
            fontsize=7,
            color=CORES["vermelho"],
        )

    salvar(figura, "frequencias_ioc")


def grafico_interceptacao() -> None:
    dados = ler_csv("interceptacao.csv")
    razoes = sorted({int(linha["letras_por_posicao"]) for linha in dados})
    chaves = sorted({int(linha["tamanho_chave"]) for linha in dados})

    def procurar(modo: str, chave: int, razao: int, coluna: str) -> float:
        for linha in dados:
            if (
                linha["modo"] == modo
                and int(linha["tamanho_chave"]) == chave
                and int(linha["letras_por_posicao"]) == razao
            ):
                return float(linha[coluna]) * 100
        raise KeyError((modo, chave, razao, coluna))

    matriz = np.array(
        [
            [procurar("ordem_original", chave, razao, "sucesso_pipeline") for razao in razoes]
            for chave in chaves
        ]
    )

    figura, eixos = plt.subplots(1, 2, figsize=(7.15, 2.75), constrained_layout=True)
    ax = eixos[0]
    imagem = ax.imshow(matriz, cmap="YlGnBu", vmin=0, vmax=100, aspect="auto")
    ax.set_title("(a) Chave exata no 1º candidato")
    ax.set_xlabel("Letras disponíveis por posição da chave (N/L)")
    ax.set_ylabel("Tamanho da chave (L)")
    ax.set_xticks(np.arange(len(razoes)))
    ax.set_xticklabels(razoes, rotation=45, ha="right")
    ax.set_yticks(np.arange(len(chaves)))
    ax.set_yticklabels(chaves)
    for i in range(len(chaves)):
        for j in range(len(razoes)):
            cor = "white" if matriz[i, j] >= 58 else "#202020"
            ax.text(j, i, f"{matriz[i, j]:.0f}", ha="center", va="center", color=cor, fontsize=6.4)
    barra = figura.colorbar(imagem, ax=ax, fraction=0.046, pad=0.03)
    barra.set_label("Sucesso (%)")

    ax = eixos[1]
    estilos = {
        "ordem_original": ("Ordem original", CORES["azul"], "o"),
        "palavras_embaralhadas": ("Palavras embaralhadas", CORES["laranja"], "s"),
        "letras_embaralhadas": ("Letras embaralhadas", CORES["verde"], "^")
    }
    chave = 9
    for modo, (rotulo, cor, marcador) in estilos.items():
        valores = [procurar(modo, chave, razao, "sucesso_pipeline") for razao in razoes]
        ax.plot(razoes, valores, label=rotulo, color=cor, marker=marcador,
                markersize=3.5, linewidth=1.35)
    conhecido = [
        procurar("ordem_original", chave, razao, "sucesso_periodo_conhecido")
        for razao in razoes
    ]
    ax.plot(razoes, conhecido, label="Período 9 já conhecido", color=CORES["cinza"],
            linestyle="--", linewidth=1.15)
    ax.axhline(90, color="#999999", linestyle=":", linewidth=0.9)
    ax.set_title("(b) Efeito do embaralhamento (L = 9)")
    ax.set_xlabel("Letras por posição da chave (N/L)")
    ax.set_ylabel("Recuperação exata (%)")
    ax.set_xscale("log", base=2)
    ax.set_xticks(razoes)
    ax.set_xticklabels(razoes, rotation=45, ha="right")
    ax.set_ylim(-2, 103)
    ax.yaxis.set_major_formatter(PercentFormatter(100))
    ax.legend(frameon=False, loc="lower right")

    salvar(figura, "interceptacao")


def grafico_desempenho() -> None:
    tamanhos = ler_csv("desempenho_tamanho.csv")
    palavras = ler_csv("desempenho_palavras.csv")
    threads = ler_csv("desempenho_threads.csv")
    ataque = ler_csv("desempenho_ataque.csv")

    figura, eixos = plt.subplots(2, 2, figsize=(7.15, 5.05), constrained_layout=True)

    ax = eixos[0, 0]
    kib = np.array([int(linha["bytes"]) / 1024 for linha in tamanhos])
    ax.plot(kib, [float(linha["retorno_mib_s"]) for linha in tamanhos], marker="o",
            markersize=3.5, color=CORES["laranja"], label="API com retorno")
    ax.plot(kib, [float(linha["in_place_mib_s"]) for linha in tamanhos], marker="s",
            markersize=3.5, color=CORES["azul"], label="Núcleo in-place")
    ax.set_xscale("log", base=4)
    ax.set_xticks(kib)
    ax.set_xticklabels(["1K", "4K", "16K", "64K", "256K", "1M", "4M", "16M", "64M"],
                       rotation=35, ha="right")
    ax.set_title("(a) Vazão por tamanho da entrada")
    ax.set_xlabel("Entrada (KiB, escala log)")
    ax.set_ylabel("MiB/s")
    ax.legend(frameon=False)

    ax = eixos[0, 1]
    comprimentos = [int(linha["comprimento_palavra"]) for linha in palavras]
    rotulos = [str(valor) if valor != 1000 else "sem\nespaços" for valor in comprimentos]
    vazoes = [float(linha["vazao_mib_s"]) for linha in palavras]
    barras = ax.bar(np.arange(len(rotulos)), vazoes, color="#7da6cf")
    ax.set_title("(b) Palavras curtas e longas")
    ax.set_xlabel("Letras entre espaços")
    ax.set_ylabel("MiB/s (in-place)")
    ax.set_xticks(np.arange(len(rotulos)))
    ax.set_xticklabels(rotulos)
    ax.set_ylim(0, max(vazoes) * 1.17)
    for barra, valor in zip(barras, vazoes):
        ax.text(barra.get_x() + barra.get_width() / 2, valor + 15, f"{valor:.0f}",
                ha="center", va="bottom", fontsize=6.5)

    ax = eixos[1, 0]
    quantidades = [int(linha["threads"]) for linha in threads]
    vazoes_threads = [float(linha["vazao_agregada_mib_s"]) for linha in threads]
    speedups = [float(linha["speedup"]) for linha in threads]
    ax.plot(quantidades, vazoes_threads, marker="o", color=CORES["verde"], linewidth=1.5)
    ax.set_title("(c) Lotes independentes em paralelo")
    ax.set_xlabel("Threads (8 núcleos / 16 lógicos)")
    ax.set_ylabel("Vazão agregada (MiB/s)")
    ax.set_xticks(quantidades)
    for x, y, speedup in zip(quantidades, vazoes_threads, speedups):
        ax.annotate(f"{speedup:.1f}x", (x, y), xytext=(0, 5), textcoords="offset points",
                    ha="center", fontsize=6.5)

    ax = eixos[1, 1]
    letras = [int(linha["letras"]) for linha in ataque]
    ax.plot(letras, [float(linha["estimador_ms"]) for linha in ataque], marker="s",
            markersize=3.5, color=CORES["laranja"], label="Somente estimador de L")
    ax.plot(letras, [float(linha["pipeline_ms"]) for linha in ataque], marker="o",
            markersize=3.5, color=CORES["vermelho"], label="Pipeline, 5 candidatos")
    ax.set_xscale("log", base=2)
    ax.set_yscale("log", base=2)
    ax.set_xticks(letras)
    ax.set_xticklabels([str(valor) for valor in letras], rotation=35, ha="right")
    ax.set_yticks([0.02, 0.05, 0.1, 0.2, 0.5, 1.0, 2.0])
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_title("(d) Custo do ataque")
    ax.set_xlabel("Letras do criptograma (escala log)")
    ax.set_ylabel("Tempo mediano (ms)")
    ax.legend(frameon=False)

    salvar(figura, "desempenho")


def resumo() -> None:
    interceptacao = ler_csv("interceptacao.csv")
    linhas_l9 = [
        linha for linha in interceptacao
        if linha["modo"] == "ordem_original" and linha["tamanho_chave"] == "9"
    ]
    primeiro_90 = next(
        linha for linha in linhas_l9 if float(linha["sucesso_pipeline"]) >= 0.90
    )
    threads = ler_csv("desempenho_threads.csv")
    melhor = max(threads, key=lambda linha: float(linha["vazao_agregada_mib_s"]))
    print(
        "L=9 ultrapassa 90% em "
        f"N/L={primeiro_90['letras_por_posicao']} (N={primeiro_90['letras_texto']}); "
        f"melhor vazão paralela: {float(melhor['vazao_agregada_mib_s']):.1f} MiB/s "
        f"com {melhor['threads']} threads ({float(melhor['speedup']):.2f}x)."
    )


def main() -> None:
    configurar_estilo()
    grafico_frequencias_ioc()
    grafico_interceptacao()
    grafico_desempenho()
    resumo()


if __name__ == "__main__":
    main()
