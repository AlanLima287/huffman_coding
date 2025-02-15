# Trabalho Prático II

## Autores

@Alan Barbosa Lima [AlanLima287](https://github.com/AlanLima287)             <br>
@Breno Augusto Braga Oliveira [bragabreno](https://github.com/bragabreno)    <br>
@Juan Pablo Ferreira Costa [juanzinhobs18](https://github.com/juanzinhobs18) <br>
@Luiz Felipe Melo Oliveira                                                   <br>
@Otávio Gomes Calazans

# Do Trabalho

Esse Trabalho foi desenvolvido como projeto acadêmico da disciplina de ALGORITMOS E ESTRUTURAS DE DADOS III, do Curso de Graduação em Sistemas de Informação da Universidade Federal dos Vales do Jequitinhonha e Mucuri, sob comando da Professora Luciana de Assis.

# Como Usar

## Como compilar

Há um árquivo próprio para a compiliação, que faz, parcialmente, a função de um linker, este é o arquivo localizado em ./build/build.cpp, para compilá-lo pode-se executar:

```sh
g++ ./build/build.cpp -O2 -o clue
```

Ou pode-se usar o make.sh (para sistemas Linux) ou o make.bat (para Windows).

## Executando o programa

O programa caracteriza uma aplicação de terminal, esse conta com três desvios de funcionamento: ajuda, codificação e decodificação.

### Ajuda

Essa é chamada por meio da flag `-h`, se essa flag estiver presente, em qualquer posição, todo o resto é ignorado. Chamando o programa com essa entrada:

```
> ./clue -h
```

Esse responderá imprimindo:

```
clue -h
   Mostra essa tela
clue <arquivo>
   Se <arquivo> = <nome>.huf, decodifica-se <arquivo> e põe os resultado em <nome>,
   caso contrário, codifica-se <arquivo> e põe o resultado em <arquivo>.huf.
clue -d <arquivo de entrada> <arquivo de saída>
   Decodifica <arquivo de entrada> e põe o resultado em <arquivo de saída>.
clue -e <arquivo de entrada> <arquivo de saída>
   Codifica <arquivo de entrada> e põe o resultado em <arquivo de saída>.
clue -s ...
   Adicinando -s a uma chamada mostrará a árvore de Huffman gerada.
```

### Codificando e Decodificando

Ao chamar o programa com apenas um nome de arquivo como argumento, esse verificará se o arquivo tem a extensão .huf. Se esse for o caso, este decodifica o arquivo e põe o resultado num arquivo de nome tal que concatenado .huf ao final ficaria igual ao nome de entrada. Caso contrário, o arquivo é codificado e o resultado é posto num arquivo com o mesmo nome e com .huf anexado ao final deste.

```
> ls
a.txt
> ./clue a.txt
Tamanho original: 9 bytes
Tamanho após compressão: 13 bytes
Taxa de compressão: -44.4444%
> ls
a.txt a.txt.huf
```

Pode-se ser mais explicito sobre o que se deseja fazer, adicionando as flags `-e` e `-d`:

* com `-e`, o primeiro nome de arquivo será codificado para o segundo, caso o segundo arquivo fornecido não tenha a extensão .huf, essa será adicionada;

```
> ls
a.txt
> ./clue -e a.txt a.huf
Tamanho original: 124 bytes
Tamanho após compressão: 70 bytes
Taxa de compressão: 43.5484%
> ls
a.huf a.txt
```

* com `-d`, o primeiro nome de arquivo será decodificado para o segundo, sem qualquer tratamento de nome.

```
> ls
a.huf
> ./clue -d a.huf a.txt
> ls
a.huf a.txt
```

Ademais, a flag `-s` pode ser adicionada a qualquer um dos casos dessa subsecção, com esta, a árvore de Huffman da codificação/decodificação:

```
> ls
a.txt
> ./clue -s -e a.txt a.huf
┬──┬──┬──: t
│  │  └──: e
│  └──┬──: o
│     └──┬──: m
│        └──┬──: r
│           └──┬──: a
│              └──: q
└──┬──: ' '
   └──┬──: p
      └──┬──: u
         └──┬──: n
            └──┬──┬──: g
               │  └──: s
               └──┬──: ,
                  └──: d
Tamanho original: 124 bytes
Tamanho após compressão: 70 bytes
Taxa de compressão: 43.5484%
```
