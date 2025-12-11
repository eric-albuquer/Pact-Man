# Pact-Man 

**Pact-Man** é a versão alternativa de Pac-Man, onde o jogador deve explorar as profundezas dos círculos do Inferno de Dante.

O jogo mistura o arcade clássico do Pac-Man com um estilo exploratório moderno, invertendo os papéis tradicionais: agora o jogador é o fantasminha e deve fugir dos corrompidos Pacts-Man, enfrentar chefes infernais, coletar fragmentos de memória e derrotar o próprio diabo para escapar do inferno.

O jogo foi desenvolvido como projeto da disciplina de **Algoritmos e Estrutura de Dados (AED)** do curso de Ciência da Computação, 3º período, ministrada pela professora Natacha Targino, na CESAR School.

📺 **Vídeo Demonstrativo:** https://www.youtube.com/watch?v=YwxyXMzec_8

---

## Nossa Inspiração

Tivemos como inspiração principal o clássico arcade **Pac-Man**, marcado por décadas de presença nos fliperamas.  
Também nos baseamos no **Inferno de Dante Alighieri**, criando biomas temáticos, personagens e estruturas infernais que simulam os círculos descritos na obra.

---

## Objetivo do Jogo

O objetivo é sobreviver aos círculos do inferno e alcançar a saída.  
Para avançar, o jogador deve coletar **fragmentos de memória** — ao menos 2 dos 3 disponíveis por círculo — exceto no último nível.

Ao final, o jogador enfrentará o **Diabo**, derrotando-o para finalmente escapar para o céu.

---

## Como Funciona

### 🎮 Mecânica Principal

* Movimentação: **WASD**, **setas** ou **controle (PS3, PS4, PS5 e Xbox)**.
* Habilidade de corrida: **Barra de espaço** ou **R2 (controle)**.
* Três fragmentos de memória por círculo:
  - Coletar 100 moedas  
  - Derrotar o chefe  
  - Encontrar o fragmento escondido  
* Encostar em inimigos causa dano.
* A **estrelinha mágica** permite derrotar inimigos e chefes.
* Cada círculo possui **3 estruturas especiais**:
  - **Fonte** → Recupera vida  
  - **Templo** → Local de spawn do chefe  
  - **Altar da Velocidade** → Recarrega a habilidade de corrida  
* O jogo possui **3 dificuldades**:
  - Fácil  
  - Médio  
  - Difícil  
  *Os scores são comparados apenas com jogadores da mesma dificuldade.*
* Após o tempo limite da fase, ocorre a **desintegração** do mapa:
  - Buracos surgem  
  - Obstáculos aparecem  
  - O cenário torna-se mais letal  
* Poderes temporários:
  - Invisibilidade  
  - Invencibilidade  
  - Congelamento do tempo  
  - Regeneração  
  - Velocidade  

### 🔥 Círculos/Biomas

* Luxúria  
* Gula  
* Heresia  
* Violência  

Cada círculo possui layout, obstáculos e comportamentos únicos.  
A progressão para o próximo círculo é sempre **para a direita**.

### 💥 Desintegração

* Cada círculo possui **3 minutos antes de colapsar**.
* Após o timer, lacunas aparecem e causam dano ao jogador.

---

## Algoritmos e Estruturas de Dados Utilizados

O jogo faz uso intensivo de estruturas e algoritmos estudados na disciplina de AED:

- Hash Table (open addressing)  
- Hash Table (chaining)  
- Perfect Hashing (Minimal Perfect Hashing)  
- Prefix Tree (Trie)  
- Tree Node Pointer  
- Tree Search (DFS)  
- Graph Search (BFS)  
- Local Spatial Matrix  
- Sorting Network  
- Quick Sort  
- Merge Sort  
- LinkedList  
- Stack  
- Queue  
- Sliding Window  
- ArrayList  
- Bitwise Operations  

Essas técnicas foram aplicadas para otimizar desempenho, IA dos inimigos, reorganização de entidades, organização do mapa, busca de caminhos, filtragens, ordenações e mais.

---

## Instruções de Instalação

1.  **Instalar o MSYS2**
    * Baixe e instale o MSYS2 a partir do site oficial: [https://www.msys2.org/](https://www.msys2.org/)

2.  **Abrir o Terminal MSYS2**
    * Inicie o terminal **UCRT64** (MINGW64) a partir do menu Iniciar ou a instalação abre automaticamente.

3.  **Instalar Ferramentas de Compilação**
    * Execute o comando abaixo para baixar o GCC, Make e outras ferramentas essenciais:
    ```bash
    pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
    ```

4.  **Adicionar o GCC ao Path do Windows**
    * pesquise na barra de pesquisa do computador variaveis de ambiente, va até variáveis de ambiente, logo após acesse o PATH das variáveis do sistema, clique em editar, depois novo e adicione o diretório `bin` da sua instalação do mingw64 às variáveis de ambiente do Windows (por exemplo: `C:\msys64\ucrt64\bin`).

5.  **Instalar a Biblioteca Raylib**
    * Dentro do mesmo terminal ucrt64 , use o comando para instalar a versão correta da Raylib para o ambiente UCRT64:
    ```bash
    pacman -S mingw-w64-ucrt-x86_64-raylib
    ```

6.  **Clonar o Repositório**
    * Clone o repositório do projeto 
    ```bash
    git clone https://github.com/eric-albuquer/Pact-Man
    ```
    * Entre na pasta do projeto:
    ```bash
    cd Pact-Man
    ```

7.  **Compilar e Executar o Jogo**
    * Dentro da pasta do projeto, execute o seguinte comando para limpar compilações antigas, compilar o projeto e executá-lo:
    ```bash
    mingw32-make clean && mingw32-make run
    ```

</details>

<details>
<summary><b>Equipe</b></summary>

* **Lucas Menezes Santana**
    * Email: lms4@cesar.school
    * LinkedIn: [www.linkedin.com/in/lucasmenezes08](https://www.linkedin.com/in/lucasmenezes08)

* **Eric Albuquerque**
    * Email: ega2@cesar.school
    * LinkedIn: [https://www.linkedin.com/in/eric-albuquerque02](https://www.linkedin.com/in/eric-albuquerque02)

* **Amanda Luz**
    * Email: alc2@cesar.school
    * LinkedIn: [https://www.linkedin.com/in/amandaaluzc](https://www.linkedin.com/in/amandaaluzc)

* **Ricardo Sergio**
    * Email: rspff@cesar.school
    * LinkedIn: [https://www.linkedin.com/in/ricardospfreitas](https://www.linkedin.com/in/ricardospfreitas)

</details>

## 🔒 Tabela de Códigos

| Código | Teclado          | Controle                             | Efeito                  |
| ------ | ---------------- | ------------------------------------ | ----------------------- |
| **1**  | D, L, W, J, U, ␣ | R2, Right, Circle, Up, Square, L1    | **Invencibilidade**     |
| **2**  | K, I, U, O, S, ␣ | R2, X, Triangle, L1, R1, Down        | **Regeneração**         |
| **3**  | A, S, K, W, Q, S | Left, Down, X, Up, L2, Down          | **Invisibilidade**      |
| **4**  | U, A, S, W, K, K | L1, Left, Down, Up, X, X             | **Congelar o tempo**    |
| **5**  | W, Q, I, A, W, L | Up, L2, Triangle, Left, Up, Circle   | **Velocidade**          |
| **6**  | U, A, S, U, I, A | L1, Left, Down, L1, Triangle, Left   | **Adicionar fragmento** |
| **7**  | Q, U, ␣, J, U, W | L2, L1, R2, Square, L1, Up           | **Todos os efeitos**    |
| **8**  | Q, L, Q, S, Q, O | L2, Circle, L2, Down, L2, R1         | (indefinido)            |
| **9**  | ␣, J, D, S, A, D | R2, Square, Right, Down, Left, Right | (indefinido)            |
| **10** | O, S, I, ␣, S, J | R1, Down, Triangle, R2, Down, Square | (indefinido)            |
