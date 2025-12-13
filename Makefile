# Forçar pastas temporárias para evitar erro de permissão no Windows
export TMP := ./tmp_build
export TEMP := ./tmp_build

# Garantir que a pasta tmp_build exista antes de compilar
PREPARE_TMP := $(shell mkdir -p $(TMP))

CC = gcc
# Adicionei -Iinclude para ele achar o globals.h e util.h
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
# Nota: C_SRC não é mais usado nas regras abaixo para evitar confusão, 
# mas mantemos aqui para referência se precisar
C_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/util.c 

# Objetos gerados
OBJS = $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/main.o $(OBJ_DIR)/util.o $(OBJ_DIR)/symtab.o

all: create_dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Compilacao concluida com sucesso! Execute: ./$(TARGET)"

# Regra para o Bison
$(OBJ_DIR)/parser.tab.o: $(Y_SRC)
	$(YACC) -d $(Y_SRC) -o $(SRC_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/parser.tab.c -o $@

# Regra para o Flex
$(OBJ_DIR)/lex.yy.o: $(L_SRC) $(SRC_DIR)/parser.tab.c
	$(LEX) -o $(SRC_DIR)/lex.yy.c $(L_SRC)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/lex.yy.c -o $@

# Regra ESPECÍFICA para main.o (apenas main.c)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $@

# Regra ESPECÍFICA para util.o (apenas util.c)
$(OBJ_DIR)/util.o: $(SRC_DIR)/util.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/util.c -o $@

$(OBJ_DIR)/symtab.o: $(SRC_DIR)/symtab.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/symtab.c -o $@

# Cria diretório de objetos
create_dirs:
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h $(TARGET).exe