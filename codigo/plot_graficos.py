from __future__ import annotations

import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


ARQUIVO_RESULTADOS = Path("ultimo_compare_sorts.csv")


def filtrar_pontos(xs: list[int], ys: list[float | None]) -> tuple[list[int], list[float]]:
    pontos_x: list[int] = []
    pontos_y: list[float] = []

    for x, y in zip(xs, ys, strict=True):
        if y is None:
            continue
        pontos_x.append(x)
        pontos_y.append(y)

    return pontos_x, pontos_y


def carregar_resultados(caminho: Path) -> tuple[list[int], list[float], list[float]]:
    tamanhos: list[int] = []
    insertion_ms: list[float] = []
    quick_ms: list[float] = []

    with caminho.open("r", newline="", encoding="utf-8") as arquivo:
        leitor = csv.DictReader(arquivo)
        for linha in leitor:
            tamanhos.append(int(linha["n"]))
            insertion_ms.append(float(linha["insertion_ms"]))
            quick_ms.append(float(linha["quick_ms"]))

    if not tamanhos:
        raise ValueError("Arquivo de resultados vazio.")

    combinados = sorted(zip(tamanhos, insertion_ms, quick_ms), key=lambda item: item[0])
    tamanhos = [item[0] for item in combinados]
    insertion_ms = [item[1] for item in combinados]
    quick_ms = [item[2] for item in combinados]
    return tamanhos, insertion_ms, quick_ms


def plotar_serie(ax: plt.Axes, xs: list[int], ys: list[float], label: str, cor: str) -> None:
    pontos_x, pontos_y = filtrar_pontos(xs, ys)
    ax.plot(pontos_x, pontos_y, marker="o", linewidth=2.5, markersize=7, label=label, color=cor)

    for x, y in zip(pontos_x, pontos_y, strict=True):
        ax.annotate(
            f"{y} ms",
            (x, y),
            textcoords="offset points",
            xytext=(0, 8),
            ha="center",
            fontsize=9,
        )


def criar_grafico(entrada: Path, saida: Path) -> None:
    tamanhos, insercao_ms, quick_ms = carregar_resultados(entrada)

    plt.style.use("seaborn-v0_8-whitegrid")

    figura, eixos = plt.subplots(1, 2, figsize=(14, 6), constrained_layout=True)

    plotar_serie(eixos[0], tamanhos, insercao_ms, "Insertion Sort", "#c0392b")
    plotar_serie(eixos[0], tamanhos, quick_ms, "Quick Sort", "#1f77b4")
    eixos[0].set_title("Tempo de execução - escala linear")
    eixos[0].set_xlabel("Tamanho da entrada (n)")
    eixos[0].set_ylabel("Tempo (ms)")
    eixos[0].set_xticks(tamanhos, [str(n) for n in tamanhos])
    eixos[0].legend()

    plotar_serie(eixos[1], tamanhos, insercao_ms, "Insertion Sort", "#c0392b")
    plotar_serie(eixos[1], tamanhos, quick_ms, "Quick Sort", "#1f77b4")
    eixos[1].set_title("Tempo de execução - escala logarítmica")
    eixos[1].set_xlabel("Tamanho da entrada (n)")
    eixos[1].set_ylabel("Tempo (ms)")
    eixos[1].set_yscale("log")
    eixos[1].set_xticks(tamanhos, [str(n) for n in tamanhos])
    eixos[1].legend()

    figura.suptitle("Comparacao entre Insertion Sort e Quick Sort", fontsize=14, fontweight="bold")
    figura.savefig(saida, dpi=200, bbox_inches="tight")
    plt.close(figura)


def main() -> None:
    parser = argparse.ArgumentParser(description="Gera graficos Matplotlib com os tempos dos experimentos.")
    parser.add_argument(
        "--entrada",
        type=Path,
        default=ARQUIVO_RESULTADOS,
        help="Arquivo CSV gerado pela ultima execucao do compare_sorts.",
    )
    parser.add_argument(
        "--saida",
        type=Path,
        default=Path("grafico_comparacao.png"),
        help="Arquivo de saida da imagem gerada.",
    )
    args = parser.parse_args()

    if not args.entrada.exists():
        raise FileNotFoundError(
            f"Arquivo de resultados nao encontrado: {args.entrada}. Execute o compare_sorts antes."
        )

    criar_grafico(args.entrada, args.saida)
    print(f"Grafico gerado em: {args.saida.resolve()}")


if __name__ == "__main__":
    main()