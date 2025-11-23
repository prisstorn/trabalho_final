#include "bst.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h> // Para medir o tempo com precisão

// =================================================================
// 1. FUNÇÕES DE CRIAÇÃO E DESTRUIÇÃO
// =================================================================

// Cria um novo Códice na memória
Codice* createCodice(const char *titulo, const char *autor, int id) {
    Codice *newCodice = (Codice *)malloc(sizeof(Codice));
    if (newCodice == NULL) {
        perror("Erro ao alocar Codice");
        exit(EXIT_FAILURE);
    }
    strncpy(newCodice->titulo, titulo, 99);
    strncpy(newCodice->autor, autor, 99);
    newCodice->id_registro = id;
    return newCodice;
}

// Cria um novo Nó da BST
Node* createNode(Codice *codice) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Erro ao alocar Node");
        exit(EXIT_FAILURE);
    }
    newNode->data = codice;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Libera a memória do Códice
void freeCodice(Codice *codice) {
    if (codice != NULL) {
        free(codice);
    }
}

// Libera a árvore inteira. O parâmetro free_codice_data garante que o Codice
// seja liberado apenas uma vez (pela BST de ID, por exemplo).
void destroyTree(Node *root, int free_codice_data) {
    if (root != NULL) {
        destroyTree(root->left, free_codice_data);
        destroyTree(root->right, free_codice_data);
        if (free_codice_data) {
            freeCodice(root->data);
        }
        free(root);
    }
}

// =================================================================
// 2. FUNÇÕES DE INSERÇÃO
// =================================================================

// Insere um nó na BST de ID (comparação numérica)
Node* insertNode_ID(Node *root, Codice *codice) {
    if (root == NULL) {
        return createNode(codice);
    }

    if (codice->id_registro < root->data->id_registro) {
        root->left = insertNode_ID(root->left, codice);
    } else if (codice->id_registro > root->data->id_registro) {
        root->right = insertNode_ID(root->right, codice);
    }
    // Se for duplicado, apenas retorna a raiz (não insere)
    return root;
}

// Insere um nó na BST de Título (comparação alfabética com strcmp)
Node* insertNode_Title(Node *root, Codice *codice) {
    if (root == NULL) {
        return createNode(codice);
    }

    int cmp = strcmp(codice->titulo, root->data->titulo);

    if (cmp < 0) {
        root->left = insertNode_Title(root->left, codice);
    } else if (cmp > 0) {
        root->right = insertNode_Title(root->right, codice);
    }
    // Se for duplicado, apenas retorna a raiz
    return root;
}

// =================================================================
// 3. FUNÇÕES DE BUSCA
// =================================================================

// Busca por ID. O ponteiro 'comparisons' conta quantos nós foram visitados.
Node* searchNode_ID(Node *root, int id, int *comparisons) {
    if (root == NULL) {
        return NULL;
    }

    (*comparisons)++; // Conta a visita
    if (id == root->data->id_registro) {
        return root;
    } else if (id < root->data->id_registro) {
        return searchNode_ID(root->left, id, comparisons);
    } else {
        return searchNode_ID(root->right, id, comparisons);
    }
}

// Busca por Título. O ponteiro 'comparisons' conta quantos nós foram visitados.
Node* searchNode_Title(Node *root, const char *title, int *comparisons) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = strcmp(title, root->data->titulo);
    (*comparisons)++; // Conta a visita

    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return searchNode_Title(root->left, title, comparisons);
    } else {
        return searchNode_Title(root->right, title, comparisons);
    }
}

// =================================================================
// 4. FUNÇÕES DE REMOÇÃO
// =================================================================

// Acha o nó com o menor valor na subárvore (o sucessor in-order)
Node* findMin(Node *node) {
    Node *current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Remove um nó da BST de ID.
Node* deleteNode_ID(Node *root, int id) {
    if (root == NULL) return root;

    // 1. Encontra o nó
    if (id < root->data->id_registro) {
        root->left = deleteNode_ID(root->left, id);
    } else if (id > root->data->id_registro) {
        root->right = deleteNode_ID(root->right, id);
    } else {
        // 2. Nó encontrado - Trata os 3 casos

        // Caso 1 e 2: Zero ou um filho
        if (root->left == NULL) {
            Node *temp = root->right;
            // Não liberamos root->data aqui, pois ele é compartilhado!
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node *temp = root->left;
            // Não liberamos root->data aqui, pois ele é compartilhado!
            free(root);
            return temp;
        }

        // Caso 3: Dois filhos
        Node *temp = findMin(root->right);

        // Copia o ponteiro de dados do sucessor para o nó atual
        root->data = temp->data;

        // Remove o sucessor in-order da subárvore direita
        root->right = deleteNode_ID(root->right, temp->data->id_registro);
    }
    return root;
}

// Remove um nó da BST de Título.
Node* deleteNode_Title(Node *root, const char *title) {
    if (root == NULL) return root;

    int cmp = strcmp(title, root->data->titulo);

    // 1. Encontra o nó
    if (cmp < 0) {
        root->left = deleteNode_Title(root->left, title);
    } else if (cmp > 0) {
        root->right = deleteNode_Title(root->right, title);
    } else {
        // 2. Nó encontrado - Trata os 3 casos

        // Caso 1 e 2: Zero ou um filho
        if (root->left == NULL) {
            Node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node *temp = root->left;
            free(root);
            return temp;
        }

        // Caso 3: Dois filhos
        Node *temp = findMin(root->right);

        // Copia o ponteiro de dados do sucessor para o nó atual
        root->data = temp->data;

        // Remove o sucessor in-order da subárvore direita
        root->right = deleteNode_Title(root->right, temp->data->titulo);
    }
    return root;
}

// =================================================================
// 5. FUNÇÕES DE TRAVESSIA
// =================================================================

// Percorre a árvore In-Order para listar os livros em ordem
void traverseInOrder(Node *root) {
    if (root != NULL) {
        traverseInOrder(root->left);
        printf("ID: %d | Título: %s | Autor: %s\n",
               root->data->id_registro, root->data->titulo, root->data->autor);
        traverseInOrder(root->right);
    }
}

// =================================================================
// 6. FUNÇÕES DE MÉTRICAS
// =================================================================

// Calcula a altura da árvore (o caminho mais longo)
int calculateHeight(Node *root) {
    if (root == NULL) {
        return -1; // Árvore vazia tem altura -1
    } else {
        int left_height = calculateHeight(root->left);
        int right_height = calculateHeight(root->right);

        // Retorna o maior lado + 1 (para contar o nó atual)
        return (left_height > right_height ? left_height : right_height) + 1;
    }
}

// A função measureTime foi removida daqui, pois a medição de tempo deve ser feita
// diretamente no 'main' ou no 'gui' antes e depois da chamada de busca,
// usando gettimeofday() para maior precisão.

// =================================================================
// 7. FUNÇÕES DE I/O
// =================================================================

// Carrega os dados do arquivo e monta as duas BSTs
CodiceEternal* loadCatalogFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Arquivo de catálogo não encontrado. Iniciando com catálogo vazio.\n");
        return NULL;
    }

    CodiceEternal *ce = (CodiceEternal *)malloc(sizeof(CodiceEternal));
    ce->root_id = NULL;
    ce->root_title = NULL;

    char line[256];
    // Lê linha por linha
    while (fgets(line, sizeof(line), file)) {
        // Espera o formato: ID;TITULO;AUTOR
        int id;
        char titulo[100], autor[100];

        if (sscanf(line, "%d;%99[^;];%99[^\n]", &id, titulo, autor) == 3) {
            Codice *newCodice = createCodice(titulo, autor, id);
            // Insere o mesmo dado nas duas árvores
            ce->root_id = insertNode_ID(ce->root_id, newCodice);
            ce->root_title = insertNode_Title(ce->root_title, newCodice);
        }
    }

    fclose(file);
    return ce;
}

// Função auxiliar para salvar um nó no arquivo
void saveNodeToFile(Node *root, FILE *file) {
    if (root != NULL) {
        // Salva no formato: ID;TITULO;AUTOR\n
        fprintf(file, "%d;%s;%s\n",
                root->data->id_registro,
                root->data->titulo,
                root->data->autor);

        saveNodeToFile(root->left, file);
        saveNodeToFile(root->right, file);
    }
}

// Salva o estado atual da BST (usamos a de ID) no arquivo
void saveCatalogToFile(Node *root, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir arquivo para salvar");
        return;
    }

    saveNodeToFile(root, file);

    fclose(file);
    printf("Catálogo salvo com sucesso em %s.\n", filename);
}
