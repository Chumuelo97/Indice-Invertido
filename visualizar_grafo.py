import networkx as nx
import matplotlib.pyplot as plt
import re

def visualizar_desde_archivo(ruta_archivo):
    G = nx.Graph()
    print(f"Leyendo el grafo desde '{ruta_archivo}'...")
    try:
        with open(ruta_archivo, 'r') as f:
            for linea in f:
                if not linea.strip():
                    continue
                partes = linea.strip().split()
                if len(partes) == 3 and all(p.isdigit() for p in partes):
                    u, v, peso = map(int, partes)
                else:
                    match = re.search(r'doc(\d+)\s*<->\s*doc(\d+):\s*(\d+)', linea)
                    if match:
                        u, v, peso = map(int, match.groups())
                    else:
                        continue
                G.add_edge(u, v, weight=peso)

    except FileNotFoundError:
        print(f"Error: No se encontró el archivo en '{ruta_archivo}'")
        return

    if G.number_of_nodes() == 0:
        print("No se encontraron datos de grafo válidos en el archivo.")
        return

    print(f"Grafo cargado: {G.number_of_nodes()} nodos y {G.number_of_edges()} aristas.")

    if G.number_of_nodes() > 200:
        print("\nAdvertencia: El grafo es muy grande (>200 nodos).")
        print("La visualización puede ser lenta y verse como una 'bola de pelos'.")
        print("Se mostrará un subconjunto de las aristas más pesadas.")
        
        aristas_pesadas = sorted(G.edges(data=True), key=lambda x: x[2]['weight'], reverse=True)[:300]
        nodos_filtrados = set()
        for u, v, _ in aristas_pesadas:
            nodos_filtrados.add(u)
            nodos_filtrados.add(v)
        
        H = G.subgraph(nodos_filtrados)
        G = H


    plt.figure(figsize=(15, 15))
    pos = nx.spring_layout(G, k=0.2, iterations=30)

    pesos = [G[u][v]['weight'] for u, v in G.edges()]
    ancho_aristas = [w * 2.5 / max(pesos) for w in pesos]
    nx.draw_networkx_nodes(G, pos, node_size=70, node_color='skyblue')
    nx.draw_networkx_edges(G, pos, width=ancho_aristas, alpha=1.0, edge_color='m')
    
    # 3. (Opcional) Dibujar los pesos como etiquetas en las aristas
    # Esto puede ser muy denso si hay muchas aristas.
    edge_labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=7)

    # --- FIN DE LA MODIFICACIÓN ---
    
    # Dibuja los IDs de los documentos como etiquetas en los nodos.
    nx.draw_networkx_labels(G, pos, font_size=8)

    plt.title("Visualización del Grafo de Co-Relevancia (Grosor por Peso)")
    plt.show()


# --- Ejecutar el script ---
# Asegúrate de que la ruta al archivo sea correcta
visualizar_desde_archivo("resultado/GrafoCoRelevancia.txt")