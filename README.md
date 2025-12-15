# Compilador C-

Este projeto implementa um compilador para a linguagem C- (C-Minus), baseado na especificação do livro *Compiler Construction: Principles and Practice* de Kenneth C. Louden. O projeto foi desenvolvido como Trabalho Final da disciplina de Compiladores.

## Autores

* **Álvaro Neves Stein**
* **João Vitor Evangelista de Souza**

## Sobre o Projeto

O compilador realiza as etapas de análise léxica, sintática, semântica e geração de código intermediário.

### Funcionalidades Implementadas:
1.  **Scanner (Léxico):**
    * Utiliza Flex.
    * Reconhece tokens da linguagem C-.
    * Suporte a identificadores contendo números e *underscores* (ex: `var_1`, `soma_total`).
    * Exibe erros léxicos com número da linha.
2.  **Parser (Sintático):**
    * Utiliza Bison.
    * Gera a Árvore Sintática Abstrata (AST).
    * Possui recuperação de erros (modo *panic*) para declarações e comandos.
3.  **Tabela de Símbolos:**
    * Implementada com Tabela Hash.
    * Suporte a escopos (variáveis locais e globais distinguíveis).
    * Armazena e verifica tipos (`int`, `void`).
4.  **Analisador Semântico:**
    * Verificação de tipos em expressões e atribuições.
    * Verificação de declarações duplicadas e variáveis não declaradas.
    * Verificação da existência da função `main`.
5.  **Geração de Código:**
    * Gera código de 3 endereços linearizado.

## Pré-requisitos e Ambiente

O projeto foi desenvolvido e testado em ambiente **Windows** utilizando o terminal **MSYS2 (MinGW64)**.

Para compilar, é necessário ter instalado:
* GCC (Compilador C)
* Make
* Flex
* Bison

### Instalação das dependências (MSYS2)
Se estiver utilizando o MSYS2, execute o comando abaixo para instalar as ferramentas necessárias:

```bash
pacman -S mingw-w64-x86_64-toolchain base-devel flex bison
```

## Compilação e Execução

### 1. Compilar o Projeto

Na raiz do projeto (onde está o `Makefile`), execute:

```bash
make
```

Isso irá gerar os arquivos objetos na pasta `obj/` e o executável `compilador.exe`.

### 2. Executar

Para rodar o compilador, passe o arquivo de código-fonte `.cm` como argumento:

```bash
./compilador tests/5_fatorial.cm
```

### 3. Limpar Arquivos Temporários

Para remover os arquivos objetos e o executável gerado:

```bash
make clean
```

## Estrutura de Arquivos

  * `src/`: Código-fonte (`.c`, `.l`, `.y`).
      * `scanner.l`: Definições léxicas (Flex).
      * `parser.y`: Gramática e construção da AST (Bison).
      * `analyze.c`: Análise semântica e verificação de tipos.
      * `symtab.c`: Implementação da Tabela de Símbolos.
      * `codegen.c`: Gerador de código intermediário.
  * `include/`: Cabeçalhos (`.h`).
  * `tests/`: Arquivos de teste na linguagem C-.
  * `obj/`: Arquivos objeto gerados durante a compilação.
  * `Makefile`: Script de automação da compilação.

## Exemplos de Saída

Ao executar com sucesso, o compilador exibirá no terminal:

1.  Status da Análise Sintática.
2.  **Árvore Sintática Abstrata (AST)** textual.
3.  **Tabela de Símbolos** detalhada (com Escopo, Tipo e Linhas).
4.  **Código Intermediário** gerado.

Exemplo de comando com erro proposital para teste:

```bash
./compilador tests/erro_tipo.cm
```