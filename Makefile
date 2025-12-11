# Forçar pastas temporárias para evitar erro de permissão no Windows
export TMP := ./tmp_build
export TEMP := ./tmp_build

# Garantir que a pasta tmp_build exista antes de compilar
PREPARE_TMP := $(shell mkdir -p $(TMP))

CC = gcc
CFLAGS = -Iinclude -Wall

YACC = bison
LEX = flex

# Arquivos de saída
TARGET = compilador
OBJ_DIR = obj
SRC_DIR = src

# Fontes
L_SRC = $(SRC_DIR)/scanner.l
Y_SRC = $(SRC_DIR)/parser.y
C_SRC = $(SRC_DIR)/main.c

# Objetos gerados
OBJS = $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/main.o

all: create_dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Compilacao concluida com sucesso! Execute: ./$(TARGET)"

# Regra para o Bison
# Gera o parser.tab.c e o parser.tab.h (o -d cria o .h)
$(OBJ_DIR)/parser.tab.o: $(Y_SRC)
	$(YACC) -d $(Y_SRC) -o $(SRC_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/parser.tab.c -o $@

# Regra para o Flex (Léxico)
# Gera o lex.yy.c
$(OBJ_DIR)/lex.yy.o: $(L_SRC) $(SRC_DIR)/parser.tab.c
	$(LEX) -o $(SRC_DIR)/lex.yy.c $(L_SRC)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/lex.yy.c -o $@

# Regra para arquivos C genéricos (como main.c)
$(OBJ_DIR)/main.o: $(C_SRC)
	$(CC) $(CFLAGS) -c $(C_SRC) -o $@

# Cria diretório de objetos se não existir
create_dirs:
	@mkdir -p $(OBJ_DIR)

# Limpeza dos arquivos gerados
clean:
	rm -rf $(OBJ_DIR) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h $(TARGET).exe