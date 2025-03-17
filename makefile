# Compilateurs
CC = gcc
CXX = g++

# Dossiers
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Extensions des fichiers sources
C_SRC_EXT = c
CPP_SRC_EXT = cpp

# Liste des fichiers sources C et C++
C_SRC_FILES = $(wildcard $(SRC_DIR)/*.$(C_SRC_EXT))
CPP_SRC_FILES = $(wildcard $(SRC_DIR)/*.$(CPP_SRC_EXT))

# Liste des fichiers objets pour C et C++
C_OBJ_FILES = $(C_SRC_FILES:$(SRC_DIR)/%.$(C_SRC_EXT)=$(OBJ_DIR)/%.o)
CPP_OBJ_FILES = $(CPP_SRC_FILES:$(SRC_DIR)/%.$(CPP_SRC_EXT)=$(OBJ_DIR)/%.o)

# Liste complète des fichiers objets
OBJ_FILES = $(C_OBJ_FILES) $(CPP_OBJ_FILES)

# Nom de l'exécutable final
EXEC = $(BIN_DIR)/program

# Flags de compilation
CFLAGS = -Wall -Wextra -g
CXXFLAGS = -Wall -Wextra -g

# Règle par défaut (création de l'exécutable)
all: $(EXEC)

# Création du dossier bin si nécessaire
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Création de l'exécutable
$(EXEC): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) -o $@ $^

# Compilation des fichiers C en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(C_SRC_EXT)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compilation des fichiers C++ en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(CPP_SRC_EXT)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Nettoyage des fichiers générés
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Refaire tout (nettoyer + recompiler)
rebuild: clean all
