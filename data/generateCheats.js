const keys = "WASD QIKLJUO";
const gamepadKeys = ["UP", "LEFT", "DOWN", "RIGHT", "R2", "L2", "TRIANGLE", "X", "CIRCLE", "SQUARE", "L1", "R1"]
const effects = ["Invencibilidade", "Regeneração", "Invisibilidade", "Congelar", "Velocidade", "Adicionar fragmento"]
const codes = 10
const keysPerCode = 6

for (let i = 1; i <= codes; i++) {
    let str = ""
    let gamepadStr = ""
    for (let j = 0; j < keysPerCode; j++) {
        const idx = Math.floor(Math.random() * keys.length)
        str += keys[idx] + ((j < keysPerCode - 1) ? "," : "")
        gamepadStr += (gamepadKeys[idx] + ((j < keysPerCode - 1) ? ", " : ""))
    }
    console.log(str + ":" + i + " / " + gamepadStr + " // " + effects[i - 1])
}