#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PALABRA 100
#define MAX_LINEA 10000
#define MAX_URL 500
#define MAX_CONTENIDO 5000
#define MAX_DOCUMENTOS 100
#define MAX_CONSULTAS 100

// Estructura para un documento
typedef struct Documento {
    int id;
    char contenido[MAX_CONTENIDO];
    char url[MAX_URL];
    struct Documento* siguiente;
} Documento;

// Nodo para lista de IDs de documentos
typedef struct NodoDocumento {
    int id;
    struct NodoDocumento* siguiente;
} NodoDocumento;

// Estructura para una entrada en el índice invertido
typedef struct EntradaIndice {
    char palabra[MAX_PALABRA];
    NodoDocumento* documentos;
    struct EntradaIndice* siguiente;
} EntradaIndice;

// Nodo para lista de stop words
typedef struct NodoStopWord {
    char palabra[MAX_PALABRA];
    struct NodoStopWord* siguiente;
} NodoStopWord;

// Estructura principal del buscador
typedef struct BuscadorDocumentos {
    Documento* documentos;
    EntradaIndice* indiceInvertido;
    NodoStopWord* stopWords;
    int totalDocumentos;
    int totalTerminos;
    int totalStopWords;
} BuscadorDocumentos;

// Funciones auxiliares
void toLower(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void limpiarPalabra(const char* palabra, char* resultado) {
    int j = 0;
    for (int i = 0; palabra[i]; i++) {
        if (isalnum(palabra[i])) {
            resultado[j++] = tolower(palabra[i]);
        }
    }
    resultado[j] = '\0';
}

// Función para agregar un documento a la lista
void agregarDocumento(BuscadorDocumentos* buscador, int id, const char* contenido, const char* url) {
    Documento* nuevo = (Documento*)malloc(sizeof(Documento));
    if (nuevo == NULL) {
        printf("Error: No se pudo asignar memoria para documento\n");
        return;
    }
    
    nuevo->id = id;
    strncpy(nuevo->contenido, contenido, MAX_CONTENIDO - 1);
    nuevo->contenido[MAX_CONTENIDO - 1] = '\0';
    strncpy(nuevo->url, url, MAX_URL - 1);
    nuevo->url[MAX_URL - 1] = '\0';
    nuevo->siguiente = buscador->documentos;
    buscador->documentos = nuevo;
    buscador->totalDocumentos++;
}

// Función para agregar una stop word
void agregarStopWord(BuscadorDocumentos* buscador, const char* palabra) {
    NodoStopWord* nuevo = (NodoStopWord*)malloc(sizeof(NodoStopWord));
    if (nuevo == NULL) {
        printf("Error: No se pudo asignar memoria para stop word\n");
        return;
    }
    
    strncpy(nuevo->palabra, palabra, MAX_PALABRA - 1);
    nuevo->palabra[MAX_PALABRA - 1] = '\0';
    nuevo->siguiente = buscador->stopWords;
    buscador->stopWords = nuevo;
    buscador->totalStopWords++;
}

// Verificar si una palabra es stop word
int esStopWord(BuscadorDocumentos* buscador, const char* palabra) {
    char palabraLower[MAX_PALABRA];
    strncpy(palabraLower, palabra, MAX_PALABRA - 1);
    palabraLower[MAX_PALABRA - 1] = '\0';
    toLower(palabraLower);
    
    NodoStopWord* actual = buscador->stopWords;
    while (actual != NULL) {
        if (strcmp(actual->palabra, palabraLower) == 0) {
            return 1;
        }
        actual = actual->siguiente;
    }
    return 0;
}

// Función para agregar un documento a la lista de IDs
void agregarDocumentoALista(NodoDocumento** lista, int id) {
    // Verificar si ya existe
    NodoDocumento* actual = *lista;
    while (actual != NULL) {
        if (actual->id == id) {
            return; // Ya existe
        }
        actual = actual->siguiente;
    }
    
    // Agregar nuevo
    NodoDocumento* nuevo = (NodoDocumento*)malloc(sizeof(NodoDocumento));
    if (nuevo == NULL) {
        printf("Error: No se pudo asignar memoria para nodo documento\n");
        return;
    }
    
    nuevo->id = id;
    nuevo->siguiente = *lista;
    *lista = nuevo;
}

// Función para encontrar o crear entrada en índice invertido
EntradaIndice* encontrarOCrearEntrada(BuscadorDocumentos* buscador, const char* palabra) {
    EntradaIndice* actual = buscador->indiceInvertido;
    
    // Buscar si ya existe
    while (actual != NULL) {
        if (strcmp(actual->palabra, palabra) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    
    // Crear nueva entrada
    EntradaIndice* nueva = (EntradaIndice*)malloc(sizeof(EntradaIndice));
    if (nueva == NULL) {
        printf("Error: No se pudo asignar memoria para entrada del índice\n");
        return NULL;
    }
    
    strncpy(nueva->palabra, palabra, MAX_PALABRA - 1);
    nueva->palabra[MAX_PALABRA - 1] = '\0';
    nueva->documentos = NULL;
    nueva->siguiente = buscador->indiceInvertido;
    buscador->indiceInvertido = nueva;
    buscador->totalTerminos++;
    
    return nueva;
}

// Función para procesar texto y agregar palabras al índice
void procesarTexto(BuscadorDocumentos* buscador, const char* texto, int docId) {
    char textoCopia[MAX_CONTENIDO];
    strncpy(textoCopia, texto, MAX_CONTENIDO - 1);
    textoCopia[MAX_CONTENIDO - 1] = '\0';
    
    char* token = strtok(textoCopia, " \t\n\r.,;:!?()[]{}\"'");
    
    while (token != NULL) {
        char palabraLimpia[MAX_PALABRA];
        limpiarPalabra(token, palabraLimpia);
        
        if (strlen(palabraLimpia) > 0 && !esStopWord(buscador, palabraLimpia)) {
            EntradaIndice* entrada = encontrarOCrearEntrada(buscador, palabraLimpia);
            if (entrada != NULL) {
                agregarDocumentoALista(&(entrada->documentos), docId);
            }
        }
        
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}\"'");
    }
}

// Cargar stop words desde archivo
void cargarStopWords(BuscadorDocumentos* buscador, const char* archivo) {
    FILE* fp = fopen(archivo, "r");
    if (fp == NULL) {
        printf("Advertencia: No se pudo cargar el archivo de stop words.\n");
        return;
    }
    
    char palabra[MAX_PALABRA];
    while (fscanf(fp, "%99s", palabra) == 1) {  // Limitar lectura
        toLower(palabra);
        agregarStopWord(buscador, palabra);
    }
    
    fclose(fp);
    printf("Stop words cargadas: %d\n", buscador->totalStopWords);
}

// Cargar documentos desde archivo - VERSIÓN MEJORADA
void cargarDocumentos(BuscadorDocumentos* buscador, const char* archivoDocumentos, const char* archivoStopWords) {
    if (archivoStopWords != NULL && strlen(archivoStopWords) > 0) {
        cargarStopWords(buscador, archivoStopWords);
    }
    
    FILE* fp = fopen(archivoDocumentos, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", archivoDocumentos);
        return;
    }
    
    char* linea = (char*)malloc(MAX_LINEA * sizeof(char));
    if (linea == NULL) {
        printf("Error: No se pudo asignar memoria para línea\n");
        fclose(fp);
        return;
    }
    
    int docId = 1;
    
    printf("Cargando documentos...\n");
    
    while (fgets(linea, MAX_LINEA, fp) && docId <= MAX_DOCUMENTOS) {
        // Mostrar progreso cada 10 documentos
        if (docId % 10 == 0) {
            printf("Procesando documento %d...\n", docId);
            fflush(stdout);
        }
        
        // Remover salto de línea
        linea[strcspn(linea, "\n")] = '\0';
        
        if (strlen(linea) == 0) continue;
        
        // Buscar separador "||"
        char* separador = strstr(linea, "||");
        if (separador == NULL) {
            printf("Advertencia: linea sin separador válido en documento %d\n", docId);
            continue;
        } 
        // Dividir URL y contenido
        *separador = '\0';
        char* url = linea;
        char* contenido = separador + 2;
        
        // Verificar longitud del contenido
        if (strlen(contenido) >= MAX_CONTENIDO) {
            printf("Advertencia: Contenido muy largo en documento %d, truncando...\n", docId);
            contenido[MAX_CONTENIDO - 1] = '\0';
        }
        
        // Agregar documento
        agregarDocumento(buscador, docId, contenido, url);
        
        // Procesar contenido para índice invertido
        procesarTexto(buscador, contenido, docId);
        
        docId++;
    }
    
    free(linea);
    fclose(fp);
    printf("Documentos cargados: %d\n", buscador->totalDocumentos);
    printf("Indice construido con %d terminos unicos.\n", buscador->totalTerminos);
}

// Mostrar estadísticas
void mostrarEstadisticas(BuscadorDocumentos* buscador) {
    printf("\n=== ESTADISTICAS DEL INDICE ===\n");
    printf("Total de documentos: %d\n", buscador->totalDocumentos);
    printf("Total de terminos unicos: %d\n", buscador->totalTerminos);
    printf("Stop words cargadas: %d\n", buscador->totalStopWords);
}

// Buscar documentos que contengan una palabra
NodoDocumento* buscarPalabra(BuscadorDocumentos* buscador, const char* palabra) {
    char palabraLimpia[MAX_PALABRA];
    limpiarPalabra(palabra, palabraLimpia);
    
    EntradaIndice* actual = buscador->indiceInvertido;
    while (actual != NULL) {
        if (strcmp(actual->palabra, palabraLimpia) == 0) {
            return actual->documentos;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

// Función para copiar lista de documentos
NodoDocumento* copiarLista(NodoDocumento* lista) {
    if (lista == NULL) return NULL;
    
    NodoDocumento* nuevaLista = NULL;
    NodoDocumento* actual = lista;
    
    while (actual != NULL) {
        NodoDocumento* nuevo = (NodoDocumento*)malloc(sizeof(NodoDocumento));
        if (nuevo == NULL) {
            printf("Error: No se pudo asignar memoria para copia de lista\n");
            return nuevaLista;
        }
        nuevo->id = actual->id;
        nuevo->siguiente = nuevaLista;
        nuevaLista = nuevo;
        actual = actual->siguiente;
    }
    
    return nuevaLista;
}

// Función para hacer intersección de dos listas
NodoDocumento* interseccionListas(NodoDocumento* lista1, NodoDocumento* lista2) {
    if (lista1 == NULL || lista2 == NULL) return NULL;
    
    NodoDocumento* resultado = NULL;
    NodoDocumento* actual1 = lista1;
    
    while (actual1 != NULL) {
        NodoDocumento* actual2 = lista2;
        while (actual2 != NULL) {
            if (actual1->id == actual2->id) {
                agregarDocumentoALista(&resultado, actual1->id);
                break;
            }
            actual2 = actual2->siguiente;
        }
        actual1 = actual1->siguiente;
    }
    
    return resultado;
}

// Buscar documentos con múltiples palabras
NodoDocumento* buscarMultiplesPalabras(BuscadorDocumentos* buscador, char palabras[][MAX_PALABRA], int numPalabras) {
    if (numPalabras == 0) return NULL;
    
    NodoDocumento* resultado = copiarLista(buscarPalabra(buscador, palabras[0]));
    
    for (int i = 1; i < numPalabras; i++) {
        NodoDocumento* documentosActuales = buscarPalabra(buscador, palabras[i]);
        NodoDocumento* nuevaInterseccion = interseccionListas(resultado, documentosActuales);
        
        // Liberar memoria de resultado anterior
        while (resultado != NULL) {
            NodoDocumento* temp = resultado;
            resultado = resultado->siguiente;
            free(temp);
        }
        
        resultado = nuevaInterseccion;
    }
    
    return resultado;
}

// Mostrar resultados de búsqueda
void mostrarResultados(NodoDocumento* documentos, char palabras[][MAX_PALABRA], int numPalabras) {
    if (documentos == NULL) {
        printf("No se encontraron documentos que contengan ");
        if (numPalabras == 1) {
            printf("la palabra \"%s\".\n", palabras[0]);
        } else {
            printf("todas las palabras: ");
            for (int i = 0; i < numPalabras; i++) {
                if (i > 0) printf(", ");
                printf("\"%s\"", palabras[i]);
            }
            printf(".\n");
        }
        return;
    }
    
    if (numPalabras == 1) {
        printf("La palabra \"%s\" aparece en los documentos: ", palabras[0]);
    } else {
        printf("Los documentos que contienen todas las palabras son: ");
    }
    
    NodoDocumento* actual = documentos;
    int primero = 1;
    while (actual != NULL) {
        if (!primero) printf(", ");
        printf("doc%d", actual->id);
        primero = 0;
        actual = actual->siguiente;
    }
    printf("\n");
}

// Extraer palabras de una consulta
int extraerPalabras(BuscadorDocumentos* buscador, const char* consulta, char palabras[][MAX_PALABRA]) {
    char consultaCopia[MAX_LINEA];
    strncpy(consultaCopia, consulta, MAX_LINEA - 1);
    consultaCopia[MAX_LINEA - 1] = '\0';
    
    int numPalabras = 0;
    char* token = strtok(consultaCopia, " \t\n\r.,;:!?()[]{}\"'");
    
    while (token != NULL && numPalabras < 50) {
        char palabraLimpia[MAX_PALABRA];
        limpiarPalabra(token, palabraLimpia);
        
        if (strlen(palabraLimpia) > 0 && !esStopWord(buscador, palabraLimpia)) {
            strncpy(palabras[numPalabras], palabraLimpia, MAX_PALABRA - 1);
            palabras[numPalabras][MAX_PALABRA - 1] = '\0';
            numPalabras++;
        }
        
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}\"'");
    }
    
    return numPalabras;
}

// Procesar consulta
void procesarConsulta(BuscadorDocumentos* buscador, const char* consulta) {
    char palabras[50][MAX_PALABRA];
    int numPalabras = extraerPalabras(buscador, consulta, palabras);
    
    if (numPalabras == 0) {
        printf("Consulta vacia o solo contiene stop words.\n");
        return;
    }
    
    NodoDocumento* resultados = buscarMultiplesPalabras(buscador, palabras, numPalabras);
    
    printf("Consulta: %s\n", consulta);
    mostrarResultados(resultados, palabras, numPalabras);
    
    // Liberar memoria de resultados
    while (resultados != NULL) {
        NodoDocumento* temp = resultados;
        resultados = resultados->siguiente;
        free(temp);
    }
}

// Modo interactivo
void modoFrecuencia(BuscadorDocumentos* buscador) {
    printf("\n=== MODO FRECUENCIA ===\n");
    printf("Escriba su consulta o 'salir' para terminar.\n");
    
    char consulta[MAX_LINEA];
    while (1) {
        printf("\n> ");
        fflush(stdout);
        if (fgets(consulta, sizeof(consulta), stdin) == NULL) break;
        
        // Remover salto de línea
        consulta[strcspn(consulta, "\n")] = '\0';
        
        if (strcmp(consulta, "salir") == 0) {
            printf("Saliendo del programa.\n");
            break;
        }
        
        if (strlen(consulta) == 0) {
            printf("Por favor ingrese una consulta valida.\n");
            continue;
        }
        
        procesarConsulta(buscador, consulta);
    }
}

// Modo automático
void modoAutomatico(BuscadorDocumentos* buscador, const char* archivoConsultas) {
    printf("\n=== MODO ID (AUTOMATICO) ===\n");
    printf("Procesando consultas desde archivo: %s\n", archivoConsultas);
    printf("LIMITE: Se procesaran maximo %d consultas\n", MAX_CONSULTAS);
    
    FILE* fp = fopen(archivoConsultas, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo de consultas %s\n", archivoConsultas);
        return;
    }
    
    char consulta[MAX_LINEA];
    int numeroConsulta = 1;
    
    while (fgets(consulta, sizeof(consulta), fp) && numeroConsulta <= MAX_CONSULTAS) {
        // Remover salto de línea
        consulta[strcspn(consulta, "\n")] = '\0';
        
        if (strlen(consulta) > 0) {
            printf("\n--- Consulta %d ---\n", numeroConsulta);
            printf("Entrada: %s\n", consulta);
            
            char palabras[50][MAX_PALABRA];
            int numPalabras = extraerPalabras(buscador, consulta, palabras);
            
            if (numPalabras > 0) {
                NodoDocumento* resultados = buscarMultiplesPalabras(buscador, palabras, numPalabras);
                
                if (resultados == NULL) {
                    printf("Salida: No se encontraron documentos.\n");
                } else {
                    printf("Salida: ");
                    NodoDocumento* actual = resultados;
                    int primero = 1;
                    while (actual != NULL) {
                        if (!primero) printf(", ");
                        printf("doc%d", actual->id);
                        primero = 0;
                        actual = actual->siguiente;
                    }
                    printf("\n");
                }
                
                // Liberar memoria
                while (resultados != NULL) {
                    NodoDocumento* temp = resultados;
                    resultados = resultados->siguiente;
                    free(temp);
                }
            } else {
                printf("Salida: Consulta vacia o solo contiene stop words.\n");
            }
            
            numeroConsulta++;
        }
    }
    
    fclose(fp);
    
    if (numeroConsulta > MAX_CONSULTAS) {
        printf("\nSe alcanzo el limite de %d consultas.\n", MAX_CONSULTAS);
        printf("Total de consultas procesadas: %d\n", MAX_CONSULTAS);
    } else {
        printf("\nProcesamiento de consultas completado.\n");
        printf("Total de consultas procesadas: %d\n", numeroConsulta - 1);
    }
}

// Función para liberar memoria
void liberarMemoria(BuscadorDocumentos* buscador) {
    // Liberar documentos
    while (buscador->documentos != NULL) {
        Documento* temp = buscador->documentos;
        buscador->documentos = buscador->documentos->siguiente;
        free(temp);
    }
    
    // Liberar índice invertido
    while (buscador->indiceInvertido != NULL) {
        EntradaIndice* temp = buscador->indiceInvertido;
        buscador->indiceInvertido = buscador->indiceInvertido->siguiente;
        
        // Liberar lista de documentos de esta entrada
        while (temp->documentos != NULL) {
            NodoDocumento* docTemp = temp->documentos;
            temp->documentos = temp->documentos->siguiente;
            free(docTemp);
        }
        
        free(temp);
    }
    
    // Liberar stop words
    while (buscador->stopWords != NULL) {
        NodoStopWord* temp = buscador->stopWords;
        buscador->stopWords = buscador->stopWords->siguiente;
        free(temp);
    }
}

// Inicializar buscador
void inicializarBuscador(BuscadorDocumentos* buscador) {
    buscador->documentos = NULL;
    buscador->indiceInvertido = NULL;
    buscador->stopWords = NULL;
    buscador->totalDocumentos = 0;
    buscador->totalTerminos = 0;
    buscador->totalStopWords = 0;
}

int main(int argc, char* argv[]) {
    printf("=== BUSCADOR DE DOCUMENTOS CON INDICE INVERTIDO ===\n");
    
    BuscadorDocumentos buscador;
    inicializarBuscador(&buscador);
    
    if (argc < 2) {
        printf("\nUso del programa:\n");
        printf("  Modo Frecuencia (interactivo): ./busqueda <gov2_pages.dat>\n");
        printf("  Modo ID (automatico):          ./busqueda <gov2_pages.dat> <Log-Queries.dat>\n");
        return 1;
    }
    
    char* archivoDocumentos = argv[1];
    char* archivoStopWords = "stopwords_english.dat";
    
    printf("\nCargando sistema...\n");
    
    // Verificar si el archivo existe
    FILE* testFile = fopen(archivoDocumentos, "r");
    if (testFile == NULL) {
        printf("Error: No se puede acceder al archivo %s\n", archivoDocumentos);
        return 1;
    }
    fclose(testFile);
    
    // Cargar documentos y construir índice
    cargarDocumentos(&buscador, archivoDocumentos, archivoStopWords);
    
    // Verificar si se cargaron documentos
    if (buscador.totalDocumentos == 0) {
        printf("Error: No se cargaron documentos. Verificar formato del archivo.\n");
        liberarMemoria(&buscador);
        return 1;
    }
    
    // Mostrar estadísticas
    mostrarEstadisticas(&buscador);
    
    if (argc == 2) {
        // Modo interactivo
        modoFrecuencia(&buscador);
    } else if (argc == 3) {
        // Modo automático
        char* archivoConsultas = argv[2];
        modoAutomatico(&buscador, archivoConsultas);
    }
    
    // Liberar memoria
    liberarMemoria(&buscador);
    
    return 0;
}