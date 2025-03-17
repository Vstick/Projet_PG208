# Compilateurs
CXX = g++

# Dossiers
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Extension des fichiers sources
CPP_SRC_EXT = cpp

# Liste des fichiers sources C++
CPP_SRC_FILES = $(wildcard $(SRC_DIR)/*.$(CPP_SRC_EXT))

# Liste des fichiers objets pour C++
OBJ_FILES = $(CPP_SRC_FILES:$(SRC_DIR)/%.$(CPP_SRC_EXT)=$(OBJ_DIR)/%.o)

# Nom de l'exécutable final
EXEC = $(BIN_DIR)/program

# Flags de compilation
CXXFLAGS = -Wall -Wextra -g

# Règle par défaut (création de l'exécutable)
all: $(EXEC)

# Création du dossier bin si nécessaire
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Création de l'exécutable
$(EXEC): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) -o $@ $^

# Compilation des fichiers C++ en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(CPP_SRC_EXT)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Nettoyage des fichiers générés
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Refaire tout (nettoyer + recompiler)
rebuild: clean all
