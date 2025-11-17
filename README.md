# Pact-Man 

<b>Pact-Man</b> é a versão alternativa de Pac-Man, onde o jogador deve explorar as profundezas dos círculos do Inferno de Dante. 

O jogo mistura o arcade clássico do Pac-Man com o novo estilo exploratório, que implementa uma inversão de personagens. Agora o player é o fantasminha, que deve fugir dos corrompidos Pacts-Man, enfrentar os chefes dos confins do inferno, coletar fragmentos de memória e derrubar o próprio diabo para escapar do inferno!

O jogo foi desenvolvido como projeto da disciplina de <b>Algoritmos e Estrutura de Dados</b> (AED) do curso de Ciência da Computação, 3º período, ministrada pela professora Natasha Targino, da CESAR School.

<details>
<summary><b>Nossa Inspiração</b></summary>

Tivemos como inspiração principal o clássico jogo arcade Pac-Man, que teve presença em inúmeros fliperamas nos anos 90, marcou gerações e até os dias de hoje é jogado com frequência. 

Além disso, nos baseamos no Inferno de <b>Dante Alighieri</b> para caracterizar a proposta de ser um jogo exploratório ambientado nos círculos do inferno com suas respectivas particularidades, personagens e características que contribuem para a mistura da obra literária com o core central do jogo virtual.

</details>

<details>
<summary><b>Objetivo do Jogo</b></summary>

O objetivo principal do jogo é sobreviver e alcançar a saída do inferno, para isso, é necessário a coleta de fragmentos de memória, no qual possibilitam a passagem de círculo para círculo. Dessa maneira, ao coletar fragmentos e atravessar os círculos. Ao chegar no ultimo, encontrará o vilão final, o próprio diabo, em que deverá sobreviver e lutar bravamente para alcançar a saída do inferno e chegar ao céu, feito que ninguém, até agora, conseguiu. Será que você consegue?

</details>

<details>
<summary><b>Como Funciona</b></summary>

### Mecânica Principal

* O jogador pode movimentar pelo mapa através das teclas WASD ou pelas setas.


* O jogo terá 3 fragmentos de memória por circulo, sendo necessário a coleta de pelo menos 2 para ir para o próximo nivel, isso é global para todas as fases, com exceção do ultimo.


* Para coletar fragmentos, você deve
    * Coletar 100 moedas 
    * Derrotar o chefão do Círculo 
    * Encontrar o fragmento escondido no mapa.


* O jogador deve evitar que os inimigos encostem, caso isso aconteça, perderá vida.


* Para atacar inimigos ou derrotar o chefão, o jogador pode pegar a estrelinha mágica, que permite derrotar inimigos ao encostar neles.


* O jogo tem tempo pré definido por fase, ao terminar o tempo da fase, o mapa começará a desintegrar, partes obscuras irão surgir e causar dano durante o colapso.


* O jogador irá dispor de poderes , disponíveis em formato de itens coletáveis no mapa. Os poderes duram um tempo limitado.
* É importante frisar que para der
rotar o chefão, é necessário exclusivamente consumir a estrelinha mágica.
* Cada círculo terá uma fonte de vida, ao entrar no espaço delimitado o jogador recupera vida.
* Existirá o coletável de vida distribuido pelo mapa, seu consumo gera retorno de vida.

### Círculos/Biomas

* Os circulos são 4: Luxúria, Gula , Heresia, Violência, cada um com sua respectiva particularidade e estratégia para passar.

* O formato de mapa e obstaculos variam de acordo com o circulo.

* Existem circulos tão específicos que o jogador precisa elaborar estratégias restritas para cada circulo que for enfrentar.

* O sentido para atravessar de um circulo para o outro é sempre para a direita.

### Desintegração

* Cada circulo possui 3 minutos até ser degenerado.
* A partir do momento que o tempo finaliza, o mapa passa por um processo de desintegração, lacunas irão surgir, e caso o jogador encoste, perderá vida.

### Poderes

* Os poderes duram por tempo limitado.
* Os poderes dão vantagem no mapa, seu consumo é opcional, com exceção da estrela mágica que permite derrotar o boss.

* Entre os poderes alguns são: 
    * Invisibilidade
    * Intangibilidade
    * Congelamento do tempo
    * Estrela do Mário: permite matar os inimigos

</details>

<details>
<summary><b>Instruções de Instalação</b></summary>

Siga estes passos para configurar o ambiente e executar o projeto:

1.  *Instalar o MSYS2*
    * Baixe e instale o MSYS2 a partir do site oficial: [https://www.msys2.org/](https://www.msys2.org/)

2.  *Abrir o Terminal MSYS2*
    * Inicie o terminal *UCRT64* (MINGW64) a partir do menu Iniciar.

3.  *Instalar Ferramentas de Compilação*
    * Execute o comando abaixo para baixar o GCC, Make e outras ferramentas essenciais:
    bash
    pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
    

4.  *Adicionar o GCC ao Path do Windows*
    * Adicione o diretório bin da sua instalação do mingw64 às variáveis de ambiente do Windows (Adicione esse path:  C:\msys64\ucrt64\bin) as variaveis do sistema.

5.  *Instalar a Biblioteca Raylib*
    * Use o pacman para instalar a versão correta da Raylib para o ambiente UCRT64:
    bash
    pacman -S mingw-w64-ucrt-x86_64-raylib
    

6.  *Clonar o Repositório*
    * Clone o repositório do projeto
    bash
    git clone https://github.com/eric-albuquer/Pact-Man
    

7.  *Abra o cmd para executar o comando a seguir*
    * Entre na pasta do projeto:
    bash
    cd Pact-Man
    

8.  *Compilar e Executar o Jogo*
    * Dentro da pasta do projeto, execute o seguinte comando para limpar compilações antigas, compilar o projeto e executá-lo:
    bash
    mingw32-make clean && mingw32-make run
    

</details>

<details>
<summary><b>Equipe</b></summary>


* *Amanda Luz*
    - Email: alc2@cesar.school
    - LinkedIn: [https://www.linkedin.com/in/amandaaluzc](https://www.linkedin.com/in/amandaaluzc)

* *Eric Albuquerque*
    - Email: ega2@cesar.school
    - LinkedIn: [https://www.linkedin.com/in/eric-albuquerque02](https://www.linkedin.com/in/eric-albuquerque02)

* *Lucas Menezes Santana*
    - Email: lms4@cesar.school
    - LinkedIn: [www.linkedin.com/in/lucasmenezes08](https://www.linkedin.com/in/lucasmenezes08)

* *Ricardo Freitas*
    - Email: rspff@cesar.school
    - LinkedIn: [https://www.linkedin.com/in/ricardospfreitas](https://www.linkedin.com/in/ricardospfreitas)

</details>