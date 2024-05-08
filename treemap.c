#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


typedef struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
} TreeNode;

typedef struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
} TreeMap;

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap* createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap* newTreeMap = (TreeMap*) malloc(sizeof(TreeMap));
    if (newTreeMap == NULL) {
        return NULL; 
    }
    newTreeMap->root = NULL; 
    newTreeMap->current = NULL; 
    newTreeMap->lower_than = lower_than; 
    return newTreeMap;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {

    // Si el árbol está vacío, creamos un nuevo nodo y lo asignamos como raíz
    if (tree->root == NULL) {
        TreeNode *newNode = createTreeNode(key, value);
        tree->root = newNode;
        tree->current = newNode;
        return;
    }
    TreeNode *root = tree->root;

    // Hacemos la comparación de los valores de los nodos
    // Dependiendo de la llave, se decide si se va a la izquierda o derecha
    while (root != NULL) {
        if (is_equal(tree, root->pair->key, key)) {
            return;
        }
        if (tree->lower_than(key, root->pair->key)) {
            if (root->left == NULL) {
                TreeNode *newNode = createTreeNode(key, value);
                root->left = newNode;
                newNode->parent = root;
                tree->current = newNode; // Actualizamos el puntero current
                return;
            }
            root = root->left;
        } else {
            if (root->right == NULL) {
                TreeNode *newNode = createTreeNode(key, value);
                root->right = newNode;
                newNode->parent = root;
                tree->current = newNode; // Actualizamos el puntero current
                return;
            }
            root = root->right;
        }
    }
}

/*
Función auxiliar recursiva para encontrar el nodo con el valor mínimo.
Se recorre el árbol hacia la izquierda hasta que no haya más nodos a la izquierda.
*/ 

TreeNode * minimum(TreeNode * x){
    if (x->left == NULL) {
        return x;
    } else {
        return minimum(x->left);
    }
}



void removeNode(TreeMap * tree, TreeNode* node) {

    /*
    Caso 1: Sin hijos
    Si el nodo simplemente no tiene hijos, se elimina el nodo mismo y se libera la memoria.
    */ 

    if (node->left == NULL && node->right == NULL) { 
        if (node->parent->left == node) {
            node->parent->left = NULL;
        } else {
            node->parent->right = NULL;
        }
        free(node);

    } 

    /*
    Caso 3: Dos hijos
    Si el nodo tiene DOS hijos, necesitamos encontrar el sucesor (ordenado) del nodo.
    (El sucesor mas pequeño con una llave mayor que la llave del nodo a eliminar),
    reemplazar el nodo con el sucesor y eliminar el sucesor.
    */ 

    else if (node->left != NULL && node->right != NULL) 
    { 
        TreeNode* successor = minimum(node->right);
        node->pair = successor->pair;
        removeNode(tree, successor);
    }

    /*
    Caso 2: Un hijo
    Si el nodo tiene un hijo, necesitamos reemplazar el nodo con su hijo
    y liberar la memoria de ese nodo.
    */ 

    else 
    { 
        TreeNode* child = (node->left != NULL) ? node->left : node->right;
        if (node->parent->left == node) {
            node->parent->left = child;
        } else {
            node->parent->right = child;
        }
        child->parent = node->parent;
        free(node);
    }
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}

Pair *searchTreeMap(TreeMap * tree, void* key) {
    TreeNode *current = tree->root;

    // Se itera el arbol de manera binaria aprovechando la propiedad de orden
    while (current != NULL) {
        // Si se encuentra la llave, se actualiza el puntero current y se retorna el par
        if (is_equal(tree, current->pair->key, key)) {
            tree->current = current;
            return current->pair;
        } else if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return NULL; // Retorna NULL si no es encontrado
}


Pair * upperBound(TreeMap * tree, void* key) {
    return NULL;
}

Pair *firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode *current = tree->root;
    while (current->left != NULL) {
        current = current->left;
    }
    tree->current = current;
    return current->pair;
}

Pair *nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;

    TreeNode *current = tree->current;

    if (current->right != NULL) {
        current = current->right;
        while (current->left != NULL) {
            current = current->left;
        }
        tree->current = current;
        return current->pair;
    }

    TreeNode *parent = current->parent;
    while (parent != NULL && current == parent->right) {
        current = parent;
        parent = current->parent;
    }

    if (parent == NULL) return NULL;

    tree->current = parent;
    return parent->pair;
}
